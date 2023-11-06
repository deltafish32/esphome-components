# Roode

[한국어](README.md)
[English](README_en_US.md)

## 개요
VL53L1X 기반의 카운터 센서 ESPHome External Components 입니다. [Roode](https://github.com/Lyr3x/Roode)를 포크하여 제작되었습니다.

그러나 치명적인 문제가 있는데, 장시간 사용시 간헐적으로 먹통이 되는 문제가 있었습니다. 이 문제를 약 6개월간 연구하여 원인과 해결 방법을 찾았습니다. 필자의 오픈소스 프로젝트 경험과 영어 실력이 부족하여, 부득이하게 소스를 포크하는 방법을 선택했습니다.


### 원인
원인은 단순합니다. VL53L1X_ULD 의 VL53L1X::read_distance 함수 내부에서 실패시 무한 루프에 빠지며 발생하는 문제입니다.


### 증상
통신이 되지 않습니다. Home Assistant API, OTA 모두 되지 않으나, 특이하게도 PING 은 응답이 옵니다.

센서 동작도 되지 않으며 로컬 자동화 (1초 주기 LED Blink 와 같은 단순한 동작으로 확인) 또한 동작하지 않습니다. 


### 해결 방법
문제 발생시 XSHUT 핀을 이용하여 센서를 초기화시키면 해결됩니다.


## 설정
나머지 배선은 동일하게 하면 되며, XSHUT는 제약 없는 아무 출력 GPIO를 사용하셔도 무방합니다.

- [Roode의 배선 도움말](https://github.com/Lyr3x/Roode#wiring)
- [ESP32 핀맵](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [ESP8266 핀맵](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/)

```yaml
external_components:
  - source: github://deltafish32/esphome-components
    refresh: always

(중략)

vl53l1x:
  (중략)
  pins:
    xshut: GPIO32
```


## 보너스
Roode 사용시 성능/편의성 개선 팁입니다.

- [문틀 장착시 인식률 개선](bonus_ranging.md)
- [자동화 편의](bonus_automation.md)


## 출처
- <https://github.com/Lyr3x/Roode>
- <https://randomnerdtutorials.com/esp32-pinout-reference-gpios/>
- <https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/>
