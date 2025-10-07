//
// bb_temperature "find the bus" example
// This example shows how to find the supported sensor, no
// matter which I2C bus it's attached to. This allows complete portability
// of the code to different Linux SBCs without having to know such details.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <bb_temperature.h>

BBTemp bbt;

int main(int argc, char *argv[])
{
int i, iBus;
DIR *pDir;
struct dirent *pDE;
uint32_t u32Buses = 0; // available I2C bus numbers (0-31)

        printf("bb_temperature find the bus example\n");
        printf("Finds which I2C bus has the supported sensor, then\ninitializes, configures, and loops displaying the sensor values.\n");
	// I2C buses in Linux are defined as a file in the /dev directory
        pDir = opendir("/dev");
	if (!pDir) {
		printf("Error searching /dev directory; aborting.\n");
		return -1;
	}
	// Search all names in the /dev directory for those starting with i2c-
        while ((pDE = readdir(pDir)) != NULL) {
		if (memcmp(pDE->d_name, "i2c-", 4) == 0) { // found one!
                    iBus = atoi(&pDE->d_name[4]);
		    u32Buses |= (1 << iBus); // collect the bus numbers
		}
	}
	closedir(pDir);
	if (u32Buses == 0) { // Something went wrong; no I2C buses
	    printf("No I2C buses found!\n");
	    printf("Check your system configuration (e.g. raspi-config)\n");
	    printf("to ensure that I2C is enabled.\n");
	    return -1;
	}
	// Search each I2C bus for a supported proximited sensor
        for (iBus=0; iBus<32; iBus++) {
	    if (u32Buses & (1<<iBus)) { // a bus that we found in /dev
		printf("Searching i2c-%d...\n", iBus);
                i = bbt.init(iBus); // scan for supported sensors
                if (i == BBT_SUCCESS) {
                    printf("Found a device on i2c-%d!\n", iBus);
		    break;
		}
	    }
        } // for each possible bus
	if (iBus == 32) { // scanned all buses and didn't find anything
            printf("No supported device found\n");
	    printf("Your system may require sudo to access I2C.\n");
            return -1; // problem - quit
	}
        printf("Sensor detected and initialized\n");
	bbt.start(); // start the sensor with the default options
	while (1) {
                BBT_SAMPLE bbts;
		bbt.getSample(&bbts);
		printf("Temp: %d.%d\n", bbts.temperature/10, bbts.temperature % 10);
                printf("Humidity: %d%%\n", bbts.humidity);
		usleep(1000000); // one sample every second
	}
return 0;
} /* main() */
