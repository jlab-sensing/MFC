#include "SDISerial.h"

#define DATA_PIN 13


int sensorDelay = 1000;
char *samples;

SDISerial sdi_serial_connection(DATA_PIN);

void setup()
{
  Serial.begin(1200);
  Serial.println("SDISerial, compiled on " __DATE__ " " __TIME__);

  sdi_serial_connection.begin();
  delay(3000);
}

void loop()
{
  // take repeated samples
  samples = get_measurement();

  // Serial.print("samples(ADDR/RAW/TMP/EC): ");
  Serial.println(samples);
  // sdi_serial_connection.sdi_cmd("0A1!");// change address from 0 to 1
  delay(10000);
}

/**
 * @brief Get measurement from sensor at address
 *
 * @see https://github.com/joranbeasley/SDISerial/blob/master/examples/SDISerialExample/SDISerialExample.ino
 * @return Single measurement
 */
char * get_measurement()
{
  // Query sensor 0
  sdi_serial_connection.sdi_query("0M!", sensorDelay);
  // you can use the time returned above to wait for the service_request_complete
  sdi_serial_connection.wait_for_response(sensorDelay);
  // Get data from sensor
  return (sdi_serial_connection.sdi_query("0D0!", sensorDelay));
}
