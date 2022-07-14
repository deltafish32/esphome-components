#include "navien_wallpad.h"
#include "log.h"


namespace esphome {
namespace navien_wallpad {

static const char *TAG = "Navien_Wallpad";




static const KSX4506_DATA polling_commands[] = {
  //KSX4506_DATA(KID_LIGHT,        0x1f, KCT_REQ_STATE, 0, NULL),
  //KSX4506_DATA(KID_GASVALVE,     0x01, KCT_REQ_STATE, 0, NULL),
  //KSX4506_DATA(KID_MONITOR,      0x0f, KCT_REQ_STATE, 0, NULL),
  KSX4506_DATA(KID_THERMOSTAT,   0x1f, KCT_REQ_STATE, 0, NULL),
};
static const int polling_commands_count = sizeof(polling_commands) / sizeof(polling_commands[0]);

static const byte VALVE_OPEN  = 0x01;
static const byte VALVE_CLOSE = 0x02;




Navien_Wallpad* Navien_Wallpad::__this = NULL;


Navien_Wallpad::Navien_Wallpad(uart::UARTComponent *parent) : uart::UARTDevice(parent) {
  __this = this;

  read_tick_ = millis();

  restart_publish_timer();
}


Navien_Wallpad::Navien_Wallpad(const Navien_Wallpad&) {

}


float Navien_Wallpad::get_setup_priority() const {
  return esphome::setup_priority::HARDWARE;
}


void Navien_Wallpad::setup() {
}


void Navien_Wallpad::loop() {
  loop_read();
  loop_write();
  loop_publish();
}


void Navien_Wallpad::dump_config() {
  ESP_LOGCONFIG(TAG, "%s:", TAG);
}


void Navien_Wallpad::loop_read() {
  while (available()) {
    read_queue_.push(read());

    KSX4506_DATA ksx_data;
    int result = parse_packet(read_queue_, &ksx_data);

    if (result > 0) {
      if (on_recv_ksx4506(ksx_data)) {
        if (show_detailed_log_) {
          trace_data("read", read_queue_, result);
        }
      }
      else {
        trace_data("read", read_queue_, result);
      }

      while (result > 0) {
        read_queue_.pop();
        result--;
      }

    }

    read_tick_ = millis();
    read_flag_ = true;
  }
}


void Navien_Wallpad::loop_write() {
  unsigned long tick = millis();
  
  if (read_flag_ && tick - read_tick_ >= write_delay_) {
    byte buffer[KSX4506_MAX_SIZE];
    byte buffer_len = 0;

    if (write_queue_.size() > 0) {
      KSX4506_DATA& ksx_data = write_queue_.front();
      buffer_len = ksx_data.to_buffer(buffer);
      write_queue_.pop_front();
    }
    else {
      byte light_sub_id = 0;
      bool light_onoff = false;
      
      if (polling_flag_ && polling_commands_count > 0) {
        // 일반적인 Polling 처리
        int step = polling_step_;
        buffer_len = polling_commands[polling_step_].to_buffer(buffer);

        // next
        polling_step_++;
        if (polling_step_ >= polling_commands_count) {
          polling_step_ = 0;
          polling_flag_ = false;
        }

        // Wallpad 에서 자체적으로 패킷이 날아오는 경우
        if (polling_commands[step].device_id == KID_THERMOSTAT
        && recv_thermostat_req_ && millis() - recv_thermostat_req_tick_ < thermostat_req_timeout_) {
          // write 건너뜀
          return;
        }
      }
    }

    if (buffer_len > 0) {
      if (show_detailed_log_) {
        trace_data("write", buffer, buffer_len);
      }

      write_array(buffer, buffer_len);
      write_tick_ = tick;

      read_flag_ = false;
    }
    
  }
}


void Navien_Wallpad::loop_publish() {
  unsigned long tick = millis();
  if (write_queue_.size() <= 0) {
    if (tick - publish_tick_ >= publish_period_) {
      if (tick - publish_tick_ >= publish_period_ * 2) {
        publish_tick_ = tick;
      }
      else {
        publish_tick_ += publish_period_;
      }

      publish_lights();
      publish_valve();
      publish_breaker();
    }

    if (tick - publish_slow_tick_ >= publish_slow_period_) {
      if (tick - publish_slow_tick_ >= publish_slow_period_ * 2) {
        publish_slow_tick_ = tick;
      }
      else {
        publish_slow_tick_ += publish_slow_period_;
      }
      
      publish_thermostat();
    }
  }
  else {
    restart_publish_timer();
  }
}


void Navien_Wallpad::publish_lights() {
  for (auto &l_group : lights_) {
    if (l_group.recv == false) continue;

    for (int i = 0; i < KSX4506_MAX_LIGHTS; i++) {
      byte sub_id = MAKE_SUB_ID(l_group.group_id, i + 1);

      for (auto &l_out : light_outputs_) {
        if (l_out->get_sub_id() == sub_id) {
          bool state;
          l_out->get_light_state()->current_values_as_binary(&state);
          
          if (l_group.init_publish == false || state != l_group.state[i]) {
            auto call = l_out->get_light_state()->make_call();
            call.set_state(l_group.state[i] ? true : false);
            call.perform();
          }

          break;
        }
      }

    }

    l_group.init_publish = true;
  }
}


void Navien_Wallpad::publish_valve() {
  if (valve_close_sensor_ == NULL) return;
  if (valve_.recv == false) return;

  if (valve_.state == VALVE_OPEN || valve_.state == VALVE_CLOSE) {
    bool state = valve_.state != VALVE_CLOSE;

    if (valve_.init_publish == false || valve_close_sensor_->state != state) {
      valve_close_sensor_->publish_state(state);

      valve_.init_publish = true;
    }
  }
}


void Navien_Wallpad::publish_breaker() {
  if (breaker_switch_ == NULL) return;
  if (breaker_.recv == false) return;

  if (breaker_.init_publish == false || breaker_switch_->state != breaker_.break_lights) {
    ESP_LOGD(TAG, "breaker %s", breaker_.break_lights ? "on" : "off");
    breaker_switch_->publish_state(breaker_.break_lights);

    breaker_.init_publish = true;
  }
}


void Navien_Wallpad::publish_thermostat() {
  for (auto &t_group : thermostats_) {
    if (t_group.recv == false) continue;
    
    for (int i = 0; i < KSX4506_MAX_CLIMATES; i++) {
      byte sub_id = MAKE_SUB_ID(t_group.group_id, i + 1);

      for (auto &climate : climates_) {
        if (climate->get_sub_id() == sub_id) {
          auto mode = (t_group.state.heating & (0x01 << i)) ? climate::CLIMATE_MODE_HEAT : climate::CLIMATE_MODE_OFF;
          if (climate->mode                != mode
           || climate->current_temperature != t_group.state.current[i]
           || climate->target_temperature  != t_group.state.target[i]) {
            climate->mode = mode;
            climate->target_temperature  = ( t_group.state.target[i]  & 0x7f);
            climate->current_temperature = ( t_group.state.current[i] & 0x7f);

            if (support_temperature_0_5_) {
              climate->target_temperature  += ((t_group.state.target[i]  & 0x80) ? 0.5f : 0);
              climate->current_temperature += ((t_group.state.current[i] & 0x80) ? 0.5f : 0);
            }

            climate->publish_state();
          }
          break;
        }
      }

    }

    t_group.init_publish = true;
  }
}


void Navien_Wallpad::register_valve_close_sensor(binary_sensor::BinarySensor *sensor) {
  valve_close_sensor_ = sensor;
}


void Navien_Wallpad::register_breaker(switch_::Switch *sw) {
  breaker_switch_ = sw;
}


void Navien_Wallpad::register_climate(WallpadClimate *climate) {
  climates_.push_back(climate);

  // 이미 등록된 그룹인지 확인합니다.
  byte group_id = GET_GROUP(climate->get_sub_id());
  bool found_group = false;
  for (auto t_group : thermostats_) {
    if (t_group.group_id == group_id) {
      found_group = true;
      break;
    }
  }

  // 없다면, 그룹을 등록합니다.
  if (found_group == false) {
    THERMOSTAT_DATA td;
    td.group_id = group_id;
    thermostats_.push_back(td);
  }
}


void Navien_Wallpad::register_light(WallpadLightOutput *light) {
  light_outputs_.push_back(light);

  // 이미 등록된 그룹인지 확인합니다.
  byte group_id = GET_GROUP(light->get_sub_id());
  bool found_group = false;
  for (auto l_group : lights_) {
    if (l_group.group_id == group_id) {
      found_group = true;
      break;
    }
  }

  // 없다면, 그룹을 등록합니다.
  if (found_group == false) {
    LIGHT_DATA ld;
    ld.group_id = group_id;
    lights_.push_back(ld);
  }
}


void Navien_Wallpad::set_support_temperature_0_5(bool b) {
  support_temperature_0_5_ = b;
}


void Navien_Wallpad::restart_publish_timer() {
  publish_tick_ = publish_slow_tick_ = millis();
}


// -1: not found
//  0: 아마도 나올 일 없음
// >0: 찾은 데이터의 끝 위치 (필요한 pop 횟수)
int Navien_Wallpad::parse_packet(const CycleQueue<byte>& queue, KSX4506_DATA* p) {
  if (queue.size() < KSX4506_MIN_SIZE) return -1;

  for (int i = 0; i < queue.size() - KSX4506_MIN_SIZE; i++) {
    // 첫 패킷 찾기
    if (queue[i] != 0xf7) continue;
  
    // 해당 길이만큼 패킷이 들어왔는지 확인
    byte len = queue[i + 4];
    if (i + KSX4506_MIN_SIZE - 1 + len >= queue.size()) continue;

    byte temp[KSX4506_MAX_SIZE];

    queue.to_buffer(temp, i, i + 5 + len + 1);
    byte xor_sum = KSX4506_DATA::xor_sum(temp, 5 + len);
    if (xor_sum != queue[i + 5 + len]) continue;

    queue.to_buffer(temp, i, i + 5 + len + 2);
    byte add_sum = KSX4506_DATA::add_sum(temp, 5 + len + 1);
    if (add_sum != queue[i + 5 + len + 1]) continue;

    p->release();
    p->device_id     = queue[i + 1];
    p->device_sub_id = queue[i + 2];
    p->command_type  = queue[i + 3];
    p->data_len      = queue[i + 4];

    if (len > 0) {
      byte* data = new byte[len];
      
      for (int j = 0; j < len; j++) {
        data[j] = queue[i + 5 + j];
      }
      p->set_data(len, data);

      delete[] data;
    }

    return (i + KSX4506_MIN_SIZE + len);
  }

  return -1;
}


unsigned long Navien_Wallpad::bcd2dec(byte b3, byte b2, byte b1, byte b0) {
  unsigned long dec = 0;
  dec += ( ((b0 & 0xf0) >> 4) * 10 + (b0 & 0x0f) ) * 1;
  dec += ( ((b1 & 0xf0) >> 4) * 10 + (b1 & 0x0f) ) * 100;
  dec += ( ((b2 & 0xf0) >> 4) * 10 + (b2 & 0x0f) ) * 10000;
  dec += ( ((b3 & 0xf0) >> 4) * 10 + (b3 & 0x0f) ) * 1000000;
  return dec;
}


// true : 응답이 처리된 경우
// false: 응답이 처리되지 않은 경우
// 현재는 log 작성 유무 판단에만 사용됩니다.
bool Navien_Wallpad::on_recv_ksx4506(const KSX4506_DATA& ksx_data) {
  switch (ksx_data.device_id) {
  case KID_LIGHT: // 조명
    if (ksx_data.command_type == KCT_REQ_STATE) {
      return true;
    }
    else if (ksx_data.command_type == KCT_RPY_STATE
          || ksx_data.command_type == KCT_RPY_CONTROL) {
      bool any_stored = false;

      byte group_id = GET_GROUP(ksx_data.device_sub_id);
      byte each_id  = GET_EACH(ksx_data.device_sub_id);

      if (ksx_data.data[0] == 0x00) { // error check
        if (group_id != KSX4506_GROUP_ALL) {
          if (each_id == KSX4506_ID_ALL) {
            // group_id 가 전체가 아니면서,
            // each_id  가 전체인 경우
            // (0x1f, 0x2f, 0x3f, ...)
            for (int i = 1; i < ksx_data.data_len; i++) {
              for (auto &l_group : lights_) {
                if (l_group.group_id == group_id) {
                  l_group.state[i - 1] = ((ksx_data.data[i] & 0x01) != 0) ? 0x01 : 0x00;
                  l_group.recv = true;
                  break;
                }
              }

              any_stored = true;
            }
          }
          // 짧은 시간에 여러 번 조작하는 경우, 개별 결과 응답이 여러 번 발생합니다.
          // 그로 인해 전체 결과 응답이 publish time 보다 늦게 오고, 오작동이 발생합니다.
          else if (each_id >= 1 && each_id <= KSX4506_MAX_LIGHTS) {
            // 개별 결과
            if (ksx_data.data_len >= 2) {
              // for (auto &l_group : lights_) {
              //   if (l_group.group_id == group_id) {
              //     l_group.state[each_id - 1] = ((ksx_data.data[1] & 0x01) != 0) ? 0x01 : 0x00;
              //     l_group.recv = true;
              //     break;
              //   }
              // }

              any_stored = true;
            }
          }
        }
      }

      return any_stored;
    }
    else if (ksx_data.command_type == KCT_RPY_SPEC && ksx_data.data_len == 5) {
      byte group_id = GET_GROUP(ksx_data.device_sub_id);
      byte each_id  = GET_EACH(ksx_data.device_sub_id);

      int lights_binary = ksx_data.data[1];
      int lights_dimmable = ksx_data.data[2];
      word light_flag = ksx_data.data[3] | (ksx_data.data[4] << 8);
      int lights_total = lights_binary + lights_dimmable;

      if (lights_binary <= KSX4506_MAX_LIGHTS
       && lights_dimmable <= KSX4506_MAX_LIGHTS) {
        ESP_LOGI(TAG, "Light Scan Result: Group: %d, Total: %d (Binary: %d, Dimmable: %d)",
          group_id, lights_total, lights_binary, lights_dimmable);

        ESP_LOGI(TAG, "  List:");
        for (int i = 0; i < lights_total; i++) {
          ESP_LOGI(TAG, "    0x%x%x: %s", group_id, i + 1, (light_flag & (1 << i)) ? "Dimmable" : "Binary");
        }
      }

      return true;
    }
    break;
  case KID_GASVALVE: // 가스 밸브
    switch (ksx_data.device_sub_id) {
    case 0x01:
      if (ksx_data.command_type == KCT_REQ_STATE) {
        return true;
      }
      else if (ksx_data.command_type == KCT_RPY_STATE
            || ksx_data.command_type == KCT_RPY_CONTROL) {
        if (ksx_data.data_len == 2) {
          valve_.state = ksx_data.data[1];

          valve_.recv = true;
          return true;
        }
      }
      break;
    }
  case KID_MONITOR: // 원격 검침기
    switch (ksx_data.device_sub_id) {
    case 0x0f:
      if (ksx_data.command_type == KCT_REQ_STATE) {
        return true;
      }
      else if (ksx_data.command_type == KCT_RPY_STATE && ksx_data.data_len == 35) {
        // * 가스-현재 미사용 추정.
        measure_.water_current = bcd2dec(                0, ksx_data.data[ 0], ksx_data.data[ 1], ksx_data.data[ 2]);
        measure_.water_total   = bcd2dec(ksx_data.data[ 3], ksx_data.data[ 4], ksx_data.data[ 5], ksx_data.data[ 6]);
        measure_.gas_current   = bcd2dec(                0, ksx_data.data[ 7], ksx_data.data[ 8], ksx_data.data[ 9]);
        measure_.gas_total     = bcd2dec(ksx_data.data[10], ksx_data.data[11], ksx_data.data[12], ksx_data.data[13]);
        measure_.power_current = bcd2dec(                0, ksx_data.data[14], ksx_data.data[15], ksx_data.data[16]);
        measure_.power_total   = bcd2dec(ksx_data.data[17], ksx_data.data[18], ksx_data.data[19], ksx_data.data[20]);

        measure_.recv = true;
        return true;
      }
      break;
    }
    break;
  case KID_BREAKER: // 일괄 차단기
    switch (ksx_data.device_sub_id) {
    case 0x01:
      if (ksx_data.command_type == KCT_REQ_STATE) {
        return true;
      }
      else if (ksx_data.command_type == KCT_RPY_STATE
            || ksx_data.command_type == KCT_RPY_CONTROL) {
        if (ksx_data.data_len == 3) {
          // ksx_data.data[0] : error bit
          // ksx_data.data[1] : 5: EV 상승, 4: EV 하강, 3: 대기전력 차단 릴레이
          //               2: 일괄전등 차단 릴레이, 1: 외출 설정, 0: 가스 잠금
          // ksx_data.data[2] : reserved
          //
          // Polling 요청은 이 응답을 받은 직후 합니다.

          breaker_.break_lights = (ksx_data.data[1] & 0x04) ? false : true;
          breaker_.recv = true;
          
          polling_flag_ = true;
          return true;
        }
      }
      break;
    }
    break;
  case KID_THERMOSTAT: // 온도조절기
    if (ksx_data.command_type == KCT_REQ_STATE) {
      // Wallpad 에서 자체적으로 패킷이 날아오는 경우
      recv_thermostat_req_ = true;
      recv_thermostat_req_tick_ = millis();
      return true;
    }
    else if (ksx_data.command_type == KCT_RPY_STATE && ksx_data.data_len >= 5) {
      // ksx_data.data[0] : 에러 상태 코드 (0 정상)
      // ksx_data.data[1] : 난방 상태 bit
      // ksx_data.data[2] : 외출 상태 bit
      // ksx_data.data[3] : 예약 상태 bit
      // ksx_data.data[4] : 온수 전용 상태 0x00, 0x01
      // ksx_data.data[5] : 온도 조절기 n의 설정 온도
      // ksx_data.data[6] : 온도 조절기 n의 현재 온도

      bool any_stored = false;

      byte group_id = GET_GROUP(ksx_data.device_sub_id);
      byte each_id  = GET_EACH(ksx_data.device_sub_id);

      if (group_id != KSX4506_GROUP_ALL) {
        if (each_id == KSX4506_ID_ALL) {
          // group_id 가 전체가 아니면서,
          // each_id  가 전체인 경우
          // (0x1f, 0x2f, 0x3f, ...)

          for (auto &t_group : thermostats_) {
            if (t_group.group_id == group_id) {
              t_group.state.error       = ksx_data.data[0];
              t_group.state.heating     = ksx_data.data[1];
              t_group.state.outing      = ksx_data.data[2];
              t_group.state.reservation = ksx_data.data[3];
              t_group.state.hotwater    = ksx_data.data[4];
              
              byte thermostat_count = (ksx_data.data_len - 5) / 2;
              for (int i = 0; i < thermostat_count; i++) {
                t_group.state.target[i]  = ksx_data.data[5 + (i * 2)];
                t_group.state.current[i] = ksx_data.data[6 + (i * 2)];
              }

              t_group.recv = true;
              any_stored = true;
              break;
            }
          }

        }
      }

      return any_stored;
    }
    else if (ksx_data.command_type == KCT_RPY_SPEC && ksx_data.data_len == 7) {
      byte group_id = GET_GROUP(ksx_data.device_sub_id);
      byte each_id  = GET_EACH(ksx_data.device_sub_id);

      int thermostats_maker      = ksx_data.data[1]; // 제조사
      int thermostats_type       = ksx_data.data[2];  // 방식 0x01: 기온, 0x02: 난방수
      int thermostats_max        = ksx_data.data[3];   // 온도 상한
      int thermostats_min        = ksx_data.data[4];   // 온도 하한
      bool thermostats_0_5       = (ksx_data.data[5] & 0x10) ? true : false;
      bool thermostats_reserve   = (ksx_data.data[5] & 0x08) ? true : false;
      bool thermostats_hot_water = (ksx_data.data[5] & 0x04) ? true : false;
      bool thermostats_outing    = (ksx_data.data[5] & 0x02) ? true : false;
      int thermostats_count      = ksx_data.data[6]; // 컨트롤러 개수

      if (thermostats_count <= KSX4506_MAX_CLIMATES) {
        ESP_LOGI(TAG, "Thermostat Scan Result: Group: %d, Total: %d", group_id, thermostats_count);
        ESP_LOGI(TAG, "  Maker: 0x%02x", thermostats_maker);
        ESP_LOGI(TAG, "  Type: %s (0x%02x)", 
          thermostats_type == 0x01 ? "Air" : 
          thermostats_type == 0x02 ? "Heating Water" : "Unknown",
          thermostats_type);
        ESP_LOGI(TAG, "  Temperature Max: %d", thermostats_max);
        ESP_LOGI(TAG, "  Temperature Min: %d", thermostats_min);
        ESP_LOGI(TAG, "  Support 0.5: %s", thermostats_0_5 ? "yes" : "no");
        ESP_LOGI(TAG, "  Support Reserve: %s", thermostats_reserve ? "yes" : "no");
        ESP_LOGI(TAG, "  Support Hot Water: %s", thermostats_hot_water ? "yes" : "no");
        ESP_LOGI(TAG, "  Support Outing: %s", thermostats_outing ? "yes" : "no");
        ESP_LOGI(TAG, "  List:");
        for (int i = 0; i < thermostats_count; i++) {
          ESP_LOGI(TAG, "    0x%x%x", group_id, i + 1);
        }
      }

      return true;
    }
    break;
  }

  return false;
}


template <class _Ty> void Navien_Wallpad::trace_data(const char* tag, const _Ty& data, byte len) {
  char data_str[1024];
  const int div = 150;
  for (int i = 0; i < (len / div + (len % div > 0 ? 1 : 0)); i++) {
    int count = (i >= (len / div)) ? len % div : div;
    for (int j = 0; j < count; j++) {
      sprintf(data_str + (j * 3), "%02x ", data[(i * div) + j]);
    }
    ESP_LOGD(tag, "%s%s", (i == 0) ? "data: " : "", data_str);
  }
}


// bool Navien_Wallpad::light_store_state(byte sub_id, bool state) {
//   if (!is_valid_sub_id_light(sub_id)) return false;

//   byte group_id = GET_GROUP(sub_id);
//   byte each_id  = GET_EACH(sub_id);

//   for (auto &l_group : lights_) {
//     if (l_group.group_id == group_id) {
//       l_group.state[each_id - 1] = state ? 0x01 : 0x00;
//       return true;
//     }
//   }

//   return false;
// }


bool Navien_Wallpad::is_valid_sub_id_light(byte sub_id) {
  byte group_id = GET_GROUP(sub_id);
  byte each_id  = GET_EACH(sub_id);

  if (!(group_id >= 1 && group_id <= KSX4506_MAX_GROUPS)) return false;

  if (!(each_id >= 1 && each_id <= KSX4506_MAX_LIGHTS)) return false;

  return true;
}


bool Navien_Wallpad::is_valid_sub_id_climate(byte sub_id) {
  byte group_id = GET_GROUP(sub_id);
  byte each_id  = GET_EACH(sub_id);

  if (!(group_id >= 1 && group_id <= KSX4506_MAX_GROUPS)) return false;

  if (!(each_id >= 1 && each_id <= KSX4506_MAX_CLIMATES)) return false;

  return true;
}


bool Navien_Wallpad::get_breaker_init() {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return false;
  }

