#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <bb_temperature.h>

BBTemp bbt;
BBT_SAMPLE bbts;

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

        printf("bb_temperature basic example\n");
        printf("Initializes, configures, then loops displaying the sensor values.\n");
        i = bbt.init(SDA_PIN, SCL_PIN);
        if (i != BBT_SUCCESS) {
            printf("Supported sensor not found!\n");
            while (1) {
                vTaskDelay(1);
            }
        }
        printf("Found: %s\n", szDevices[bbt.type()]);
        bbt.start(); // start with default options
        while (1) {
                bbt.getSample(&bbts);
                printf("Temp: %d.%d\n", bbts.temperature/10, bbts.temperature % 10);
                printf("Humidity: %d%%\n", bbts.humidity);
                vTaskDelay(100); // 1 sample every second
        }
} /* app_main() */
