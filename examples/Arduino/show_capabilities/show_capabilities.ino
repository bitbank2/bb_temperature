//
// Example sketch for bb_temperature
// written by Larry Bank
// bitbank@pobox.com
//
// This example shows how to query the type and capabilities
// of a supported sensor and use that info to display the values
// to the serial terminal.
//
#include <bb_temperature.h>
BBTemp bbt;
BBT_SAMPLE bbts;

const char *szDevices[] = {"Not found", "AHT20", "BMP180", "BME280", "BMP388", "SHT3X", "HDC1080", "HTS221", "MCP9808"};
const char *szCaps[] = {"Temperature", "Humidity", "Pressure"};
char szTemp[32];
uint32_t u32Caps;

void setup()
{
int rc;

  Serial.begin(115200);
  delay(3000);
  rc = bbt.init(); // default I2C pins are defined for this target device, no need to pass any parameters to init
  if (rc == BBT_SUCCESS) {
    sprintf(szTemp, "Found: %s", szDevices[bbt.type()]);
    Serial.println(szTemp);
    Serial.print("With capabilities: ");
    u32Caps = bbt.caps();
    for (int i=0; i<3; i++) {
      if (u32Caps & (1<<i)) {
        Serial.print(szCaps[i]);
        Serial.print(", ");
      }
    }
    Serial.println(" ");
  } else {
    Serial.println("Failed");
    while (1) {};
  }
  bbt.start();
} /* setup() */

void loop()
{
     while (1) { // loop forever
       bbt.getSample(&bbts); // Read the latest sensor value(s) into our structure
       if (u32Caps & BBT_CAP_TEMPERATURE) {
        // The temperature is an integer reported as 10x the actual value
        // This allow for displaying accuracy to 1/10th of a degree celcius without
        // the use of floating point math (which is not present on some MCUs)
          sprintf(szTemp, "Temp: %d.%dC", bbts.temperature / 10, bbts.temperature % 10);
          Serial.println(szTemp);
       }
       if (u32Caps & BBT_CAP_HUMIDITY) {
          sprintf(szTemp, "Humidity: %d%%", bbts.humidity);
          Serial.println(szTemp);
       }
       if (u32Caps & BBT_CAP_PRESSURE) {
          sprintf(szTemp, "Pressure: %d hPa", bbts.pressure);
          Serial.println(szTemp);
       }
       delay(1000); // One sample per second is typical of temperature sensors
   }
} /* loop() */
