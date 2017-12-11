/*
 *	Welder.h
 *	2 October 2017
 *	Matthew Buchanan
 *	Forgeware
 */

#ifndef WELDER_H
#define WELDER_H

#include <stdint.h>

#include "Module.h"
#include "gpio.h"

class Welder: public Module {
	public:
		Welder();
		~Welder() {};
		
		void on_module_loaded();
		void on_halt();
		void on_gcode_received(void *argument);
		
		void on();
		void off();
	
	private:
	
		GPIO on_off_pin = GPIO(P1_22);
		GPIO high_low_pin = GPIO(P2_6);
		GPIO polarity_pin = GPIO(P2_4);
};

#endif