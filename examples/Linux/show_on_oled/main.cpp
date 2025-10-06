//
// bb_temperature "show on OLED" example
// This example shows how to find the supported sensor, no
// matter which I2C bus it's attached to, and share that same I2C bus
// with another BitBank library (OneBitDisplay)
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <bb_temperature.h>
#include <OneBitDisplay.h>

ONE_BIT_DISPLAY obd;
BBTemp bbt;
BBT_SAMPLE bbts;
const char *szDevices[] = {"Not found", "AHT20", "BMP180", "BME280", "BMP388", "SHT3X", "HDC1080", "HTS221", "MCP9808"};

int main(int argc, char *argv[])
{
int i;

        printf("bb_temperature shared bus example\n");
        printf("Initializes, configures, and loops displaying the sensor values.\n");
	i = bbt.init(4); // I2C bus number
	if (i == BBT_SUCCESS) {
            printf("Found: %s\n", szDevices[bbt.type()]);
	} else {
	    printf("Sensor not found; check your connections\n");
	    return -1;
	}
	// Pass the I2C handle to OneBitDisplay to share for the OLED display
	obd.setBB(bbt.getBB());
	obd.I2Cbegin(OLED_128x64, 0x3c);
	obd.fillScreen(OBD_WHITE); // color is relative to B/W LCDs
	obd.setFont(FONT_12x16);
	obd.drawString("BitBank", 0, 0);
	obd.drawString("bb_temp", 0, 16);
        obd.setFont(FONT_8x8);
	bbt.start(); // start reading sample values
	while (1) {
		char szTemp[64];
		bbt.getSample(&bbts);
		sprintf(szTemp, "Temp: %d.%dC  ", bbts.temperature/10, bbts.temperature % 10);
		obd.drawString(szTemp, 0, 48);
		sprintf(szTemp, "Humidity: %d%% ", bbts.humidity);
		obd.drawString(szTemp, 0, 56);
		usleep(1000000); // show a new sample every second
	}
return 0;
} /* main() */
