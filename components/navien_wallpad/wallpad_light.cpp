#include "wallpad_light.h"
#include "navien_wallpad.h"


namespace esphome {
namespace navien_wallpad {


static const char *TAG = "WallpadLightOutput";


WallpadLightOutput::WallpadLightOutput() {

}


float WallpadLightOutput::get_setup_priority() const {
  return esphome::setup_priority::HARDWARE;
}


void WallpadLightOutput::setup() {
}


void WallpadLightOutput::dump_config() {
  ESP_LOGCONFIG(TAG, "%s:", TAG);
  ESP_LOGCONFIG(TAG, "  sub_id: %d", sub_id_);
}


light::LightTraits WallpadLightOutput::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supported_color_modes({light::ColorMode::ON_OFF});
  return traits;
}


void WallpadLightOutput::setup_state(light::LightState *state) {
  this->light_state_ = state;
}


void WallpadLightOutput::write_state(light::LightState *state) {
  bool onoff;
  state->current_values_as_binary(&onoff);

  // 재부팅시 상태 변경 방지
  if (Navien_Wallpad::get_light_init(sub_id_)) {
    Navien_Wallpad::set_light_state(sub_id_, onoff);
  }
}


light::LightState *WallpadLightOutput::get_light_state() {
  return light_state_;
}


void WallpadLightOutput::set_sub_id(int sub_id) {
  sub_id_ = sub_id;
}


int WallpadLightOutput::get_sub_id() const {
  return sub_id_;
}


} // namespace navien_wallpad
} // namespace esphome
