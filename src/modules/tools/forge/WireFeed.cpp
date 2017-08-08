///////////////////////////////////////////////////////////////////////////////////////////////////
/*																								 */
/*	WireFeed.cpp																					 */
/*	07 August 2017																				 */
/*	Matthew Buchanan																			 */
/*	Forgeware																					 */
/*																								 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "WireFeed.h"

#include "Module.h"
#include "Kernel.h"
#include "Gcode.h"
#include "PwmOut.h" // mbed.h lib
#include "Pin.h"
#include "StreamOutputPool.h"

#define FEEDPIN "1.23"

WireFeed::WireFeed() { 

	rate = 0; //need to put in a default wire feed rate
	multiplier = 1;
	period = 20;
	feeding = 0;
}

void WireFeed::on_module_loaded() { 

	Pin* dummypin = new Pin;
	dummypin->from_string(FEEDPIN)->as_output();

	feed_pin = dummypin->hardware_pwm();

	if (feed_pin == NULL) {

		THEKERNEL->streams->printf("Error in pin initialization\n");
		delete dummypin;
		delete this;
		return;
	}
	delete dummypin;
	dummypin = NULL;

	this->feed_pin->period_us(period);

	this->register_for_event(ON_GCODE_RECEIVED);
}

void WireFeed::on_gcode_received(void *argument) { 

	Gcode *gcode = static_cast<Gcode *>(argument);

    // M codes execute immediately
    if (gcode->has_m) {
        if (gcode->m == 750) { 
            if(gcode->has_letter('R')) { //this will be the wire feed rate in ipm or mmpm, will be a modal command
                this->rate = gcode->get_value('R');
            }
            this->feed_pin->write(rate * multiplier); //need to put in the relationship between duty cycle and rate
            this->feeding = 1;
        }
        if (gcode->m == 760) {
        	this->feed_pin->write(0);
        	this->feeding = 0;
        }
        if (gcode->m == 755) { //this will be the wire feed rate override command
        	if(gcode->has_letter('S')) { 
        		this->multiplier = gcode->get_value('S');
        		if(this->feeding) {
        			this->feed_pin->write(rate *multiplier);
        		} else {
        			this->feed_pin->write(0);
        		}

        	}
        }
    }
}