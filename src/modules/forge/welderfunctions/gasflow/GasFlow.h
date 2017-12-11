/*
 *	GasFlow.h
 *	2 October 2017
 *	Matthew Buchanan
 *	Forgeware
 */

#ifndef GAS_FLOW_H
#define GAS_FLOW_H

#include <stdint.h>

#include "Module.h"
#include "gpio.h"

class GasFlow: public Module {
	public:
		GasFlow();
		~GasFlow() {};
		
		void on_module_loaded();
		void on_halt();
		void on_gcode_received(void *argument);
		
		void on();
		void off();
		
	private:
	
		GPIO gas_pin = GPIO(P2_7);
};

#endif