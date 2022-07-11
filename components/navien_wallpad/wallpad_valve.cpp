#include "wallpad_valve.h"
#include "navien_wallpad.h"


namespace esphome {
namespace navien_wallpad {


static const char *TAG = "WallpadValveCloseButton";


void WallpadValveCloseButton::dump_config() {
  ESP_LOGCONFIG(TAG, "%s:", TAG);
}


float WallpadValveCloseButton::get_setup_priority() const {
  return setup_priority::HARDWARE;
}


void WallpadValveCloseButton::press_action() {
  Navien_Wallpad::set_valve_close();
}


} // navien_wallpad
} // esphome
