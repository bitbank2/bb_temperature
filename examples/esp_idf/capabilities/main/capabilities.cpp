//
// bb_temperature
// Capabilities example program
// 
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <bb_temperature.h>

BBTemp bbt;
BBT_SAMPLE bbts;
uint32_t u32Caps;
const char *szCaps[] = {"Temperature", "Humidity", "Pressure"};
const char *szDevices[] = {"Not found", "AHT20", "BMP180", "BME280", "BMP388", "SHT3X", "HDC1080", "HTS221", "MCP9808"};

extern "C" {
void app_main(void);
}

// I2C Bus on Xiao ESP32C3
#define SDA_PIN 6
#define SCL_PIN 7

void app_main(void)
{
int i;

        printf("bb_temperature capabilitiesc example\n");
        printf("Initializes, configures, then loops displaying the sensor values.\n");
        i = bbt.init(SDA_PIN, SCL_PIN);
        if (i != BBT_SUCCESS) {
            printf("Supported sensor not found!\n");
            while (1) {
                vTaskDelay(1);
            }
        }
        u32Caps = bbt.caps();
        printf("Found: %s\n", szDevices[bbt.type()]);
        printf("Capabilities: ");
        for (int i=0; i<3; i++) {
            if (u32Caps & (1<<i)) {
                printf(szCaps[i]);
                printf(", ");
            }
        } // for i
        printf("\n");
        bbt.start(); // start with default options
        while (1) {
                bbt.getSample(&bbts);
                if (u32Caps & BBT_CAP_TEMPERATURE) {
                    printf("Temp: %d.%d\n", bbts.temperature/10, bbts.temperature % 10);
                }
                if (u32Caps & BBT_CAP_HUMIDITY) {
                    printf("Humidity: %d%%\n", bbts.humidity);
                }
                if (u32Caps & BBT_CAP_PRESSURE) {
                    printf("Pressure: %d hPa\n", bbts.pressure);
                }
                vTaskDelay(100); // 1 sample every second
        } // while (1)
} /* app_main() */
