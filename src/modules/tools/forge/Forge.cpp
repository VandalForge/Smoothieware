///////////////////////////////////////////////////////////////////////////////////////////////////
/*																								 */
/*	Forge.cpp																				     */
/*	28 June 2017																				 */
/*	Matthew Buchanan																			 */
/*	Forgeware 																					 */
/*																								 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Forge.h"
#include "Bus.h"

#include "Module.h"
#include "Kernel.h"
#include "StreamOutputPool.h"
#include "StepTicker.h"
#include "SlowTicker.h"
#include "Block.h"
#include "ActuatorCoordinates.h"
#include "Gcode.h"

#include <algorithm>					//allows use of data manipulation functions
#include <stdlib.h>						//allows use of the C++ Standard library functions
#include <bitset>						//allows use of type bitset
#include <string>						//allows use of type string
#include <stdint.h>						//allows use of uint8_t

Forge::Forge() {
	
	tick = 0;
	enable = 0;
	
	north = false;
	south = false;
	east = false;
	west = false;

	initial_temp = 0;
	final_temp = 0;
}
void Forge::on_module_loaded() {
	
	this->controller = new Bus(); //should only be initiated once
	
	this->register_for_event(ON_IDLE);
	this->register_for_event(ON_GCODE_RECEIVED);
	
	THEKERNEL->slow_ticker->attach(4, this, &Forge::set_tick);
}
void Forge::on_idle(void *argument) {
	
	if(tick && enable) {
		get_direction(); 
		get_temperature(); 
		print_profile();
		tick = 0;
	}
}
void Forge::on_gcode_received(void *argument) {
	
	Gcode *gcode = static_cast<Gcode *>(argument);

    if (gcode->has_m) {
        if (gcode->m == 701) { // enable temperature profile recording
			enable = 1;
        }
		else if (gcode->m == 702) { // disable temperature profile recording
			enable = 0;
		}
    }
}
uint32_t Forge::set_tick(uint32_t dummy) {
	
	if(!tick) {tick = 1;}
	return 0;
}
void Forge::get_direction() {
/*	
 *	This function finds the current direction of the print head by comparing the current
 *	step profile and direction bits of the current block.
 */
 
 	north = false;
	south = false;
	east = false;
	west = false;
 
	block = StepTicker::getInstance()->get_current_block();
	
	if(block != nullptr && block->is_ready && block->is_ticking) { 
		std::bitset<k_max_actuators> bits = block->direction_bits;
		std::string bits_str = bits.to_string();
		char temp[k_max_actuators+1] = {0}; 
        std::copy(bits_str.begin(), bits_str.end(), temp);

		if(block->steps[0] > 0 && block->steps[1] > 0) { //possible directions are NW, SW, NE, SE
			if(temp[3] == '1' && temp[4] == '1') 		{south = true; east = true;}
			else if(temp[3] == '0' && temp[4] == '0') 	{north = true; west = true;}
			else if(temp[3] == '1' && temp[4] == '0')	{south = true; west = true;}
			else if(temp[3] == '0' && temp[4] == '1')	{north = true; east = true;}
		}
		if(block->steps[0] > 0 && block->steps[1] == 0) { //possible directions are W and E
			if(temp[4] == '1') 	east = true;
			else 				west = true;
		}
		if(block->steps[0] == 0 && block->steps[1] > 0) { //possible directions are N and S
			if(temp[3] == '1') 	south = true;
			else 				north = true;
		}
	}
}
void Forge::get_temperature() {
/*
 *	This function finds the initial and final temperatures at each interval, using the direction profile
 *	to determine which sensors to read from.
 */	
	controller->get_temp(this);
}
void Forge::print_profile() {
/*
 *	This function prints the direction and temperature profile to the serial monitor.
 */
	if(north) {
		if(east) {
			THEKERNEL->streams->printf("Direction:NE - Initial Temp:%f - Final Temp:%f\n", initial_temp, final_temp);
		}
		else if(west) {
			THEKERNEL->streams->printf("Direction:NW - Initial Temp:%f - Final Temp:%f\n", initial_temp, final_temp);
		}
		else {
			THEKERNEL->streams->printf("Direction:N  - Initial Temp:%f - Final Temp:%f\n", initial_temp, final_temp);
		}
	}
	else if(south) {
		if(east) {
			THEKERNEL->streams->printf("Direction:SE - Initial Temp:%f - Final Temp:%f\n", initial_temp, final_temp);
		}
		else if(west) {
			THEKERNEL->streams->printf("Direction:SW - Initial Temp:%f - Final Temp:%f\n", initial_temp, final_temp);
		}
		else {
			THEKERNEL->streams->printf("Direction:S  - Initial Temp:%f - Final Temp:%f\n", initial_temp, final_temp);
		}
	}
	else if(west) {
		THEKERNEL->streams->printf("Direction:W  - Initial Temp:%f - Final Temp:%f\n", initial_temp, final_temp);
	}
	else if(east) {
		THEKERNEL->streams->printf("Direction:E  - Initial Temp:%f - Final Temp:%f\n", initial_temp, final_temp);
	}
}
