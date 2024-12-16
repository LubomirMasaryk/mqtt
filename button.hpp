#pragma once

#include "soc/gpio_num.h"
class Button {
private:
	gpio_num_t pin;
	bool state = false;
	bool prevPressedState = false;
	bool prevReleasedState = false;

public:
	Button(gpio_num_t pin);
	bool isPressed();
	bool getPress();
	bool getRelease();
};