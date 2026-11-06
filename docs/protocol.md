# LD2450 serial protocol notes

Summary of the Hi-Link LD2450 serial protocol (v1.03), written for this project. The original document is linked at the bottom.

## UART

- 256000 baud, 8 data bits, no parity, 1 stop bit
- Little-endian for all multi-byte fields
- The radar sends report frames on its own, 10 times per second

## Report frame (30 bytes)

```
offset  size  field
0       4     header      AA FF 03 00
4       8     target 1
12      8     target 2
20      8     target 3
28      2     tail        55 CC
```

Unused target slots are all zeros.

### Target slot (8 bytes)

```
offset  size  field                 unit
0       2     X                     mm
2       2     Y                     mm
4       2     speed                 cm/s
6       2     distance resolution   mm (uint16)
```

**Sign convention for X, Y and speed is not two's complement.** The highest bit is the sign, and it is **1 for positive, 0 for negative**. The lower 15 bits are the magnitude.

Example slot from the protocol document, for practice decoding by hand:

```
0E 03 B1 86 10 00 40 01
```

## Command frames

Commands and their replies use a different frame:

```
header   FD FC FB FA
length   2 bytes, little-endian (command word + value)
command  2 bytes
value    N bytes
tail     04 03 02 01
```

The reply (ACK) echoes the command word and adds a 2-byte status (0 = success, 1 = failure) before any returned data.

Commands are not used yet. Candidates for later: read the firmware version, switch single / multi target mode.

## Reference

- HLK-LD2450 Serial Communication Protocol v1.03: https://make.net.za/wp-content/datasheets/HLK%20LD2450%20Serial%20Communication%20Protocol%20v1.03.pdf
