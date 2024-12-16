#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "soc/gpio_num.h"
#include "led.hpp"

Led::Led(gpio_num_t pin, bool isOn) : isOn(isOn), pin(pin) {
    gpio_reset_pin(this->pin);
    gpio_set_direction(this->pin, GPIO_MODE_OUTPUT);
    gpio_set_level(this->pin, this->isOn);
}

void Led::toggle() {
    this->isOn = !this->isOn;
    gpio_set_level(this->pin, this->isOn);
}

void Led::turnOn() {
    if (this->isOn == true) return;
    this->isOn = true;
    gpio_set_level(this->pin, 1);
}

void Led::turnOff() {
    if (this->isOn == false) return;
    this->isOn = false;
    gpio_set_level(this->pin, false);
}

gpio_num_t Led::getPin() {
    return this->pin;
}

bool Led::getState() {
    return this->isOn;
}