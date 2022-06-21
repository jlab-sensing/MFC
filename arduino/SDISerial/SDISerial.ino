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
  samples = get_measurement('0');

  // Serial.print("samples(ADDR/RAW/TMP/EC): ");
  Serial.println(samples);
  // sdi_serial_connection.sdi_cmd("0A1!");// change address from 0 to 1
  delay(10000);
}

/**
 * @brief Get measurement from sensor at address
 * 
 * @param _addr Address of sensor
 *
 * @see https://github.com/joranbeasley/SDISerial/blob/master/examples/SDISerialExample/SDISerialExample.ino
 * @return Single measurement
 */
char * get_measurement(char _addr)
{
  // Measure query
  char m_query[4];
  // Data query
  char d_query[5];

  // Format query strings
  sprintf(m_query, "%cM!", _addr);
  sprintf(d_query, "%cD0!", _addr);

  // Query sensor 0
  sdi_serial_connection.sdi_query(m_query, sensorDelay);
  // you can use the time returned above to wait for the service_request_complete
  sdi_serial_connection.wait_for_response(sensorDelay);
  // Get data from sensor
  return (sdi_serial_connection.sdi_query(d_query, sensorDelay));
}
