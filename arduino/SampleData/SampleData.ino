void setup() {
	Serial.begin(1200);
	Serial.println("SampleData, compiled on " __DATE__ " " __TIME__);

	delay(3000);
}

void loop() {
	Serial.println("0+2639.13+21.8+312");
	delay(10000);
}