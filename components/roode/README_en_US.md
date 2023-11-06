# Roode

[한국어](README.md)
[English](README_en_US.md)

## Overview
People Counter ESPHome External Components. Made by forking this source, [Roode](https://github.com/Lyr3x/Roode).

Roode has a problem, It goes down when used for a long time. I researched this problem for 6 months to find the cause and solution. I don't have a lot of open source experience and I am not good at English, so I forked the source.


### Causes
It's simple. When VL53L1X::read_distance function in VL53L1X_ULD fails, it gets stuck in an infinite loop.


### Symptoms
Cannot communicate include Home Assistant API, OTA, however, the ping response works normally.

Sensor does not work, Local Automation also does not work too (like 1s period LED blink).


### Solutions
Initialize using XSHUT when a problem occurs.


## Settings
The rest of the wiring is the same, XSHUT can wiring any Output GPIO without restrictions.

- [Wiring Roode](https://github.com/Lyr3x/Roode#wiring)
- [Pinout of ESP32](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [Pinout of ESP8266](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/)

```yaml
external_components:
  - source: github://deltafish32/esphome-components
    refresh: always

...

vl53l1x:
  ...
  pins:
    xshut: GPIO32
```


## Bonus
Tips when using Roode.

English translation is WIP.

- [Improved accuracy when used on door frames](bonus_ranging.md)
- [Easy Automation](bonus_automation.md)


## References
- <https://github.com/Lyr3x/Roode>
- <https://randomnerdtutorials.com/esp32-pinout-reference-gpios/>
- <https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/>
