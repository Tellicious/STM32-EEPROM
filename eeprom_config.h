/* BEGIN Header */
/**
 ******************************************************************************
 * \file            eeprom_config_template.h
 * \author          Andrea Vivani
 * \brief           Template for the eeprom_config.h file
 ******************************************************************************
 * \copyright
 *
 * Copyright 2024 Andrea Vivani
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 ******************************************************************************
 */
/* END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_CONFIG_H__
#define __EEPROM_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Mandatory parameters ------------------------------------------------------*/
/* Type of microcontroller */
#define STM32F4
/* Address of Page 0 in memory */
#define PAGE0_ADDRESS  0x08004000
/* Number of Page 0 in memory */
#define PAGE0_NUM      0
/* Size of each of the two pages used */
#define PAGE_SIZE      0x4000 //16K
/* Offset between Page 0 and Page 1: if PAGE1_NUM is not defined, PAGE1_NUM = PAGE0_NUM + PAGE1_OFFSET */
#define PAGE1_OFFSET   1
/* Number of variables stored in EEPROM */
#define EEPROM_VAR_NUM 2

/* Optional parameters -------------------------------------------------------*/
/* Address of Page 1 in memory */
//#define PAGE1_ADDRESS (0x08004000 + 0x4000)
/* Number of Page 1 in memory */
//#define PAGE1_NUM     1

#ifdef __cplusplus
}
#endif

#endif /* __EEPROM_CONFIG_H__ */