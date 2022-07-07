# climate_ir_lg_ex

[한국어](README.md)
[English](README_en_US.md)

## 개요
LG 에어컨을 위한 ESPHome External Components 입니다.

`climate_ir_lg` 에서 불합리한 부분을 수정했습니다.

수정 사항
- 냉난방 기능 삭제
- 송풍 기능 추가
- 냉방 신호 수정 (자세한 내용은 코드 참고)
- 현재 온도를 설정 온도로 표시



## 설정
예제:

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


## 기타
LG 에어컨 2in1 중 벽걸이형 `SQ07B9PWBJ.AKOR` 모델에서 확인했습니다.

CL-15L (SI5312-H) IR LED, 2N2222 NPN 트랜지스터, 0Ω 저항을 사용했습니다.
