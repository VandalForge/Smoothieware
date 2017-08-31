///////////////////////////////////////////////////////////////////////////////////////////////////
/*																								 */
/*	Forge.h																						 */
/*	28 June 2017																				 */
/*	Matthew Buchanan																			 */
/*	Forgeware																					 */
/*																								 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once 									//file will compile only once in a single compilation

#include "libs/Module.h" 						//allows use of Module class as superclass (Smoothie library)
#include <stdint.h>
#include <string>


class Bus;
class Block;

class Forge: public Module {
	public:
		Forge(); 								//class constructor
		~Forge() {}; 							//class destructor
		
		void on_module_loaded();
		void on_idle(void *argument);
		void on_gcode_received(void *argument);
		
		friend class Bus;						//needs access to private data struct in Forge
		
	private:									//use pointers for efficient memory allocation and manipulation
		Bus *controller;						//set up a controller object to talk to the sensors	(should only be one instance)		
		const Block *block;
		
		void get_direction();					//calls the direction finder (only a method not an object)
		void get_current_position();
		void get_temperature();					//calls the bus controller object
		void print_profile();					//calls the printer (only a method not an object)
		
		uint32_t set_tick(uint32_t dummy);
		
		std::string position;
		
		struct { //idea is to only store this data in one location, the other functions/objects will edit this data							
		
			bool tick;
			bool enable;
			
			float frequency;
		
			//current movement direction
			bool north;
			bool south;
			bool east;
			bool west;
			
			//current temperature profile
			float initial_temp;					//temperature read from leading sensor
			float final_temp;					//temperature read from trailing sensor
			
			long double time;
		};
};
