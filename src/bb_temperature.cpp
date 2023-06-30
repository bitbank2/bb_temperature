//
// BitBank Temperature Sensor Library
// Written by Larry Bank
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
#include "bb_temperature.h"
//
// Tell the sensor to start sampling
//
int BBTemp::start(void)
{
   return BBT_SUCCESS;
} /* start() */

//
// Tell the sensor to stop sampling
//
void BBTemp::stop(void)
{
} /* stop() */

//
// Initialize the library
// It only needs to initialize the I2C interface; the chip is ready
//
int BBTemp::init(int iSDA, int iSCL, bool bBitBang, uint32_t u32Speed)
{
uint8_t ucTemp[4];
int rc, iOff;

    _iType = -1;
    _bbi2c.bWire = !bBitBang; // use bit bang?
    _bbi2c.iSDA = iSDA;
    _bbi2c.iSCL = iSCL;
    I2CInit(&_bbi2c, u32Speed);

    for (iOff=0; iOff<8; iOff++) { // I2C address permutations
        if (I2CTest(&_bbi2c, BBT_ADDR_AHT20+iOff)) { // check for AHT20
           rc = I2CReadRegister(&_bbi2c, BBT_ADDR_AHT20+iOff, AHT20_REG_STATUS, ucTemp, 1);
           if (rc && ((ucTemp[0] & 0x7f) == 0x18 || (ucTemp[0] & 0x7f) == 0x1c)) { // yes
               _iAddr = BBT_ADDR_AHT20+iOff;
               _iType = BBT_TYPE_AHT20;
               _u32Caps = BBT_CAP_TEMPERATURE | BBT_CAP_HUMIDITY;
               return BBT_SUCCESS;
           }
        } // if AHT20

        if (I2CTest(&_bbi2c, BBT_ADDR_BME280+iOff)) { // check for Bosch BME280
           rc = I2CReadRegister(&_bbi2c, BBT_ADDR_BME280+iOff, BME280_REG_WHOAMI, ucTemp, 1);
           if (rc && ucTemp[0] == BME280_DEVICE_ID) { // yes
               _iAddr = BBT_ADDR_BME280+iOff;
               _iType = BBT_TYPE_BME280;
               _u32Caps = BBT_CAP_TEMPERATURE | BBT_CAP_HUMIDITY | BBT_CAP_PRESSURE;
               return BBT_SUCCESS;
           }
        } // if BME280

        if (I2CTest(&_bbi2c, BBT_ADDR_HDC1080+iOff)) {
           rc = I2CReadRegister(&_bbi2c, BBT_ADDR_HDC1080+iOff, HDC_REG_DEVICEID, ucTemp, 2);
           if (rc && ucTemp[0] == (HDC_VAL_DEVICEID >> 8) && ucTemp[1] == (HDC_VAL_DEVICEID & 0xff)) { // yes
               _iAddr = BBT_ADDR_HDC1080+iOff;
               _iType = BBT_TYPE_HDC1080;
               _u32Caps = BBT_CAP_TEMPERATURE | BBT_CAP_HUMIDITY;
               return BBT_SUCCESS;
           }
        } // if HDC1080

    } // for each address permutation
    return BBT_ERROR;
} /* init() */

int BBTemp::type(void)
{
   return _iType;
} /* type() */

uint32_t BBTemp::caps(void)
{
   return _u32Caps;
} /* caps() */

int BBTemp::getSample(BBT_SAMPLE *pBS)
{

   return BBT_SUCCESS;
} /* getSample() */
