# ESP32 audio transmitter firmware

```bash
source get_idf.sh
idf.py menuconfig
idf.py build
idf.py -p /dev/ttyUSB0 flash
idf.py monitor

# Compile app code only, flash and monitor
idf.py -p /dev/ttyUSB0 app-flash monitor
```
