#include "esphome.h"
#include <ESPAsyncE131.h>
#include "sacn_component.h"

namespace esphome {
namespace sacn_component {

static const char *TAG = "sACN";
float sACNComponent::get_setup_priority() const { return setup_priority::LATE; }

void sACNComponent::setup() {
    ESP_LOGI(TAG, "Setting up sACN component...");

    e131 = ESPAsyncE131(universe_); 

    // Choose one to begin listening for E1.31 data
    //if (e131.begin(E131_UNICAST))                               // Listen via Unicast
    if (e131.begin(E131_MULTICAST, universe_, 1))   // Listen via Multicast
        ESP_LOGI(TAG, "Listening for data...");
    else 
        ESP_LOGE(TAG, "*** e131.begin failed ***");
}

void sACNComponent::loop() {
    if (!e131.isEmpty()) {
        e131_packet_t packet;
        e131.pull(&packet);     // Pull packet from ring buffer
        
        uint16_t universe = htons(packet.universe);
        uint16_t property_value_count = htons(packet.property_value_count) - 1;
        // TODO: Some assertions on property value count etc
        if( universe == universe_ ) {
            //ESP_LOGD(TAG, "Start: %u, Num: %u, Max: %u", start_address_, rgb_num_leds_, start_address_ + (rgb_num_leds_ * 3));
            uint16_t channel, index;
            for( channel = start_address_, index = 0; channel < max_channel_; index++, channel++ ) {
                // set RGB and current state variables if there is a change
                bool dirty = firstBoot;
                if( packet.property_values[channel] != current_led_state[index].red ) {
                    current_led_state[index].red = packet.property_values[channel];
                    dirty = true;
                }
                channel++;
                if( packet.property_values[channel] != current_led_state[index].green ) {
                    current_led_state[index].green = packet.property_values[channel];
                    dirty = true;
                }
                channel++;
                if( packet.property_values[channel] != current_led_state[index].blue ) {
                    current_led_state[index].blue = packet.property_values[channel];
                    dirty = true;
                }
                if( dirty ) {
                    (*getRGBLight())[index].set(current_led_state[index]);
                    ESP_LOGD(TAG, "Changing LED index: %d based on %d to (%d, %d, %d)",
                        index, channel - 3, current_led_state[index].red, current_led_state[index].green, current_led_state[index].blue
                    );
                }
            }
            getRGBLight()->schedule_show();
            if( firstBoot || packet.property_values[motor_channel_] != current_motor_state ) {
                current_motor_state = packet.property_values[motor_channel_];
                light::LightCall call = getMotor()->make_call();
                if( current_motor_state == 0 )
                    call = getMotor()->turn_off();
                else {
                    call = getMotor()->turn_on();
                    float newMotor = current_motor_state / 255.0;
                    ESP_LOGD(TAG, "Changing motor to: %f based on %d", newMotor, packet.property_values[motor_channel_]);
                    call.set_brightness(newMotor);
                }
                call.perform();
            }
            firstBoot = false;
        }
    }

        // ESP_LOGD(TAG, "Universe %u / %u Channels | Packet#: %u / Errors: %u / RGB: %u,%u,%u\n",
        //         htons(packet.universe),                 // The Universe for this packet
        //         htons(packet.property_value_count) - 1, // Start code is ignored, we're interested in dimmer data
        //         e131.stats.num_packets,                 // Packet counter
        //         e131.stats.packet_errors,               // Packet error counter
        //         packet.property_values[start_address_],       // Dimmer data for Channel 1
        //         packet.property_values[start_address_+1],         // Dimmer data for Channel 2
        //         packet.property_values[start_address_+2]);       // Dimmer data for Channel 3
}

void sACNComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "sACN component");
    ESP_LOGCONFIG(TAG, "  Universe: %d", universe_);
    ESP_LOGCONFIG(TAG, "  Start address: %d", start_address_);
    ESP_LOGCONFIG(TAG, "  Number of LEDs: %d", rgb_num_leds_);
}

}  // namespace sacn_component
}  // namespace esphome
