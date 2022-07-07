# climate_ir_lg_ex

[한국어](README.md)
[English](README_en_US.md)

## Overview
ESPHome External Components for LG air conditioner.

I modified `climate_ir_lg` to my liking.

changelog
- remove heat_cool
- add fan_only mode
- modify cool signal
- always display target temperature to current temperature



## Configuration
example:

```yaml
external_components:
  - source: github://deltafish32/esphome-components/climate_ir_lg_ex
    refresh: always

remote_transmitter:
  pin: D2
  carrier_duty_percent: 50%

climate:
  - platform: climate_ir_lg_ex
    name: "Bedroom Ac"
    id: bedroom_ac
    supports_cool: true
    supports_heat: false
    visual:
      temperature_step: 1.0
```


## Trivia
I verified it with the `SQ07B9PWBJ.AKOR` among LG air conditioner 2in1.

I used CL-15L (SI5312-H) IR LED, 2N2222 NPN transtor and 0Ω resistor.
