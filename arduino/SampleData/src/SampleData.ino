/**
 * @file SampleData.ino
 * @author John Madden (jtmadden@ucsc.edu)
 * @brief Prints randomly generated sample data over serial
 * @version 0.1
 * @date 2022-06-30
 */

/** Number of sensors to emulate measurements from */
#define NUM_SENSORS 2

void setup() {
	Serial.begin(9600);
	//Serial.println("SampleData, compiled on " __DATE__ " " __TIME__);

	// Seed random with noise from A0
	random(analogRead(0));

	delay(3000);
}

void loop() {
	// Constant values
	const int moisture = 2500;
	const int temp = 25;
	const int conductivity = 0;

	// Wait for command
	while (Serial.available() == 0) {}

	String cmd = Serial.readStringUntil('\n');
	cmd.trim();

	if (cmd == "MEAS") {
		for (int i=0; i < NUM_SENSORS; i++)	{
			char reading[256];
			sprintf(reading, "%d+%d+%d+%d", i, moisture, temp, conductivity);

			Serial.println(reading);
		}

		Serial.println("END");
	}

	delay(10000);
}