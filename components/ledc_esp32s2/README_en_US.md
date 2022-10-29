# ledc_esp32s2

[한국어](README.md)
[English](README_en_US.md)

## Overview
ESPHome External Components for `ledc` Component that is compatible with `ESP32-S2`.

The timer frequency of `ESP32` is up to `20-bits`, but `ESP32-S2` only supports up to `14-bits`. The `ledc` component incorrectly recognizes it as `ESP32` and does not operate normally. Using this Component solves it.


## Configuration
example:

```yaml
external_components:
  - source: github://deltafish32/esphome-components
    refresh: always

output:
  - platform: ledc_esp32s2
    pin: 21
    id: rtttl_out
```

Just modify `ledc` to `ledc_esp32s2`.



## Trivia
