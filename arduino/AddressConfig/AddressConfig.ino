int i = 0;

void setup() {
	Serial.begin(9600);

	Serial.println("Hello World");
}

void loop() {
	Serial.println(i++);
	delay(500);
}