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


class Bus;
class Block;

class Forge: public Module {
	public:
		Forge(); 								//class constructor
		~Forge() {}; 							//class destructor
		
		//these events will trigger the appropriate functions to execute
		void on_module_loaded();
		void on_idle(void *argument);
		void on_gcode_received(void *argument);
		
		friend class Bus;						//needs access to private data struct in Forge
		
	private:									//use pointers for efficient memory allocation and manipulation
		Bus *controller;						//set up a controller object to talk to the sensors	(should only be one instance)		
		const Block *block;
		
		//these are the action functions of the forge module
		void get_direction();					//calls the direction finder (only a method not an object)
		void get_temperature();					//calls the bus controller object
		void print_profile();					//calls the printer (only a method not an object)

		uint32_t set_tick(uint32_t dummy);
		
		struct { //idea is to only store this data in one location, the other functions/objects will edit this data							
		
			bool tick;
			bool enable;
		
			//current movement direction
			bool north;							//true if print head is moving toward the north
			bool south;							//true if print head is moving toward the south
			bool east;							//true if print head is moving toward the east
			bool west;							//true if print head is moving toward the west
			
			//current temperature profile
			float initial_temp;					//temperature read from leading sensor
			float final_temp;					//temperature read from trailing sensor
		};
};
