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
		mbed::PwmOut *feed_pin;
		int rate;
		int multiplier;
		uint32_t period;
		bool feeding;
};