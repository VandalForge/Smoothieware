///////////////////////////////////////////////////////////////////////////////////////////////////
/*																								 */
/*	Bus.h																						 */
/*	28 June 2017																				 */
/*	Matthew Buchanan																			 */
/*	Forgeware																		 			 */
/*																								 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "mlx90614.h"

namespace mbed { class I2C; } 					//allows use of pointers to I2C objects (mbed library)

class Forge;									//allows use of pointers to Forge objects (Forgeware)

class Bus {
	public:
		Bus();									//class constructor
		~Bus();									//class destructor
		
//		void get_temp(Forge* f);				//public member function, called to set the temperature values in Forge
		
		float read(float dir);
		float read(float* curr, float* past);
		
	private:
		mbed::I2C sensor_bus(PinName, PinName);	//setup the I2C bus with mbed-style pins
		MLX90614 sensor(I2C*);					//setup the IR sensor
		
		float read_sensor(int addr);			//public member function, called to return the value from a sensor
		
		
		struct { 
			
			//sensor variables
			int sens[8];							//sensor dummy variables, currently just testing with one sensor
		};
};
