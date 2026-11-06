// LD2450 bring-up: find report frames on the UART and print them as hex.
// Decoding the target fields is the next step (see README roadmap), so it is
// left out on purpose. Arduino IDE Tools settings are listed in README.md.

#include <Arduino.h>

// Wiring (see docs/hardware.md). LD2450 TX goes to the ESP32 RX pin.
constexpr int RADAR_RX_PIN = 47;  // ESP32 RX  <- LD2450 TX
constexpr int RADAR_TX_PIN = 21;  // ESP32 TX  -> LD2450 RX
constexpr uint32_t RADAR_BAUD = 256000;  // LD2450 default, 8N1

// Report frame: 4-byte header, 3 target slots of 8 bytes, 2-byte tail.
constexpr uint8_t FRAME_HEADER[4] = {0xAA, 0xFF, 0x03, 0x00};
constexpr uint8_t FRAME_TAIL[2] = {0x55, 0xCC};
constexpr size_t FRAME_LEN = 30;

constexpr uint32_t STATS_PERIOD_MS = 5000;

HardwareSerial radar(1);

uint8_t frame[FRAME_LEN];
size_t frame_pos = 0;  // number of bytes collected for the current frame

uint32_t frames_ok = 0;
uint32_t frames_bad_tail = 0;
uint32_t last_stats_ms = 0;

void print_frame(const uint8_t *buf, size_t len) {
  for (size_t i = 0; i < len; i++) {
    Serial.printf("%02X ", buf[i]);
    // Separators between the header, the three target slots and the tail.
    if (i == 3 || i == 11 || i == 19 || i == 27) {
      Serial.print("| ");
    }
  }
  Serial.println();
}

// Feed one byte. The first 4 bytes must match the header, then the rest of
// the frame is collected and the tail is checked.
void feed(uint8_t b) {
  if (frame_pos < sizeof(FRAME_HEADER)) {
    if (b == FRAME_HEADER[frame_pos]) {
      frame[frame_pos++] = b;
    } else {
      // Restart. This simple restart is enough because 0xAA appears only once
      // in the header, so a mismatch can only be the start of a new header.
      frame_pos = 0;
      if (b == FRAME_HEADER[0]) {
        frame[frame_pos++] = b;
      }
    }
    return;
  }

  frame[frame_pos++] = b;
  if (frame_pos < FRAME_LEN) {
    return;
  }

  if (frame[FRAME_LEN - 2] == FRAME_TAIL[0] && frame[FRAME_LEN - 1] == FRAME_TAIL[1]) {
    frames_ok++;
    print_frame(frame, FRAME_LEN);
  } else {
    frames_bad_tail++;
  }
  frame_pos = 0;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  radar.begin(RADAR_BAUD, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
  Serial.printf("LD2450 frame sniffer: RX=GPIO%d TX=GPIO%d %lu baud\n", RADAR_RX_PIN,
                RADAR_TX_PIN, (unsigned long)RADAR_BAUD);
  last_stats_ms = millis();
}

void loop() {
  while (radar.available() > 0) {
    feed((uint8_t)radar.read());
  }

  uint32_t now = millis();
  if (now - last_stats_ms >= STATS_PERIOD_MS) {
    float seconds = (now - last_stats_ms) / 1000.0f;
    Serial.printf("-- %.1f frames/s, ok %lu, bad tail %lu\n", frames_ok / seconds,
                  (unsigned long)frames_ok, (unsigned long)frames_bad_tail);
    if (frames_ok == 0) {
      Serial.println("-- no frames: check 5V power, TX/RX swap, and baud rate");
    }
    frames_ok = 0;
    frames_bad_tail = 0;
    last_stats_ms = now;
  }
}