  return __this->breaker_.init_publish;
}


bool Navien_Wallpad::get_climate_init(byte sub_id) {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return false;
  }

  byte group_id = GET_GROUP(sub_id);

  for (auto &t_group : __this->thermostats_) {
    if (t_group.group_id == group_id) {
      return t_group.init_publish;
    }
  }

  return false;
}


bool Navien_Wallpad::get_light_init(byte sub_id) {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return false;
  }

  byte group_id = GET_GROUP(sub_id);

  for (auto &l_group : __this->lights_) {
    if (l_group.group_id == group_id) {
      return l_group.init_publish;
    }
  }

  return false;
}


void Navien_Wallpad::set_light_state(byte sub_id, bool onoff) {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return;
  }

  ESP_LOGD(TAG, "%s(0x%02x, %s)", __func__, sub_id, BOOL_STR(onoff));

  byte group_id = GET_GROUP(sub_id);
  byte each_id  = GET_EACH(sub_id);
  byte index = (each_id - 1);

  if (!((group_id >= 1 && group_id <= KSX4506_MAX_GROUPS) || group_id == KSX4506_GROUP_ALL)) {
    return;
  }
  
  if (each_id == KSX4506_ID_ALL) {
    byte data[1] = { (byte)(onoff ? 1 : 0) };
    // __this->write_queue_.push_back(KSX4506_DATA(KID_LIGHT, MAKE_SUB_ID(group_id, KSX4506_ID_ALL), KCT_REQ_CONTROLALL, 1, data));
    __this->write_queue_.emplace_back(KID_LIGHT, MAKE_SUB_ID(group_id, KSX4506_ID_ALL), KCT_REQ_CONTROLALL, 1, data);
  }
  else {
    if (!(each_id >= 1 && each_id <= KSX4506_MAX_LIGHTS)) {
      return;
    }
    
    byte data[1] = { (byte)(onoff ? 1 : 0) };
    // __this->write_queue_.push_back(KSX4506_DATA(KID_LIGHT, sub_id, KCT_REQ_CONTROL, 1, data));
    __this->write_queue_.emplace_back(KID_LIGHT, sub_id, KCT_REQ_CONTROL, 1, data);
  }

  for (auto &l_group : __this->lights_) {
    if (l_group.group_id == group_id) {
      l_group.recv = false;
      break;
    }
  }

  __this->restart_publish_timer();
}


