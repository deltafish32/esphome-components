#include "wallpad_climate.h"
#include "navien_wallpad.h"


namespace esphome {
namespace navien_wallpad {


static const char *TAG = "WallpadClimate";



WallpadClimate::WallpadClimate() {

}


float WallpadClimate::get_setup_priority() const {
  return esphome::setup_priority::HARDWARE;
}


void WallpadClimate::setup() {

}


void WallpadClimate::dump_config() {
  ESP_LOGCONFIG(TAG, "%s:", TAG);
  ESP_LOGCONFIG(TAG, "  sub_id: %d", sub_id_);
}


climate::ClimateTraits WallpadClimate::traits() {
  // The capabilities of the climate device
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supports_two_point_target_temperature(false);
  traits.set_supported_modes({
    climate::CLIMATE_MODE_OFF,
    climate::CLIMATE_MODE_HEAT,
  });
  traits.set_visual_temperature_step(1.0f);
  return traits;
}


void WallpadClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    // User requested mode change
    climate::ClimateMode callmode = *call.get_mode();
    // Send mode to hardware
    // ...

    if (Navien_Wallpad::get_climate_init(sub_id_)) {
      Navien_Wallpad::set_thermostat_onoff(sub_id_, callmode != climate::CLIMATE_MODE_OFF);
    }

    // Publish updated state
    mode = callmode;
    publish_state();
  }
  
  if (call.get_target_temperature().has_value()) {
    // User requested target temperature change
    float temp = *call.get_target_temperature();
    // Send target temp to climate
    // ...

    if (Navien_Wallpad::get_climate_init(sub_id_)) {
      Navien_Wallpad::set_thermostat_temperature(sub_id_, temp);
    }
  }
}


void WallpadClimate::set_sub_id(int sub_id) {
  sub_id_ = sub_id;
}


int WallpadClimate::get_sub_id() const {
  return sub_id_;
}


} // namespace navien_wallpad
} // namespace esphome
