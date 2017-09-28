///////////////////////////////////////////////////////////////////////////////////////////////////
/*																								 */
/*	Bus.cpp																					     */
/*	28 June 2017																				 */
/*	Matthew Buchanan																			 */
/*	Forgeware																					 */
/*																								 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "I2C.h"
#include "Bus.h"
#include "Forge.h"
#include "Kernel.h"				//for debugging
#include "StreamOutputPool.h"	//for debugging
#include "mlx90614.h"

#define SDA p28
#define SCL p27

#define NORTH 		sens[0]
#define NORTHEAST 	sens[1]
#define EAST 		sens[2]
#define SOUTHEAST 	sens[3]
#define SOUTH 		sens[4]
#define SOUTHWEST 	sens[5]
#define WEST 		sens[6]
#define NORTHWEST 	sens[7]

#define PI 3.141592653

Bus::Bus() {
	sens[0] = 0x0A;		//north
	sens[1] = 0x14;		//northeast
	sens[2] = 0x2A;		//east
	sens[3] = 0x3A;		//southeast
	sens[4] = 0x4A;		//south
	sens[5] = 0x5A;		//southwest
	sens[6] = 0x6A;		//west
	sens[7] = 0x7A;		//northwest
}
/*
void Bus::get_temp(Forge* f) {

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
*/
float Bus::read_sensor(int addr) { 
 
/*	//use for testing without sensors
	if 		(addr == SOUTH) 	{return 5;}
	else if	(addr == SOUTHWEST)	{return 6;}
	else if (addr == WEST)		{return 7;}
	else if (addr == NORTHWEST) {return 8;}
	else if (addr == NORTH) 	{return 1;}
	else if (addr == NORTHEAST) {return 2;}
	else if (addr == EAST)		{return 3;}
	else if (addr == SOUTHEAST) {return 4;}
	else {
		return 665;	//error code
	}
/**/
	//Commented out for testing without sensors
	//get an error when below functions are in constructor
	mbed::I2C sensor_bus(SDA, SCL);					
	MLX90614 IR_thermometer(&sensor_bus, addr);

	float rawObj = 0;
	if (IR_thermometer.getTemp(&rawObj)) {
		return rawObj;
	}
	else {
		THEKERNEL->streams->printf("Failed I2C Read in Bus.cpp::read_sensor()\n");
		return 0; //indicate a failed read
	}
/**/
}
float Bus::read(float direction) {
	
	if	    (direction >= 338 || direction < 23) 	{return read_sensor(SOUTH);}
	else if (direction >= 23  && direction < 68)  	{return read_sensor(SOUTHWEST);}
	else if (direction >= 68  && direction < 113) 	{return read_sensor(WEST);}
	else if (direction >= 113 && direction < 158) 	{return read_sensor(NORTHWEST);}
	else if (direction >= 158 && direction < 203) 	{return read_sensor(NORTH);}
	else if (direction >= 203 && direction < 248) 	{return read_sensor(NORTHEAST);}
	else if (direction >= 248 && direction < 293) 	{return read_sensor(EAST);}
	else if (direction >= 293 && direction < 338) 	{return read_sensor(SOUTHEAST);}
	else {
//		THEKERNEL->streams->printf("problem: direction is: %i\n", direction);
		return 666;	//error code
	}
}
float Bus::read(float* curr, float* past) {
		
//		float d = 0;
		double delta_x = curr[0] - past[0];
		double delta_y = curr[1] - past[1];
		
		//this is basically the same direction finder as in Forge.cpp - needs to be optimized
		double angle = /*std::*/atan2(delta_x,delta_y) * 180.00/PI;
		if(angle > 0) angle = 360 - angle;
		if(angle < 0) angle = -angle;
		
		return read((float)angle);
		
//		if(angle != 0) angle = 360 - angle;
		
/*
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
*/		
		
//		return angle; //Bus::read(d);
}