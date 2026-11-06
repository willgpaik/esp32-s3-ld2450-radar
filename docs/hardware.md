# Hardware

## Parts

| Part | Spec |
|---|---|
| ESP32-S3 board with camera connector (Freenove) | Dual-core 240 MHz, 8 MB flash, 8 MB octal PSRAM. 5V and 3V3 pins on the header. |
| HLK-LD2450 (hardware V1.1) | 24 GHz FMCW radar, 1 TX / 2 RX antennas. Tracks up to 3 moving targets with X/Y position and speed. Range about 6 m, azimuth ±60°, pitch ±35°. 5 V supply (more than 200 mA), 3.3 V UART I/O, 256000 baud 8N1, 10 reports per second. Bluetooth for the HLKRadarTool app. |

The LD2450 has a JST-ZH compatible connector and a **2.0 mm** pin header. Standard 2.54 mm jumper wires fit loosely on 2.0 mm pins, so use the cable that comes with the module or 2.0 mm to 2.54 mm jumpers.

## Wiring

```
LD2450      ESP32-S3
------      --------
5V       -> 5V
GND      -> GND
TX       -> GPIO47  (ESP32 RX, UART1)
RX       <- GPIO21  (ESP32 TX, UART1)
```

- TX goes to RX. If no frames show up, swap these two first.
- Both sides use 3.3 V logic, so no level shifter is needed.
- The 5V pin on the board comes from USB. If detection range looks short (only 1 to 2 m), try a separate 5 V supply with a shared GND. Users have reported this on underpowered setups.
- GPIO41/GPIO42 are also free if GPIO47/GPIO21 are taken. They are JTAG pins, which only matters when an external JTAG probe is used.

## GPIOs on this board

From the Freenove pinout for this board:

| Function | GPIO |
|---|---|
| USB-to-serial (UART0, used by Serial Monitor) | 43 (TX), 44 (RX) |
| Onboard LED | 2 |
| Camera | 4 to 13, 15 to 18 |
| Octal PSRAM (not usable when PSRAM is on) | 35, 36, 37 |
| microSD | 38, 39, 40 |
| USB D- / D+ | 19 / 20 |
| Strapping pins (avoid) | 0, 3, 45, 46 |
| WS2812 RGB LED | 48 |
| **Free** | 1, 2, 14, 21, 41, 42, 47 |

GPIO1/GPIO2 are kept for an I2C sensor (an MLX90642 thermal sensor in another project). The LD2450 on GPIO47/GPIO21 does not overlap, so both can run on this board at the same time. GPIO2 also drives the onboard LED. I2C has worked on it at 400 kHz, but if I2C errors appear, that LED is the first suspect.

## Firmware

- Check the radar firmware version with the HLKRadarTool app right after unboxing. ESPHome asks for V2.02.23090617 or newer, which is a useful reference point.
- The Bluetooth interface is reported to stay open only for about 30 seconds after power-on.
- There is no official rollback. Update only when there is a reason to.

## Mounting

- Hi-Link and Screek recommend a mounting height of 1.5 to 2 m.
- Keep ceiling fans, swinging curtains and large reflective surfaces out of the detection area. They are known sources of ghost targets.
- Do not let the detection areas of two radars overlap.

## Known limitations

These come from user reports and vendor FAQs, and they are the things this repo plans to measure:

- **Stationary people.** The LD2450 is a moving-target tracker. A person who sits completely still may drop out. Sleeping people are very hard to keep.
- **Ghost targets** from reflections, fans and curtains. Pets can be reported as people.
- **Short dropouts.** The radar can report "no target" for a single frame. Automations usually add a debounce.
- **Position drift.** The X/Y grid can be warped, and points may jump once in a while.
- **Through-wall detection.** 24 GHz passes through drywall and thin partitions.

## References

- Hi-Link LD2450 product page: https://www.hlktech.net/index.php?id=1182
- LD2450 instruction manual: https://www.tinytronics.nl/product_files/006000_HLK-LD2450-Instruction-Manual.pdf
- LD2450 serial protocol v1.03: https://make.net.za/wp-content/datasheets/HLK%20LD2450%20Serial%20Communication%20Protocol%20v1.03.pdf
- Screek 2A (LD2450) FAQ: https://screek.io/2a/usage-faq
- ESPHome LD2450: https://esphome.io/components/sensor/ld2450/
- Freenove board repo (pinout PDF): https://github.com/Freenove/Freenove_ESP32_S3_WROOM_Board
