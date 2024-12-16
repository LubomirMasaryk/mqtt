#pragma once
#include "soc/gpio_num.h"

class Led {
private:
	bool isOn;
	gpio_num_t pin;

public:
	Led(gpio_num_t pin, bool isOn = false);
	void toggle();
	void turnOn();
	void turnOff();
	bool getState();
	gpio_num_t getPin();
};