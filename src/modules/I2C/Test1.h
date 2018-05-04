#ifndef TEST1_H
#define TEST1_H

#include "Module.h"
#include "mbed.h"
#include "PinNames.h"

namespace mbed {
class PwmOut;
class I2C;
}
// class Pin;
// class stepper;
// class Gcode;

class IOTest : public Module {
public:
/************I2C Stuff**************/

/************END I2C***************/
//IOTest(PinName pin) : test_pin(pin);
IOTest();

void on_module_loaded();

void on_gcode_received(void* argument);
//void on_second_tick(void* argument);
//          void on_speed_change(void* argument);

private:
/************I2C Stuff**************/
mbed::I2C* i2c;
int i2cAddr;
/************END I2C***************/
//PinName p = p21;
//PwmOut test_pin;
//PwmOut test_pin; //(P21);
mbed::PwmOut *test_pin;
bool test_on = true;
};


#endif
