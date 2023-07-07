//
// BitBank Capacitive Touch Sensor Library
// written by Larry Bank
//
// Copyright 2023 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================

#include <Arduino.h>
#include <BitBang_I2C.h>

#ifndef __BB_TEMP__
#define __BB_TEMP__

#define BBT_SUCCESS 0
#define BBT_NOT_READY 1
#define BBT_ERROR 2

#define BBT_CAP_TEMPERATURE 1
#define BBT_CAP_HUMIDITY 2
#define BBT_CAP_PRESSURE 4

enum {
  BBT_TYPE_UNKNOWN = 0,
  BBT_TYPE_AHT20,
  BBT_TYPE_BMP180,
  BBT_TYPE_BME280,
  BBT_TYPE_SHT3X,
  BBT_TYPE_HDC1080,
  BBT_TYPE_HTS221,
  BBT_TYPE_MCP9808,
  BBT_TYPE_COUNT
};

#define BBT_ADDR_MCP9808 0x18
#define BBT_ADDR_AHT20 0x38
#define BBT_ADDR_HDC1080 0x40
#define BBT_ADDR_HTS221 0x5f
#define BBT_ADDR_BME280 0x76
#define BBT_ADDR_BME680 0x76
#define BBT_ADDR_SHT3X 0x44

#define BME280_REG_WHOAMI       0xd0
#define BME280_REG_CTRL_HUM     0xf2
#define BME280_REG_CTRL_STATUS  0xf3
#define BME280_REG_CTRL_MEAS    0xf4
#define BME280_REG_CONFIG       0xf5
// WHOAMI register value for the BME280
#define BME280_DEVICE_ID        0x60
enum {
    BME280_SLEEP_MODE = 0,
    BME280_FORCED_MODE,
    BME280_FORCED_MODE2,
    BME280_NORMAL_MODE
};

enum {
    BME280_OVERSAMPLE1=1,
    BME280_OVERSAMPLE2,
    BME280_OVERSAMPLE4,
    BME280_OVERSAMPLE8,
    BME280_OVERSAMPLE16
};

#define AHT20_REG_STATUS 0x71
#define AHT20_REG_RESET 0xBA
#define AHT20_REG_INIT 0xBE
#define AHT20_REG_MEASURE 0xAC

// AHT20 status bits
#define AHT20_CALIBRATED 0x08
#define AHT20_BUSY 0x80

#define HDC_REG_TEMPERATURE 0x00
#define HDC_REG_HUMIDITY    0x01
#define HDC_REG_CONFIG      0x02
#define HDC_REG_DEVICEID    0xff
#define HDC_VAL_DEVICEID    0x1050

#define SHT3X_MEAS_HIGHREP_STRETCH 0x2C06
#define SHT3X_MEAS_MEDREP_STRETCH  0x2C0D
#define SHT3X_MEAS_LOWREP_STRETCH  0x2C10
#define SHT3X_MEAS_HIGHREP         0x2400
#define SHT3X_MEAS_MEDREP          0x240B
#define SHT3X_MEAS_LOWREP          0x2416
#define SHT3X_READSTATUS           0xF32D
#define SHT3X_CLEARSTATUS          0x3041
#define SHT3X_SOFTRESET            0x30A2
#define SHT3X_HEATEREN             0x306D
#define SHT3X_HEATERDIS            0x3066

#define HTS221_WHO_AM_I_REG         0x0f
#define HTS221_WHO_AM_I_VAL         0xbc
#define HTS221_CTRL1_REG            0x20
#define HTS221_CTRL2_REG            0x21
#define HTS221_STATUS_REG           0x27
#define HTS221_HUMIDITY_OUT_L_REG   0x28
#define HTS221_TEMP_OUT_L_REG       0x2a
#define HTS221_H0_rH_x2_REG         0x30
#define HTS221_H1_rH_x2_REG         0x31
#define HTS221_T0_degC_x8_REG       0x32
#define HTS221_T1_degC_x8_REG       0x33
#define HTS221_T1_T0_MSB_REG        0x35
#define HTS221_H0_T0_OUT_REG        0x36
#define HTS221_H1_T0_OUT_REG        0x3a
#define HTS221_T0_OUT_REG           0x3c
#define HTS221_T1_OUT_REG           0x3e

typedef struct _tagbbtsample
{
   int temperature; // Temp in C * 10
   int humidity; // humidity in percent (0-100)
   int pressure; // pressure in hpa
} BBT_SAMPLE;

class BBTemp
{
public:
    BBTemp() { _iType = BBT_TYPE_UNKNOWN; _u32Caps = 0; }
    ~BBTemp() {}
    int type(void);
    uint32_t caps(void);
    int init(int iSDA = -1, int iSCL = -1, bool bBitBang = false, uint32_t u32Speed=400000);
    int start(void);
    void stop(void);
    int getSample(BBT_SAMPLE *pBS);
 
private:
    int _iAddr;
    int _iType;
    uint32_t _u32Caps;
    BBI2C _bbi2c;
    int32_t _calT1, _calT2, _calT3; // calibration data
    int32_t _calP1, _calP2, _calP3, _calP4, _calP5, _calP6, _calP7, _calP8, _calP9;
    int32_t _calH1, _calH2, _calH3, _calH4, _calH5, _calH6;
    float _hts221HumiditySlope;
    float _hts221HumidityZero;
    float _hts221TemperatureSlope;
    float _hts221TemperatureZero;
    void readMultiple(int iRegister, uint8_t *pData, int iCount);

}; // class BBTemp
#endif // __BB_TEMP__
