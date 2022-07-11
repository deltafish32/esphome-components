#include "wallpad_misc.h"
#include "navien_wallpad.h"


namespace esphome {
namespace navien_wallpad {


float WallpadBreakLightsSwitch::get_setup_priority() const {
  return setup_priority::HARDWARE;
}


void WallpadBreakLightsSwitch::setup() {
}


void WallpadBreakLightsSwitch::dump_config() {
  ESP_LOGCONFIG("WallpadBreakLightsSwitch", "%s:", "WallpadBreakLightsSwitch");
}


void WallpadBreakLightsSwitch::write_state(bool state) {
  // 재부팅시 상태 변경 방지
  if (Navien_Wallpad::get_breaker_init()) {
    Navien_Wallpad::set_break_lights(state);

    // Breaker 로 소등되지 않는 전등들 소등
    if (state) {
      Navien_Wallpad::set_light_state(0xff, false);
    }
  }

  publish_state(state);
}


float WallpadLogSwitch::get_setup_priority() const {
  return setup_priority::HARDWARE;
}


void WallpadLogSwitch::setup() {
}


void WallpadLogSwitch::dump_config() {
  ESP_LOGCONFIG("WallpadLogSwitch", "%s:", "WallpadLogSwitch");
}


void WallpadLogSwitch::write_state(bool state) {
  Navien_Wallpad::set_show_detailed_log(state);
  publish_state(state);
}


void WallpadTestButton::dump_config() {
  ESP_LOGCONFIG("WallpadTestButton", "%s:", "WallpadTestButton");
}


float WallpadTestButton::get_setup_priority() const {
  return setup_priority::HARDWARE;
}


void WallpadTestButton::press_action() {
  //NavienWallpad::writeTest(KSX4506_DATA(KID_DOORLOCK, 0x01, KCT_REQ_STATE, 0, NULL));
}


} // navien_wallpad
} // esphome
