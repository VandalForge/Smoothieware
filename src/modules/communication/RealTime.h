

// Real-Time Header File

#include <stdint.h>

// Serial Real-Time Commands
#define FEED_OVR_RESET 			0xF0 	//240
#define FEED_OVR_COARSE_PLUS	0xF1 	//241
#define FEED_OVR_COARSE_MINUS	0xF2	//242
#define FEED_OVR_FINE_PLUS		0xF3	//243
#define FEED_OVR_FINE_MINUS		0xF4	//244

//Real-Time Status Flags
#define EXEC_FEED_RESET			bit(0)	//0b00000001
#define EXEC_FEED_COARSE_PLUS	bit(1)	//0b00000010
#define EXEC_FEED_COARSE_MINUS	bit(2)	//0b00000100
#define EXEC_FEED_FINE_PLUS		bit(3)	//0b00001000
#define EXEC_FEED_FINE_MINUS	bit(4)	//0b00010000

volatile uint8_t rt_flag = 0;

set_flag(uint8_t temp) {

	//in GRBL some register manipulation happens in this function
	//for now this just sets a global system flag that will be checked by another function
	rt_flag = temp;

}