/*
 *	WelderFunction.h
 *	created: 10 November 2017
 *	updated: 10 November 2017
 *	Matthew Buchanan, Mitchell Williams
 *	Forgeware
 */
 
#ifndef WELDER_FUNCTION_H
#define WELDER_FUNCTION_H

#include "Module.h"

class WireFeed;
class Welder;
class GasFlow;

class WelderFunction: public Module {
	
public:
	WelderFunction();
	~WelderFunction();
	
	void on_module_loaded();
	void on_halt();
	void on_gcode_received(void *argument);
	
private:
	
	WireFeed 	*wirefeed;
	Welder 		*welder;
	GasFlow		*gasflow;
	
	void dwell(int delay_ms);
	uint32_t set_wire_feed(uint32_t dummy);
	
};

#endif