/**
 * @file SampleData.ino
 * @author John Madden (jtmadden@ucsc.edu)
 * @brief Prints randomly generated sample data over serial
 * @version 0.1
 * @date 2022-06-30
 */

void setup() {
	Serial.begin(1200);
	//Serial.println("SampleData, compiled on " __DATE__ " " __TIME__);

	// Seed random with noise from A0
	random(analogRead(0));

	delay(3000);
}

void loop() {
	int moisture = random(2400, 3000);
	int temp = random(20, 30);
	int conductivity = random(300, 400);
		
	char reading[256];
	sprintf(reading, "0+%d+%d+%d", moisture, temp, conductivity);

	Serial.println(reading);

	delay(10000);
}