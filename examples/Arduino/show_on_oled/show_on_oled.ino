//
// Example sketch for bb_temperature
// written by Larry Bank
// bitbank@pobox.com
//
// This example shows how to use a shared I2C bus with the supported sensor and a SSD1306 OLED display
// The sensor type will be displayed along with the temperature and humidity values (if initialization succeeds)
// For this example, I'm use the Xiao ESP32C3 which has predefined I2C pins (SDA/SCL) in the Arduino variant info
// This allows for both OneBitDisplay and bb_temperature to not hard code the GPIO pin numbers. This code will run
// unchanged on any Arduino supported board which defines a default I2C bus
// 
#include <OneBitDisplay.h>
#include <bb_temperature.h>
ONE_BIT_DISPLAY obd;
BBTemp bbt;

const char *szDevices[] = {"Not found", "AHT20", "BMP180", "BME280", "BMP388", "SHT3X", "HDC1080", "HTS221", "MCP9808"};
BBT_SAMPLE bbts;
char szTemp[32];

void setup() {
  int rc;

// Initialize the SSD1306 OLED - change the type if needed)
  obd.I2Cbegin(OLED_128x64);
  obd.fillScreen(OBD_WHITE);
  obd.setFont(FONT_12x16);
  obd.println("bb_temp");
  obd.setFont(FONT_8x8);
  // Pass the I2C setup from OneBitDisplay to initialize bb_temperature
  // This is the easiest way to share the I2C bus on every platform
  // It is required on Linux since the I2C handle is exclusive and cannot be reused until
  // the original owner closes it.
  rc = bbt.init(obd.getBB());
  if (rc == BBT_SUCCESS) {
    sprintf(szTemp, "Found: %s", szDevices[bbt.type()]);
    obd.println(szTemp);
  } else {
    obd.println("Failed");
    while (1) {};
  }
  bbt.start();
}

void loop() {

   while (1) { // loop forever
       bbt.getSample(&bbts); // Read the latest sensor value(s) into our structure
       sprintf(szTemp, "Temp: %d.%dC", bbts.temperature / 10, bbts.temperature % 10);
       obd.setCursor(0, 32);
       obd.print(szTemp);
       sprintf(szTemp, "Humidity: %d%%", bbts.humidity);
       obd.setCursor(0, 40);
       obd.print(szTemp);
       delay(1000); // One sample per second is typical of temperature sensors
   }
}

