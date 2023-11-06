# 보너스: 자동화 편의를 위한 이벤트 설정
## 개요
Roode는 Presence 기능이 있어 카운터 센서 앞에 서있다가 출입을 취소하는 경우도 감지할 수 있습니다. 그러나 이 기능으로 인해 이를 Home Assistant 자동화에 접목시키는 난이도 상승 요인으로 작용합니다.

대부분의 경우 1인 이상 출입시 전등을 켜고, 한 명도 없는 경우 전등을 끄는 자동화로 많이 사용하는데, 이 자동화를 간편하게 할 수 있는 이벤트 설정 방법입니다.


## 이벤트
참고로 카운터를 Home Assistant에서 수동으로 변경하는 경우에도 이벤트가 발행됩니다.

카운터 센서 앞에 사람이 감지되는 경우 Presnece On, 감지되지 않는 경우 Presence Off 입니다.

| 이벤트 | 설명 | 비고 |
| - | - | - |
| esphome.roode_first_entry | 첫 1명 출입시 | |
| esphome.roode_last_exit | 마지막 1명 출문시 | |

추가로 특수한 자동화 구성시 사용할 수 있는 이벤트입니다.

| 이벤트 | 설명 | 비고 |
| - | - | - |
| esphome.roode_entry | 2번째 이후 출입시 | |
| esphome.roode_exit | 마지막이 아닌 사람 출문시 | |
| esphome.roode_presence | 2번째 이후 Presence On 시 | |
| esphome.roode_return | 카운터 센서 앞에 섰다가 출입/출문을 취소한 경우 | |


## ESPHome 자동화 설정
아래와 같이 Counter와 Presence에 id를 지정해야 합니다.

Presence binary_sensor에는 아래와 같이 delayed_on을 주는 것을 권장드립니다. 햇빛 등 주변광에 의해 오작동하는 것을 막을 수 있습니다.

```yaml
binary_sensor:
  - platform: roode
    presence_sensor:
      name: Counter Bathroom Presence
      id: presence
      filters:
        - delayed_on: 100ms

number:
  - platform: roode
    people_counter:
      name: Counter Bathroom
      id: counter
      max_value: 10
```

이후, 제어에 필요한 변수와 로컬 자동화입니다.

```yaml
globals:
  - id: counter_prev
    type: int
    restore_value: false
    initial_value: "-1"
  - id: presence_prev
    type: bool
    restore_value: false
    initial_value: "false"
  - id: first_entry
    type: bool
    restore_value: false
    initial_value: "false"

interval:
  - interval: 0ms
    then:
      - if: # init
          condition:
            lambda: return id(counter_prev) < 0;
          then:
            - lambda: id(counter_prev) = id(counter).state;
            - lambda: id(presence_prev) = id(presence).state;
            - lambda: id(first_entry) = (id(counter).state > 0);
      - if: # presence on
          condition:
            lambda: return (id(presence_prev) != id(presence).state && id(presence).state);
          then:
            - if:
                condition:
                  lambda: return id(counter).state <= 0;
                then:
                  - homeassistant.event:
                      event: esphome.roode_first_entry
                      data:
                        entity_id: $device_name
                else:
                  - homeassistant.event:
                      event: esphome.roode_presence
                      data:
                        entity_id: $device_name
            - lambda: id(presence_prev) = id(presence).state;
            - lambda: id(first_entry) = true;
      - if: # presence off, or counter modified
          condition:
            or:
              - lambda: return (id(presence_prev) != id(presence).state && id(presence).state == false);
              - lambda: return (id(counter_prev) != id(counter).state);
          then:
            - if: # counter modified first entry
                condition:
                  lambda: return (id(first_entry) == false && id(counter_prev) <= 0 && id(counter).state > 0);
                then:
                  - homeassistant.event:
                      event: esphome.roode_first_entry
                      data:
                        entity_id: $device_name
                  - lambda: id(first_entry) = true;
            - if: # entry
                condition:
                  lambda: return (id(counter_prev) < id(counter).state);
                then:
                  - homeassistant.event:
                      event: esphome.roode_entry
                      data:
                        entity_id: $device_name
            - if: # exit
                condition:
                  lambda: return (id(counter_prev) > id(counter).state);
                then:
                  - homeassistant.event:
                      event: esphome.roode_exit
                      data:
                        entity_id: $device_name
            - if: # return
                condition:
                  lambda: return (id(counter_prev) == id(counter).state);
                then:
                  - homeassistant.event:
                      event: esphome.roode_return
                      data:
                        entity_id: $device_name
            - if: # last exit
                condition:
                  lambda: return (id(counter).state <= 0);
                then:
                  - homeassistant.event:
                      event: esphome.roode_last_exit
                      data:
                        entity_id: $device_name
                  - lambda: id(first_entry) = false;
            # wrapup
            - lambda: id(counter_prev) = id(counter).state;
            - lambda: id(presence_prev) = id(presence).state;
```

이제, Home Assistant 자동화는 간편하게 설정할 수 있습니다.


## Home Assistant 자동화 설정

```yaml
alias: Bathroom Light Counter
description: ""
trigger:
  - platform: event
    event_type: esphome.roode_first_entry
    event_data:
      entity_id: Counter Bathroom
    id: first_entry
  - platform: event
    event_type: esphome.roode_last_exit
    event_data:
      entity_id: Counter Bathroom
    id: last_exit
condition: []
action:
  - alias: Light On
    if:
      - condition: trigger
        id: first_entry
    then:
      - service: light.turn_on
        data: {}
        target:
          entity_id:
            - light.bathroom
  - alias: Light Off
    if:
      - condition: trigger
        id: last_exit
    then:
      - service: light.turn_off
        data: {}
        target:
          entity_id:
            - light.bathroom
mode: queued
max: 10
```
