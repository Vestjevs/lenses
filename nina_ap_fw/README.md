# NINA-W102 SoftAP firmware (ESP-IDF)

ESP32 firmware for u-blox NINA-W102 that exposes a simple UART protocol to configure/start a Wi-Fi SoftAP. The STM32 sends commands over UART; the module replies with text responses.

- Target: ESP32 (NINA-W102)
- SDK: ESP-IDF 5.1+
- Default: channel 1; WPA2-PSK if password is non-empty; OPEN if password is empty

## Build and Flash

1. Install ESP-IDF 5.1+ (or use the Docker image `espressif/idf:release-v5.1`).
2. Set target and configure (optional):
   ```sh
   idf.py set-target esp32
   idf.py menuconfig   # to change UART pins/port/baud if needed
   ```
3. Build and flash:
   ```sh
   idf.py build
   idf.py -p /dev/ttyUSB0 flash monitor
   ```

Default UART settings to communicate with STM32:
- Port: UART1
- TX (ESP32->STM32): GPIO17
- RX (STM32->ESP32): GPIO16
- Baud: 115200

Adjust via `menuconfig` under: `NINA AP UART settings`.

## UART Protocol

ASCII line-oriented protocol with CRLF terminators. Module transmits a single `READY` line on boot.

- `READY NINA-AP v1` — sent once after boot.
- `PING` → `PONG`
- `AP SET "<ssid>" "<pwd>"` → `OK` or `ERR <code> <message>`
  - Channel fixed to 1.
  - If `<pwd>` is empty string `""`, the AP is open (no password).
  - WPA2-PSK if password length is 8..64.
- `AP STOP` → `OK`
- `AP STATUS` → `STATUS state=<RUNNING|STOPPED> ssid="..." ip=<x.x.x.x> ch=1 clients=<n>`

Notes:
- SSID up to 32 bytes, password 8..64 for WPA2-PSK. Use `""` to set OPEN network.
- Strings are quoted with double quotes. Avoid embedding double quotes in SSID/password.

## STM32 example (pseudo-code)

```c
send_line("PING\r\n"); // expect PONG
send_line("AP SET \"MyAP\" \"MySecretPwd\"\r\n"); // expect OK
send_line("AP STATUS\r\n"); // read STATUS ...
```

## License
MIT