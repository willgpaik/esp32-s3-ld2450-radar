# esp32-s3-ld2450-radar

The HLK-LD2450 24 GHz mmWave radar on an ESP32-S3: a frame parser, a live view of tracked targets in the browser, and measured tests of how well the radar actually works.

> Status: early setup. The sketch finds report frames on the UART and prints them as hex. It compiles with Arduino-ESP32 3.3.12 but has not been run on hardware yet.

## Why measure it

The LD2450 gets mixed reviews. It tracks moving people well but drops people who sit still, and it can report ghost targets from fans, curtains and reflections. Most reports are anecdotal. This repo puts numbers on those claims: position jitter at known spots, detection rate by distance, and ghost rate with common interference sources.

## Hardware

| Part | Spec |
|---|---|
| MCU board (Freenove) | ESP32-S3, dual-core 240 MHz, 8 MB flash, 8 MB octal PSRAM |
| Radar | HLK-LD2450 (V1.1), 24 GHz FMCW, up to 3 targets with X/Y and speed, about 6 m, ±60° azimuth, UART 256000 baud, 5 V supply |

Wiring:

```
LD2450      ESP32-S3
------      --------
5V       -> 5V
GND      -> GND
TX       -> GPIO47  (ESP32 RX, UART1)
RX       <- GPIO21  (ESP32 TX, UART1)
```

Pin notes, power, mounting and known limitations: [docs/hardware.md](docs/hardware.md)
Frame format: [docs/protocol.md](docs/protocol.md)

## Setup

1. Install Arduino IDE 2.x.
2. Add this URL in **File > Preferences > Additional boards manager URLs**:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. In **Boards Manager**, install **esp32 by Espressif Systems** 3.x (checked with 3.3.12).
4. Set the **Tools** menu:

   | Setting | Value |
   |---|---|
   | Board | ESP32S3 Dev Module |
   | Flash Size | 8MB (64Mb) |
   | PSRAM | OPI PSRAM |
   | Partition Scheme | 8M with spiffs (3MB APP/1.5MB SPIFFS) |
   | USB CDC On Boot | Disabled (when using the USB-to-serial port) |

   These settings also work on the 16 MB flash version of the board. It will only use the first 8 MB.

## Build and upload

From Arduino IDE: open the sketch, check the Tools settings, then **Upload**. Open the Serial Monitor at 115200 baud.

From the command line with `arduino-cli`:

```sh
FQBN=esp32:esp32:esp32s3:FlashSize=8M,PSRAM=opi,PartitionScheme=default_8MB
arduino-cli compile --fqbn $FQBN .
arduino-cli upload  --fqbn $FQBN -p <PORT> .
```

Expected output with the radar powered and someone walking in front of it:

```
LD2450 frame sniffer: RX=GPIO47 TX=GPIO21 256000 baud
AA FF 03 00 | xx xx xx xx xx xx xx xx | 00 00 00 00 00 00 00 00 | 00 00 00 00 00 00 00 00 | 55 CC
...
-- 10.0 frames/s, ok 50, bad tail 0
```

About 10 frames per second means the wiring, power and baud rate are right. If the count is 0, the sketch prints a hint.

## Roadmap

- [ ] 0. Bring-up: find report frames on the UART and print them (this sketch)
- [ ] 1. Decode one real frame by hand: X, Y, speed, resolution
- [ ] 2. Parser as a small C module, separate from the Arduino code, so it can also be used from ESP-IDF
- [ ] 3. Host-side unit test for the parser, starting with the example frame from the protocol document
- [ ] 4. Live top-down view of targets in the browser over Wi-Fi
- [ ] 5. Measurements
  - position jitter while standing at marked spots
  - detection rate by distance and angle
  - sitting still: how long until the target drops
  - ghost targets with a fan or curtain in view
- [ ] 6. Write up the results in `docs/results.md`
- [ ] 7. Commands: read the firmware version, switch single / multi target mode (optional)

## Repository layout

```
.
├── esp32-s3-ld2450-radar.ino   sketch entry point
├── docs/
│   ├── hardware.md
│   └── protocol.md
└── .gitignore
```

Planned:

- `src/ld2450/`: the parser (Arduino IDE also compiles the `src/` folder of a sketch)
- `tests/`: host-side tests for the parser
- `captures/`: small recorded radar streams for replay and tests

## License

MIT. See [LICENSE](LICENSE).

Third party code keeps its own license. Each one will be listed here as it is added.
