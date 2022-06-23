#include "SDISerial.h"

/** Max Delay between the measure and data commands */
#define SENSOR_DELAY 1000

/** Pin used for line. Must be interrupt pin. */
#define DATA_PIN 13

SDISerial conn(DATA_PIN);

void setup()
{
	Serial.begin(9600);
	Serial.println("TEROS-12 Address configurator, compiled on " __DATE__ " " __TIME__);

	conn.begin();
	delay(3000);

	char * sens_info = conn.sdi_query("?!", 1000);
	Serial.print("Current Sensor Address: ");
	Serial.print(sens_info);
}

void loop() {}