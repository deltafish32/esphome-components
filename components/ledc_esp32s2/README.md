# ledc_esp32s2

[한국어](README.md)
[English](README_en_US.md)

## 개요
`ESP32-S2` 에서 사용 가능한 `ledc` Component 입니다.

`ESP32` 의 Timer 주파수는 최대 `20-bits` 이나, `ESP32-S2` 는 최대 `14-bits` 만 지원합니다. `ledc` Component 에서 `ESP32` 로 잘못 인식하여 정상 동작하지 않습니다. 이 Component 를 사용하면 해결됩니다.



## 설정
예제:

```yaml
external_components:
  - source: github://deltafish32/esphome-components
    refresh: always

output:
  - platform: ledc_esp32s2
    pin: 21
    id: rtttl_out
```

`ledc` 대신 `ledc_esp32s2` 로 변경만 하면 됩니다.



## 기타
