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
  traits.add_feature_flags(climate::CLIMATE_SUPPORTS_CURRENT_TEMPERATURE);
  traits.set_supported_modes({
    climate::CLIMATE_MODE_OFF,
    climate::CLIMATE_MODE_HEAT,
  });
  traits.set_visual_temperature_step(Navien_Wallpad::get_support_temperature_0_5() ? 0.5f : 1.0f);
  return traits;
}


void WallpadClimate::control(const climate::ClimateCall &call) {
  // 재부팅 직후 등 Wallpad 상태를 아직 받지 못한 경우, 명령을 보내지 않고
  // 현재 상태를 다시 publish 해서 UI 를 원래 값으로 되돌립니다.
  if (!Navien_Wallpad::get_climate_init(sub_id_)) {
    ESP_LOGW(TAG, "0x%02x: not initialized yet, control ignored", sub_id_);
    publish_state();
    return;
  }

  if (call.get_mode().has_value()) {
    climate::ClimateMode callmode = *call.get_mode();
    Navien_Wallpad::set_thermostat_onoff(sub_id_, callmode != climate::CLIMATE_MODE_OFF);
    mode = callmode;
  }

  if (call.get_target_temperature().has_value()) {
    float temp = *call.get_target_temperature();
    Navien_Wallpad::set_thermostat_temperature(sub_id_, temp);
    target_temperature = temp;
  }

  // 낙관적으로 publish 하고, 이후 Wallpad 응답으로 갱신합니다.
  publish_state();
}


void WallpadClimate::set_sub_id(int sub_id) {
  sub_id_ = sub_id;
}


int WallpadClimate::get_sub_id() const {
  return sub_id_;
}


} // namespace navien_wallpad
} // namespace esphome
