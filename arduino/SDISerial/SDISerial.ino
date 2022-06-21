#include "SDISerial.h"

// Max Delay between the measure and data collection
#define SENSOR_DELAY 1000

#define DATA_PIN 13

// Array of addresses
const char addrs[] = {'0', '1'};
const unsigned int addrs_len = 2;

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
  for (int i = 0; i < addrs_len; i++) {
    char * samples = get_measurement(addrs[i]);
    Serial.print(samples);
  }

  // Serial.print("samples(ADDR/RAW/TMP/EC): ");
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
  sdi_serial_connection.sdi_query(m_query, SENSOR_DELAY);
  // you can use the time returned above to wait for the service_request_complete
  sdi_serial_connection.wait_for_response(SENSOR_DELAY);
  // Get data from sensor
  return (sdi_serial_connection.sdi_query(d_query, SENSOR_DELAY));
}
