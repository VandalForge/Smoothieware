///////////////////////////////////////////////////////////////////////////////////////////////////
/*																								 */
/*	WireFeed.h																					 */
/*	07 August 2017																				 */
/*	Matthew Buchanan																			 */
/*	Forgeware																					 */
/*																								 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "libs/Module.h"

#include <stdint.h>

namespace mbed { class PwmOut; }

class Pin;

class WireFeed: public Module {
	public:
		WireFeed();
		~WireFeed() {};

		void on_module_loaded();
		void on_gcode_received(void *argument);
		
	private:
	
		float pwm_duty_cycle();
	
		mbed::PwmOut *feed_pin;
		float rate;
		uint32_t period;
		bool feeding;
		float factor;
};