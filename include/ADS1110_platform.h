/**
 **********************************************************************************
 * @file   ADS1110_platform.h
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  ADS1110 ADC driver platform dependent part
 **********************************************************************************
 *
 * Copyright (c) 2023 Mahda Embedded System (MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************
 */

/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef	_ADS1110_PLATFORM_H_
#define _ADS1110_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include "ADS1110.h"


/* Functionality Options --------------------------------------------------------*/
#define ADS1110_I2C_NUM   I2C_NUM_0
#define ADS1110_I2C_RATE  100000
#define ADS1110_SCL_GPIO  GPIO_NUM_7
#define ADS1110_SDA_GPIO  GPIO_NUM_5



/**
 ==================================================================================
                             ##### Functions #####                                 
 ==================================================================================
 */

/**
 * @brief  Initialize platform device to communicate ADS1110.
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
ADS1110_Platform_Init(ADS1110_Handler_t *Handler);


#ifdef __cplusplus
}
#endif


#endif //! _ADS1110_PLATFORM_H_