void Navien_Wallpad::set_valve_close() {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return;
  }

  LOG_SET_NULL();

  byte data[1] = {0};
  // __this->write_queue_.push_back(KSX4506_DATA(KID_GASVALVE, 0x01, KCT_REQ_CONTROL, 1, data));
  __this->write_queue_.emplace_back(KID_GASVALVE, 0x01, KCT_REQ_CONTROL, 1, data);
  __this->valve_.recv = false;

  __this->restart_publish_timer();
}


void Navien_Wallpad::set_break_lights(bool onoff) {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return;
  }

  LOG_SET_BOOL(onoff);

  __this->breaker_.break_lights = onoff;
  byte data[1] = { (byte)(onoff ? 0x00 : 0x01) };
  // __this->write_queue_.push_back(KSX4506_DATA(KID_BREAKER, 0x01, KCT_REQ_CONTROL, 1, data));
  __this->write_queue_.emplace_back(KID_BREAKER, 0x01, KCT_REQ_CONTROL, 1, data);
  __this->breaker_.recv = false;

  __this->restart_publish_timer();
}


void Navien_Wallpad::set_show_detailed_log(bool onoff) {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return;
  }

  LOG_SET_BOOL(onoff);

  __this->show_detailed_log_ = onoff;
}


void Navien_Wallpad::set_thermostat_onoff(byte sub_id, bool onoff) {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return;
  }

  ESP_LOGD(TAG, "%s(0x%02x, %s)", __func__, sub_id, BOOL_STR(onoff));

  if (!is_valid_sub_id_climate(sub_id)) return;

  byte group_id = GET_GROUP(sub_id);
  byte each_id  = GET_EACH(sub_id);
  byte index = (each_id - 1);

  for (auto &t_group : __this->thermostats_) {
    if (t_group.group_id == group_id) {
      if (onoff) {
        t_group.state.heating |= (0x01 << index);
        t_group.state.outing &= ~(0x01 << index);

        byte data[1] = {1};
        // __this->write_queue_.push_back(KSX4506_DATA(KID_THERMOSTAT, sub_id, KCT_REQ_HEATING, 1, data));
        __this->write_queue_.emplace_back(KID_THERMOSTAT, sub_id, KCT_REQ_HEATING, 1, data);
        t_group.recv = false;
      }
      else {
        t_group.state.heating &= ~(0x01 << index);
        t_group.state.outing |= (0x01 << index);

        byte data[1] = {1};
        // __this->write_queue_.push_back(KSX4506_DATA(KID_THERMOSTAT, sub_id, KCT_REQ_OUTING, 1, data));
        __this->write_queue_.emplace_back(KID_THERMOSTAT, sub_id, KCT_REQ_OUTING, 1, data);
        t_group.recv = false;
      }

      break;
    }
  }

  __this->restart_publish_timer();
}


