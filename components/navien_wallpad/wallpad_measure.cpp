#include "wallpad_measure.h"
#include "navien_wallpad.h"


namespace esphome {
namespace navien_wallpad {


static const char *TAG = "WallpadMeasure";


WallpadMeasure::WallpadMeasure() {
}


float WallpadMeasure::get_setup_priority() const {
  return esphome::setup_priority::HARDWARE;
}


void WallpadMeasure::setup() {
}


void WallpadMeasure::update() {

}


void WallpadMeasure::dump_config() {
  ESP_LOGCONFIG(TAG, "%s:", class_name_);
  ESP_LOGCONFIG(TAG, "  update_interval: %.1f secs", update_interval_ / 1000.0f);
}




WallpadWaterCurrent::WallpadWaterCurrent() {
  class_name_ = "WallpadWaterCurrent";
}


void WallpadWaterCurrent::update() {
  publish_state(Navien_Wallpad::get_water_current());
}


WallpadWaterTotal::WallpadWaterTotal() {
  class_name_ = "WallpadWaterTotal";
}


void WallpadWaterTotal::update() {
  publish_state(Navien_Wallpad::get_water_total());
}



WallpadGasCurrent::WallpadGasCurrent() {
  class_name_ = "WallpadGasCurrent";
}


void WallpadGasCurrent::update() {
  publish_state(Navien_Wallpad::get_gas_current());
}


WallpadGasTotal::WallpadGasTotal() {
  class_name_ = "WallpadGasTotal";
}


void WallpadGasTotal::update() {
  publish_state(Navien_Wallpad::get_gas_total());
}



WallpadPowerCurrent::WallpadPowerCurrent() {
  class_name_ = "WallpadPowerCurrent";
}


void WallpadPowerCurrent::update() {
  publish_state(Navien_Wallpad::get_power_current());
}


WallpadPowerTotal::WallpadPowerTotal() {
  class_name_ = "WallpadPowerTotal";
}


void WallpadPowerTotal::update() {
  publish_state(Navien_Wallpad::get_power_total());
}


} // namespace navien_wallpad
} // namespace esphome
