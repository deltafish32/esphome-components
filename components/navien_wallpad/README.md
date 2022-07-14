# Navien_Wallpad

[한국어](README.md)
[English](README_en_US.md)

## 개요
**주의: 이 components 는 현재 제작중이므로 완벽하지 않습니다. 잘못된 동작이나 불안정한 동작을 할 수 있으며, 언제든 기능 변경이 있을 수 있습니다.**

경동 나비엔 월패드를 위한 ESPHome External Components 입니다. EW11 대비 저렴한 RS485 컨버터와 ESP 보드로 구성할 수 있습니다.

제가 쓰려고 만들었습니다. 우리집에 있는 기능만 지원되며, 모든 경동 나비엔 월패드에 동작을 보장하지 않습니다.

[KSX4506 프로토콜](https://standard.go.kr/KSCI/ksNotification/getKsNotificationView.do?ntfcManageNo=2016-00079&menuId=921&topMenuId=502)을 사용하였습니다.


## 지원 기능

| 기능 | 지원 | 비고 |
| - | :-: | - |
| 시스템 에어컨 | ❌ | |
| 전자레인지 | ❌ | |
| 식기세척기 | ❌ | |
| 드럼세탁기 | ❌ | |
| 조명 | ✅ | |
| 가스밸브 | ✅ | 닫기만 지원 |
| 커튼 | ❌ | |
| 원격검침기 | ✅ | |
| 도어락 | ❌ | |
| 실내환기 시스템 | ❌ | |
| 일괄차단기 | ✅ | |
| 방범확장 | ❌ | |
| 보일러 | ❌ | |
| 온도조절기 | ✅ | 난방/외출 |
| ZigBee 모듈 | ❌ | |
| 스마트 전력량계 | ❌ | |
| 대기전력 차단기기 | ❌ | |
| 엘리베이터 | ❌ | 별도의 네트워크를 사용하여 불가 |
| 현관모션 | ❌ | 별도의 회로 구성시 가능 |
| 인터폰 | ❌ | 별도의 네트워크를 사용하여 불가 |


### 조명
On/Off 및 상태 확인이 지원됩니다.

### 온도조절기
On(난방)/Off(외출) 및 상태 확인이 지원됩니다. 온도는 1도 단위로 조정되며, (지원하는 하드웨어의 경우) 옵션 활성화시 0.5도 단위 조정도 가능합니다.

### 가스밸브
닫기 및 상태 확인이 지원됩니다. 열기는 프로토콜 자체가 지원하지 않아 불가합니다. 하드웨어적으로는 가능한 것으로 보이나, 안전을 위한 것으로 추정됩니다.

### 일괄차단기(일괄소등)
On/Off 및 상태 확인이 지원됩니다.

### 원격검침기
| 기능 | 지원 | 비고 |
| - | :-: | - |
| 수도-현재 | ✅ | |
| 수도-전체 | ✅ | |
| 가스-현재 | ❌ | |
| 가스-전체 | ✅ | |
| 전력-현재 | ✅ | |
| 전력-전체 | ✅ | |


## 하드웨어
ESP32 혹은 ESP8266 에 RS485 to TTL 을 연결합니다.

RS485 는 원하시는 곳 어디든 연결하셔도 좋습니다. 아래와 같은 곳에 있을 수 있습니다.

- **월패드 뒤쪽 (권장)**
- 싱크대 아래
- 주방 TV


## 설정
### 필수

```yaml
external_components:
  - source: github://deltafish32/esphome-components
    refresh: always

uart:
  id: uart_bus
  tx_pin: D2
  rx_pin: D1
  baud_rate: 9600
  rx_buffer_size: 1024
  
navien_wallpad:
  uart_id: uart_bus
```

이제, 필요한 항목을 하나씩 추가하시면 됩니다.

### 스캔

조명과 온도조절기의 갯수와 종류를 파악할 수 있습니다.

```yaml
  wallpad_scan:
    name: "Wallpad Scan"
```

ESPHome Log 창을 켜놓은 상태로 추가된 버튼을 누르면 스캔을 진행합니다. 시간이 약간 걸립니다.

스캔에 성공한다면 아래와 같이 표시됩니다.

```log
[00:00:00][I][Navien_Wallpad:461]: Light Scan Result: Group: 1, Total: 3 (Binary: 3, Dimmable: 0)
[00:00:00][I][Navien_Wallpad:464]:   List:
[00:00:00][I][Navien_Wallpad:466]:     0x11: Binary
[00:00:00][I][Navien_Wallpad:466]:     0x12: Binary
[00:00:00][I][Navien_Wallpad:466]:     0x13: Binary
[00:00:00][I][Navien_Wallpad:604]: Thermostat Scan Result: Group: 1, Total: 4
[00:00:00][I][Navien_Wallpad:605]:   Maker: 0x00
[00:00:00][I][Navien_Wallpad:606]:   Type: Air (0x01)
[00:00:00][I][Navien_Wallpad:610]:   Temperature Max: 40
[00:00:00][I][Navien_Wallpad:611]:   Temperature Min: 5
[00:00:00][I][Navien_Wallpad:612]:   Support 0.5: no
[00:00:00][I][Navien_Wallpad:613]:   Support Reserve: yes
[00:00:00][I][Navien_Wallpad:614]:   Support Hot Water: no
[00:00:00][I][Navien_Wallpad:615]:   Support Outing: yes
[00:00:00][I][Navien_Wallpad:616]:   List:
[00:00:00][I][Navien_Wallpad:618]:     0x11
[00:00:00][I][Navien_Wallpad:618]:     0x12
[00:00:00][I][Navien_Wallpad:618]:     0x13
[00:00:00][I][Navien_Wallpad:618]:     0x14
```

List 부분에 0x11, 0x12 와 같이 표시된 ID 를 아래의 설정에서 사용하시면 됩니다.

조명은 아래와 같은 종류로 구분됩니다.

- Binary: On/Off 만 지원되는 일반적인 조명
- Dimmable: 밝기 조절되는 조명 **(지원되지 않음)**

온도조절기는 아래와 같은 항목이 있습니다.

- Maker: 제조사 (번호로 표시되나 알 수 없음)
- Type: 종류 (Air: 기온식, Hot Water: 난방수 온도식)
- Temperature Max: 최고 온도
- Temperature Min: 최저 온도
- Support 0.5: 0.5도 단위 제어 지원 유무
- Support Reserve: 예약 기능 지원
- Support Hot Water: 온수 기능 지원
- Support Outing: 외출 기능 지원

설정이 끝난 이후 삭제하셔도 됩니다.

### 조명

```yaml
  lights:
    - name: "LivingRoom Light1"
      id: livingroom_light1
      sub_id: 0x11
```

sub_id 는 위에서 스캔한 ID 를 기입하시면 됩니다. 0x11, 0x12, 0x13, ... 과 같은 형식입니다.

### 가스밸브

```yaml
  valve_sensor:
    name: "Gas Valve"
    id: gas_valve
  valve_close_button:
    name: "Gas Valve Close"
    id: gas_valve_close
```

### 온도조절기

```yaml
  climates:
    - name: "LivingRoom Climate"
      id: livingroom_climate
      sub_id: 0x11
```

마찬가지로, sub_id 는 위에서 스캔한 ID 를 기입하시면 됩니다. 0x11, 0x12, 0x13, ... 과 같은 형식입니다.

0.5도 단위 조작을 원하시면 아래 설정을 추가하시면 됩니다. (우리집에서 지원되지 않아 검증되지 않음)

```yaml
  support_temperature_0_5: true
```

### 원격검침기

```yaml
  measures:
    water_current:
      name: "Water Current"
      update_interval: 30s
    water_total:
      name: "Water Total"
      update_interval: 30s
    gas_total:
      name: "Gas Total"
      update_interval: 30s
    power_current:
      name: "Power Current"
      update_interval: 30s
    power_total:
      name: "Power Total"
      update_interval: 30s
```

### 일괄차단기(일괄소등)

```yaml
  breaker:
    name: "Break Lights"
    id: break_lights
```

### 기타

실제 전송 데이터를 보고 싶다면, 아래 스위치를 추가하고 켜면 됩니다.

```yaml
  detailed_log:
    name: "Wallpad Log"
    id: wallpad_log
```

간혹 조명이 켜지지 않거나 하는 등 통신 오류가 발생 할 수 있습니다. 그런 경우 재전송하는 기능이 있으며 횟수 설정이 가능합니다(기본값 2회, 최소 1회, 최대 5회). 무작정 횟수를 늘리시는 것보다 장치와 케이블을 점검하시는 것을 권장합니다.

```yaml
  write_retry: 2
```

모든 항목을 추가한 예제는 [여기서](example.yaml) 확인하실 수 있습니다.


## 기타
모션, 도어 센서는 별도의 전압 분배 회로를 이용하시면 됩니다.

간헐적으로 사용 불가(`unavailable`) 상태가 되는 문제가 있습니다.