void Navien_Wallpad::set_thermostat_temperature(byte sub_id, float temperature) {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return;
  }

  ESP_LOGD(TAG, "%s(0x%02x, %.1f)", __func__, sub_id, temperature);

  if (!is_valid_sub_id_climate(sub_id)) return;

  byte group_id = GET_GROUP(sub_id);
  byte each_id  = GET_EACH(sub_id);
  byte index = (each_id - 1);

  for (auto &t_group : __this->thermostats_) {
    if (t_group.group_id == group_id) {
      byte data[1] = { (byte)temperature };
      
      if (__this->support_temperature_0_5_) {
        data[0] |= ((int)(temperature * 2) % 2 > 0) ? 0x80 : 0x00;
      }

      t_group.state.target[index] = data[0];
      
      // __this->write_queue_.push_back(KSX4506_DATA(KID_THERMOSTAT, sub_id, KCT_REQ_TEMPERATURE, 1, data));
      __this->write_queue_.emplace_back(KID_THERMOSTAT, sub_id, KCT_REQ_TEMPERATURE, 1, data);
      t_group.recv = false;
    }
  }

  __this->restart_publish_timer();
}


double Navien_Wallpad::get_water_current() {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return NAN;
  }

  if (!__this->measure_.recv) return NAN;

  return __this->measure_.water_current / 1000.0;
}


double Navien_Wallpad::get_water_total() {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return NAN;
  }

  if (!__this->measure_.recv) return NAN;

  return __this->measure_.water_total/ 100.0;
}


double Navien_Wallpad::get_gas_current() {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return NAN;
  }

  if (!__this->measure_.recv) return NAN;

  return __this->measure_.gas_current / 1000.0;
}


double Navien_Wallpad::get_gas_total() {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return NAN;
  }

  if (!__this->measure_.recv) return NAN;

  return __this->measure_.gas_total / 100.0;
}


double Navien_Wallpad::get_power_current() {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return NAN;
  }

  if (!__this->measure_.recv) return NAN;

  return __this->measure_.power_current;
}


double Navien_Wallpad::get_power_total() {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return NAN;
  }

  if (!__this->measure_.recv) return NAN;

  return __this->measure_.power_total / 10.0;
}


void Navien_Wallpad::push_queue(const KSX4506_DATA& ksx_data) {
  if (__this == NULL) {
    LOG_NOT_INITIALIZED();
    return;
  }

  __this->write_queue_.push_back(ksx_data);
}


} // namespace navien_wallpad
} // namespace esphome
