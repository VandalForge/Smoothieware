///////////////////////////////////////////////////////////////////////////////////////////////////
/*																								 */
/*	Bus.cpp																					     */
/*	28 June 2017																				 */
/*	Matthew Buchanan																			 */
/*	Forgeware																					 */
/*																								 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "I2C.h"
#include "Bus.h"
#include "Forge.h"
#include "Kernel.h"				//for debugging
#include "StreamOutputPool.h"	//for debugging
#include "mlx90614.h"

#define SDA p28
#define SCL p27

Bus::Bus() {
	sens[0] = 0x5A;		//north
	sens[1] = 0xB4;		//northeast
	sens[2] = 0x2A;		//east
	sens[3] = 0x3A;		//southeast
	sens[4] = 0xB4;		//south
	sens[5] = 0x5A;		//southwest
	sens[6] = 0x6A;		//west
	sens[7] = 0x7A;		//northwest
}
void Bus::get_temp(Forge* f) {
/*
 *	This function decides which sensors to read from based on the direction the print head is currently moving.
 *	The idea is to read the temperature immediately before and immediately after the weld puddle passes over.
 *	NOTE: For now only one sensor is being tested, so the addresses passed in this function are not used yet.
 */
	if(f->north) {
		if(f->east) 		{f->initial_temp = read_sensor(sens[1]); f->final_temp = read_sensor(sens[5]);}
		else if(f->west) 	{f->initial_temp = read_sensor(sens[7]); f->final_temp = read_sensor(sens[3]);}
		else				{f->initial_temp = read_sensor(sens[0]); f->final_temp = read_sensor(sens[4]);}
	}
	else if (f->south) {	
		if(f->east)			{f->initial_temp = read_sensor(sens[3]); f->final_temp = read_sensor(sens[7]);}
		else if(f->west)	{f->initial_temp = read_sensor(sens[5]); f->final_temp = read_sensor(sens[1]);}
		else				{f->initial_temp = read_sensor(sens[4]); f->final_temp = read_sensor(sens[0]);}
	}
	else if (f->west)		{f->initial_temp = read_sensor(sens[6]); f->final_temp = read_sensor(sens[2]);}
	else if (f->east)		{f->initial_temp = read_sensor(sens[2]); f->final_temp = read_sensor(sens[6]);}
}
void Bus::get_address(uint8_t* addr) {
	
	mbed::I2C sensor_bus(SDA, SCL);					
	MLX90614 IR_thermometer(&sensor_bus, 0xB4);

	if (IR_thermometer.getAddress(addr)) {
//		return addr;
	}
	else {
		THEKERNEL->streams->printf("Failed I2C Read in Bus.cpp::get_address()\n");
//		return 0; //indicate a failed read
	}
}
float Bus::read_sensor(int addr) {  
/*
 *	This function calls the getTemp function from the MLX90614 library, which performs the I2C read.
 *	This will print a "Failed Read" to the serial monitor upon I2C read failure (NACK).
 */
	//get an error when below functions are in constructor
	mbed::I2C sensor_bus(SDA, SCL);					
	MLX90614 IR_thermometer(&sensor_bus, 0xB4);

	float rawObj = 0;
	if (IR_thermometer.getTemp(&rawObj)) {
		return rawObj;
	}
	else {
		THEKERNEL->streams->printf("Failed I2C Read in Bus.cpp::read_sensor()\n");
		return 0; //indicate a failed read
	}
}
