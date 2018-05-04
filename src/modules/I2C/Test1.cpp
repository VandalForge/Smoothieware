#include "Test1.h"

#include "mbed.h"
#include "libs/Module.h"
#include "libs/Kernel.h"
#include "modules/communication/utils/Gcode.h"
#include "modules/robot/Stepper.h"
#include "Laser.h"
#include "libs/nuts_bolts.h"
#include "Config.h"
#include "StreamOutputPool.h"
#include "Block.h"
#include "checksumm.h"
#include "ConfigValue.h"

#include "libs/Pin.h"
#include "Gcode.h"
#include "PwmOut.h" // mbed.h lib

#define PWMPIN "2.5"
#define SDA p28 // "0.10"
#define SCL p27 // "0.11"
//#define Freq 10000


IOTest::IOTest() {
//      this->i2c = i2c;
//      this->test_pin.period_us(10);
}



void IOTest::on_module_loaded() {

	Pin* dummypin = new Pin; //create new pin variable to assign IO to
	dummypin->from_string(PWMPIN)->as_output();

	test_pin = dummypin->hardware_pwm();

	if (test_pin == NULL) {

		THEKERNEL->streams->printf("Error in pin initialization\n");
		delete dummypin;
		delete this;
		return;
	}
	delete dummypin;
	dummypin = NULL;

	this->register_for_event(ON_GCODE_RECEIVED);
	//this->register_for_event(ON_SECOND_TICK);

}

void IOTest::on_gcode_received(void* argument){
	//mbed::I2C i2c(SDA,SCL);

	mbed::I2C i2c(SDA,SCL);

	int success = 1;

	int address1 = 0x51;
	/*
	char cmdT[] = {0xAF};
	char fullT[] = {0xFF};
	char halfT[] = {0x7F};
	char testresT[] = {0xE9};
	char zeroT[] = {0x00};
	char* valuesTest[] = {zeroT, halfT, fullT};
	*/
	//char addresses[] = {0x51, 0x53, 0x55, 0x57, 0x59, 0x5B, 0x5D, 0x5F}; //READ
	char addresses[] = {0x50, 0x52, 0x54, 0x56, 0x58, 0x5A, 0x5C, 0x5E}; //WRITE
	char commands[] = {0xA9, 0xAA, 0xAF}; // commands[] = pot0, pot1, both
//        char data[2];

	char values[256] = {};

	for (int i = 0; i < 256; i++)
	{
		values[i] = i;
	}



	Gcode* gcode = static_cast<Gcode*>(argument);

	//Easy Frequency change for testing
	//MAX FREQUENCY = 3700*1000;
	//MIN FREQUENCY = 3*1000;
	if(gcode->has_letter('H'))
	{
		int Freq = gcode->get_value('H');
		Freq = Freq*1000;
				i2c.frequency(Freq);
	}

	if( gcode->has_letter('I')) { //reads values 0-100(%)



		/*
		   THEKERNEL->streams->printf("READ G-CODE:I\n");
		   int code = gcode->get_value('I');
		   if(code == 0){         // I0

		    success = i2c.write2(address1, cmd, full, 1, false);
		    THEKERNEL->streams->printf("0 = N, 1 = Y: %i\n", success);

		   }else if( code == 1 ){ // I1

		    success = i2c.write2(address1, cmd, testres, 1, false);
		    THEKERNEL->streams->printf("0 = N, 1 = Y: %i\n", success);

		   }else if( code == 2){ // I2

		    success = i2c.write2(address1, cmd, zero, 1, false);
		    THEKERNEL->streams->printf("0 = N, 1 = Y: %i\n", success);

		   }
		 */
		THEKERNEL->streams->printf("READ I-CODE\n");
		int code = gcode->get_value('I');
		code = code*2.55; //convert from %% to 255 base
		THEKERNEL->streams->printf("code = %i\n", code);
		char valWrite[1] = {values[code]};


		if(gcode->has_letter('C')) { //reads values 0-9, default is a value used for testing only
			int Ccode = gcode->get_value('C');

			//Initialized to default vals
			int address = addresses[6]; // A1,A2 = 5v
			char cmd[1] = {commands[0]}; // pot0

			switch(Ccode) {
			case 1:
				address = addresses[0]; // all grounded
				cmd[0] = commands[1]; // pot1
				break;
			case 2:
				address = addresses[0]; // all grounded
				cmd[0] = commands[0]; // pot0
				break;
			case 3:
				address = addresses[2]; // A2 = 5v
				cmd[0] = commands[0]; // pot0
				success = i2c.write2(address, cmd, valWrite, 1, false);

				address = addresses[7]; // all = 5v
				cmd[0] = commands[0]; // pot0
				break;
			case 4:
				address = addresses[1];         // A0 = 5v
				cmd[0] = commands[1];         // pot1
				break;
			case 5:
				address = addresses[3]; // A0,A1 = 5v
				cmd[0] = commands[0]; // pot0
				break;
			case 6:
				//ADD STUFF
				address = addresses[2]; // A2 = 5v
				cmd[0] = commands[1]; // pot0
				success = i2c.write2(address, cmd, valWrite, 1, false);

				address = addresses[7];         // all = 5v
				cmd[0] = commands[1];         // pot1
				break;
			case 7:
				address = addresses[1]; // A0 = 5v
				cmd[0] = commands[0]; // pot0
				break;
			case 8:
				address = addresses[3]; // A0,A1 = 5v
				cmd[0] = commands[1]; // pot1
				break;
			case 9:
				address = addresses[5]; // A0,A2 = 5V
				cmd[0] = commands[1]; // pot0
				break;
			default:
				address = addresses[6]; // A1,A2 = 5v
				cmd[0] = commands[0]; //pot1
				break;
			}

//			int i = 0;

//			while(i < 1000)
//			{
				success = i2c.write2(address, cmd, valWrite, 1, false);
//				i++;
//			}

			THEKERNEL->streams->printf("0 = N, 1 = Y: %i\n", success);

		}

	}

}
/*
   void IOTest::on_second_tick(void* argument) {
        THEKERNEL->streams->printf("I'm alive...\n");
   }
 */
