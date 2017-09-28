///////////////////////////////////////////////////////////////////////////////////////////////////
/*																								 */
/*	Forge.cpp																				     */
/*	28 June 2017																				 */
/*	Matthew Buchanan																			 */
/*	Forgeware 																					 */
/*																								 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>					//allows use of data manipulation functions
#include <stdlib.h>						//allows use of the C++ Standard library functions
#include <bitset>						//allows use of type bitset
#include <string>						//allows use of type string
#include <stdint.h>						//allows use of uint8_t
#include <math.h>						//allows use of atan

#include "Forge.h"
#include "Bus.h"
#include "PointList.h"

#include "Module.h"
#include "Kernel.h"
#include "Robot.h"
#include "StreamOutputPool.h"
#include "StepTicker.h"
#include "SlowTicker.h"
#include "Block.h"
#include "ActuatorCoordinates.h"
#include "Gcode.h"
#include "SerialMessage.h"

#define PI 3.141592653

#define POINT_DISTANCE 1
#define N_DISTANCE 13
#define WAIT_DISTANCE 10

Forge::Forge() {
	
	tick = 0;
	enable = 0;
	
	frequency = 300.00F;
	
	north = false;
	south = false;
	east = false;
	west = false;

	block_temp = 660;
	sensor_temp = 661;
	
	time = 0;
	
	count = 0;
	
	block_dir = 664;
	for(int i = 0; i < 3; i++) {
		last_pos[i] = 0;
		pos[i] = 0;
	}
	for(int i = 0; i < NUM_PARAMS; i++) {
		tail_params[i] = 0;
	}
}
void Forge::on_module_loaded() {
	
	this->controller = new Bus(); //should only be initiated once
	this->list = new PointList();
	
	this->register_for_event(ON_IDLE);
	this->register_for_event(ON_GCODE_RECEIVED);
	
	THEKERNEL->slow_ticker->attach((int)frequency, this, &Forge::set_tick);
//	THEKERNEL->streams->append_stream(this);
}
void Forge::on_idle(void *argument) {

	if(tick && enable) {
//		get_direction(); 
//		get_current_position();
//		get_temperature(); 
//		print_profile();
		
		//check if moved the point distance (1 mm)
		//if yes:
		//	make a point with current position and block->temperature
		//	if we have moved > N distance (center distance between print head and ir sensor):
		//		get last point position info
		//		get direction to current position
		//		read correct sensor (function of direction(point to current)
		//		compare sensor value with point temperature value
		//		issue feedback command
		//		delete point
		//number of points at any one time should be equal to N distance / point distance
		
		
		
		THEROBOT->get_current_machine_position(pos);
		block = StepTicker::getInstance()->get_current_block();
		
		if(block != nullptr && block->is_ready && block->is_ticking) { 
			std::bitset<k_max_actuators> bits = block->direction_bits;
			std::string bits_str = bits.to_string();
			char directions[k_max_actuators+1] = {0}; 
			std::copy(bits_str.begin(), bits_str.end(), directions);
			block_temp = block->temperature;
			block_dir = direction(block->steps[0], block->steps[1], directions);
		} // is it possible for a point to be added but the block to be a nullptr?
		
		if(distance(pos, last_pos) >= POINT_DISTANCE) {
			
			count++;
			
			list->add_to_head(pos[0], pos[1], block_temp, block_dir);
//			THEKERNEL->streams->printf("point count: %i\n", list->get_count());
			for(int i = 0; i < 3; i++) {
				last_pos[i] = pos[i];
			}
			if(list->get_count() > N_DISTANCE) {
				list->get_tail_params(&tail_params[0], &tail_params[1], &tail_params[2], &tail_params[3]);
				if((block_dir - tail_params[3] <= 90 && block_dir - tail_params[3] >= -90) || block_dir - tail_params[3] == block_dir) {
					sensor_temp = controller->read(pos, tail_params);
					THEKERNEL->streams->printf("reading from sensor: %f\n", sensor_temp);
					THEKERNEL->streams->printf("current block temp: %f\n", tail_params[2]);
					if(sensor_temp > tail_params[2]) { //the weld is too hot
						THEKERNEL->streams->printf("weld is too hot!\n");
						if(count > WAIT_DISTANCE) {
							send_slow_down_gcode();
							count = 0;
						}
					}
					if(tail_params[2] > sensor_temp) { //the weld is too cold
						THEKERNEL->streams->printf("weld is too cold!\n");
						if(count > WAIT_DISTANCE) {
							send_speed_up_gcode();
							count = 0;
						}
					}
					//compare sensor_temp with tail_params[2]
					//issue feedback command
				}
				list->delete_tail();
//				THEKERNEL->streams->printf("tail: x_pos: %f, y_pos: %f, temp: %f\n", tail_params[0], tail_params[1], tail_params[2]);
			}
		}
		tick = 0;
	}
/*	else if (!enable) {
		list->clear_list();
	}
*/
/**/
}
void Forge::on_gcode_received(void *argument) {
	
	Gcode *gcode = static_cast<Gcode *>(argument);

	if (gcode->has_g) {
		if (gcode->g == 28) {		//reset the timer on homing
			time = 0;
		}
	}
    if (gcode->has_m) {
        if (gcode->m == 701) { 		// enable temperature profile recording, tool path is starting
			enable = 1;
        }
		else if (gcode->m == 702) { // disable temperature profile recording, tool path has ended
			enable = 0;
		}
    }
}
uint32_t Forge::set_tick(uint32_t dummy) {
	
	if(!tick) {tick = 1;}
	if(enable) {time += (1.00F/frequency);}	
	return 0;
}
/*
uint32_t Forge::pos_check(uint32_t dummy) {
	
//	position.clear();
	float pos[3];
	THEROBOT->get_current_machine_position(pos);
	if(distance(pos, last_pos) > 5) {
		
	}
	
}
*/
void Forge::get_direction() {
 
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
		
/*	code replaced with direction() function
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
*/		
		block_dir = direction(block->steps[0], block->steps[1], temp);
		
//		THEKERNEL->streams->printf("X: %o, Y: %o, Z: %o\n", block->steps[0], block->steps[1], block->steps[2]);
//		THEKERNEL->streams->printf("Direction Bits: %c%c\n", temp[3], temp[4]);
//		THEKERNEL->streams->printf("Direction: %i\n", direction((int)block->steps[0], (int)block->steps[1], temp));
	}
}
void Forge::get_current_position() {
	
	position.clear();
	THEROBOT->print_position(1, position, true);
}
void Forge::get_temperature() {

//	controller->get_temp(this);
	this->sensor_temp = controller->read(block_dir);
}
/*
void Forge::print_profile() {

	if(north) {
		if(east) {
			THEKERNEL->streams->printf("Direction:NE \nInitial Temp:%4.2f \nFinal Temp:%4.2f\n", initial_temp, final_temp);
			THEKERNEL->streams->printf("Current Position: %s\n", position.c_str());
			THEKERNEL->streams->printf("Time: %7.2Lf\n", time);
		}
		else if(west) {
			THEKERNEL->streams->printf("Direction:NW \nInitial Temp:%4.2f \nFinal Temp:%4.2f\n", initial_temp, final_temp);
			THEKERNEL->streams->printf("Current Position: %s\n", position.c_str());
			THEKERNEL->streams->printf("Time: %7.2Lf\n", time);
		}
		else {
			THEKERNEL->streams->printf("Direction:N  \nInitial Temp:%4.2f \nFinal Temp:%4.2f\n", initial_temp, final_temp);
			THEKERNEL->streams->printf("Current Position: %s\n", position.c_str());
			THEKERNEL->streams->printf("Time: %7.2Lf\n", time);
		}
	}
	else if(south) {
		if(east) {
			THEKERNEL->streams->printf("Direction:SE \nInitial Temp:%4.2f \nFinal Temp:%4.2f\n", initial_temp, final_temp);
			THEKERNEL->streams->printf("Current Position: %s\n", position.c_str());
			THEKERNEL->streams->printf("Time: %7.2Lf\n", time);
		}
		else if(west) {
			THEKERNEL->streams->printf("Direction:SW \nInitial Temp:%4.2f \nFinal Temp:%4.2f\n", initial_temp, final_temp);
			THEKERNEL->streams->printf("Current Position: %s\n", position.c_str());
			THEKERNEL->streams->printf("Time: %7.2Lf\n", time);
		}
		else {
			THEKERNEL->streams->printf("Direction:S  \nInitial Temp:%4.2f \nFinal Temp:%4.2f\n", initial_temp, final_temp);
			THEKERNEL->streams->printf("Current Position: %s\n", position.c_str());
			THEKERNEL->streams->printf("Time: %7.2Lf\n", time);
		}
	}
	else if(west) {
		THEKERNEL->streams->printf("Direction:W  \nInitial Temp:%4.2f \nFinal Temp:%4.2f\n", initial_temp, final_temp);
		THEKERNEL->streams->printf("Current Position: %s\n", position.c_str());
		THEKERNEL->streams->printf("Time: %7.2Lf\n", time);
	}
	else if(east) {
		THEKERNEL->streams->printf("Direction:E  \nInitial Temp:%4.2f \nFinal Temp:%4.2f\n", initial_temp, final_temp);
		THEKERNEL->streams->printf("Current Position: %s\n", position.c_str());
		THEKERNEL->streams->printf("Time: %7.2Lf\n", time);
	}
}
*/
float Forge::direction(float x, float y, char* dir) {
	
	double angle = std::atan2(x,y) * 180.00/PI;
	if 		(y == 0 && dir[4] == '1') 		{return 90;} 
	else if (y == 0 && dir[4] == '0') 		{return 270;} 
	else if (x == 0 && dir[3] == '1') 		{return 180;}
	else if (x == 0 && dir[3] == '0') 		{return 0;}
	else if (dir[3] == '0' && dir[4] == '1') 	{return angle;}		 		//1st quadrant (0-90 degrees)
	else if (dir[3] == '0' && dir[4] == '0') 	{return 360 - angle;}   	//2st quadrant (270-360 degrees)
	else if (dir[3] == '1' && dir[4] == '0') 	{return angle + 180;}   	//3rd quadrant (180-270 degrees)
	else if (dir[3] == '1' && dir[4] == '1') 	{return 180 - angle;}	 	//4th quadrant (90-180 degrees)
	else {
			return 666;	//error in function
	}
}
int Forge::distance(float* fin, float* init) {
	
	return (int)std::sqrt( std::pow((fin[1]-init[1]),2) + std::pow((fin[0]-init[0]),2) );
}

void Forge::send_speed_up_gcode() {
	
	float factor = 6000.0/THEROBOT->get_seconds_per_minute();
	factor += 5;
	char mes[8] = {'0'};
	sprintf(mes, "%f", factor);
    struct SerialMessage message;
    message.message = "M220 S";
	message.message.append(mes);
    message.stream = &(StreamOutput::NullStream);
	THEKERNEL->streams->printf("Sending Override Command\n");
    THEKERNEL->call_event(ON_CONSOLE_LINE_RECEIVED, &message );
}

void Forge::send_slow_down_gcode() {
	
	float factor = 6000.0/THEROBOT->get_seconds_per_minute();
	factor -= 5;
	char mes[8] = {'0'};
	sprintf(mes, "%f", factor);
    struct SerialMessage message;
    message.message = "M220 S";
	message.message.append(mes);
    message.stream = &(StreamOutput::NullStream);
	THEKERNEL->streams->printf("Sending Override Command\n");
    THEKERNEL->call_event(ON_CONSOLE_LINE_RECEIVED, &message );
}