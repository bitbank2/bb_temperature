//
// bb_scd41 I/O wrapper functions for Linux
// Copyright (c) 2025 BitBank Software, Inc.
// Written by Larry Bank (bitbank@pobox.com)
//
// SPDX-License-Identifier: Apache-2.0
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
//

// Adapt these functions to whatever target platform you're using
// and the rest of the code can remain unchanged
//
#ifndef __BB_TEMPERATURE_IO__ 
#define __BB_TEMPERATURE_IO__

static void delay(uint32_t u32)
{
        usleep(u32*1000);
} /* delay() */

// Initialize the I2C bus on Linux
static void I2CInit(BBI2C *pI2C, uint32_t iClock)
{
char filename[32];
int iChannel = pI2C->iSDA;

// Only try to initialize the handle if it hasn't already been done
    if (pI2C->file_i2c == -1) {
        sprintf(filename, "/dev/i2c-%d", iChannel);
        if ((pI2C->file_i2c = open(filename, O_RDWR)) < 0)
        {
                fprintf(stderr, "Failed to open the i2c bus\n");
        }
    }
} /* I2CInit() */
//
// Test if an I2C address responds
// returns 1 for success, 0 for failure
//
static uint8_t I2CTest(BBI2C *pI2C, uint8_t addr)
{
uint8_t response = 0;
    if (ioctl(pI2C->file_i2c, I2C_SLAVE, addr) >= 0) {
            // probe this address
        uint8_t ucTemp;
        if (read(pI2C->file_i2c, &ucTemp, 1) >= 0)
            response = 1;
    }
    return response;
} /* I2CTest() */
//
// Read n bytes from the given I2C address
//
static int I2CRead(BBI2C *pI2C, uint8_t iAddr, uint8_t *pData, int iLen)
{
int rc;
        ioctl(pI2C->file_i2c, I2C_SLAVE, iAddr);
        rc = read(pI2C->file_i2c, pData, iLen);
        return rc;
} /* I2CRead() */
#ifdef FUTURE
//
// Read n bytes from the given address, after setting the register number
//
static int I2CReadRegister(BBI2C *pI2C, uint8_t iAddr, uint8_t u8Register, uint8_t *pData, int iLen)
{
int rc;
        // Reading from an I2C device involves first writing the 8-bit register
        // followed by reading the data
        ioctl(pI2C->file_i2c, I2C_SLAVE, iAddr);
        rc = write(pI2C->file_i2c, &u8Register, 1); // write the register value
        if (rc == 1)
        {
                rc = read(pI2C->file_i2c, pData, iLen);
        }
        return rc;

} /* I2CReadRegister() */
#endif // FUTURE
//
// Write n bytes to the given address
//
static int I2CWrite(BBI2C *pI2C, uint8_t iAddr, uint8_t *pData, int iLen)
{
int rc;
        ioctl(pI2C->file_i2c, I2C_SLAVE, iAddr);
        rc = write(pI2C->file_i2c, pData, iLen);
        return rc;
} /* I2CWrite() */
#endif // __BB_TEMPERATURE_IO__ 
