/*
 *	WelderFunction.cpp
 *	created: 10 November 2017
 *	updated: 10 November 2017
 *	Matthew Buchanan, Mitchell Williams
 *	Forgeware
 */

#include "mbed.h"
#include "Kernel.h"
#include "Conveyor.h"
#include "Gcode.h"
#include "WireFeed.h"
#include "Welder.h"
#include "GasFlow.h"
#include "WelderFunction.h"
#include "SlowTicker.h"
#include "StepTicker.h"
#include "Block.h"
 
WelderFunction::WelderFunction() {}
 
WelderFunction::~WelderFunction() {}
 
void WelderFunction::on_module_loaded() {
	
	wirefeed = 	new WireFeed();
	welder = 	new Welder();
	gasflow = 	new GasFlow();
	
	THEKERNEL->slow_ticker->attach(1000, this, &WelderFunction::set_wire_feed);
}

void WelderFunction::on_halt() {}

/*
 * Realized this won't work, need to execute these commands when gcodes execute, not when they are received.
 * Need to use the block strategy that laser.h uses. Check the is_g123 boolean to determine when to turn on or off the welder functions.
 */

void WelderFunction::on_gcode_received(void *argument) {
/*
	Gcode *gcode = static_cast<Gcode *>(argument);
	
    if (gcode->has_g) {
        if (gcode->g == 0) {

			wirefeed->off();
			dwell(300);
			welder->off();
			dwell(300);
			gasflow->off();
        }
		if (gcode->g == 1) {	

			gasflow->on();
			dwell(1000);
			welder->on();
			dwell(300);
			wirefeed->on();
		}
    }
*/
}

void WelderFunction::dwell(int delay_ms) {
	
	if (delay_ms > 0) {
		// drain queue
        THEKERNEL->conveyor->wait_for_idle();
        // wait for specified time
        int start = (int)us_ticker_read(); // mbed call
        while ((us_ticker_read() - start) < delay_ms * 1000) {
            THEKERNEL->call_event(ON_IDLE, this);
            if(THEKERNEL->is_halted()) return;
        }
    }
}

uint32_t WelderFunction::set_wire_feed(uint32_t dummy) {
/*
	const Block *block = StepTicker::getInstance()->get_current_block();
	
	if(block != nullptr && block->is_ready && block->is_g123) {
		wirefeed->on();
    } else {
		wirefeed->off();
	}
*/
}
