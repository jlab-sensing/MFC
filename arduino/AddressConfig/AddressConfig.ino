#include "SDISerial.h"

/** Max Delay between the measure and data commands */
#define SENSOR_DELAY 1000

/** Pin used for line. Must be interrupt pin. */
#define DATA_PIN 13

SDISerial conn(DATA_PIN);

/** New address of TEROS-12 */
const char new_addr = '1';

void setup()
{
	Serial.begin(9600);
	Serial.println("TEROS-12 Address configurator, compiled on " __DATE__ " " __TIME__);

	conn.begin();
	delay(3000);	

	// Query for sensors	
	char * sens_info = conn.sdi_query("?!", 1000);

	Serial.print("Sensor active at ");
	Serial.print(sens_info);

	Serial.print("Changing to ");
	Serial.println(new_addr);

	// Update address
	char a_query[5];
	sprintf(a_query, "%cA%c!", sens_info[0], new_addr);
	char * resp_addr = conn.sdi_query(a_query, 1000);

	Serial.print("Responded ");
	Serial.print(resp_addr);
}

void loop() {}