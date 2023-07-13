#pragma once

#include "esphome/core/component.h"
#include <ESPAsyncE131.h>

namespace esphome {
namespace sacn_component {

class sACNComponent : public Component {
  public:
    void setup() override;
    void loop() override;
    float get_setup_priority() const override;
    void dump_config() override;

    void set_universe(uint16_t universe) { universe_ = universe; };
    void set_start_address(uint8_t start_address) { start_address_ = start_address; update_max_channel(); }
    void set_rgb_light(light::AddressableLightState *rgb_light) { rgb_light_ = rgb_light; }
    void set_rgb_num_leds(uint8_t rgb_num_leds) {
      rgb_num_leds_ = rgb_num_leds;
      update_max_channel();
      current_led_state.clear();
      current_led_state.reserve(rgb_num_leds);
    }
    void set_motor(light::LightState *motor_light) { motor_light_ = motor_light; }

    ESPAsyncE131 e131;

  protected:
    uint16_t universe_;
    uint8_t start_address_;
    light::AddressableLightState* rgb_light_;
    uint8_t rgb_num_leds_;
    light::LightState* motor_light_;

    uint16_t max_channel_;
    uint16_t motor_channel_;
    std::vector<Color> current_led_state;
    uint8_t current_motor_state;
    bool firstBoot = true;

    network::IPAddress ip;
    std::string mac;

    light::AddressableLight* getRGBLight() { return (light::AddressableLight*) rgb_light_->get_output(); }
    light::LightState* getMotor() { return motor_light_; }

  private:
    // RGB x num LEDs + motor channel
    void update_max_channel() {
      max_channel_ = start_address_ + (rgb_num_leds_ * 3);
      motor_channel_ = max_channel_;
    }
};

}  // namespace artnet_component
}  // namespace esphome
