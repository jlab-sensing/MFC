#include "SDISerial.h"

/** Max Delay between the measure and data commands */
#define SENSOR_DELAY 1000

/** Pin used for line. Must be interrupt pin. */
#define DATA_PIN 13

SDISerial sdi_serial_connection(DATA_PIN);

void setup()
{
	Serial.begin(9600);
	Serial.println("TEROS-12 Address configurator, compiled on " __DATE__ " " __TIME__);

	sdi_serial_connection.begin();
	delay(3000);
}

void loop() {}