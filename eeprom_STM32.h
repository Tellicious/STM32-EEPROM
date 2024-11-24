/* BEGIN Header */
/**
 ******************************************************************************
 * \file            eeprom_STM32.h
 * \author          Andrea Vivani
 * \brief           Configuration parameters of different STM32 families
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
#ifndef __EEPROM_STM32_H__
#define __EEPROM_STM32_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Macros --------------------------------------------------------------------*/
#define EEPROM_ERASE_PAGE_ADDRESS  0
#define EEPROM_ERASE_PAGE_NUMBER   1
#define EEPROM_ERASE_SECTOR_NUMBER 2

#if defined(STM32F0)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_ADDRESS
#elif defined(STM32F1)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_ADDRESS
#elif defined(STM32F2)
#define EEPROM_ERASE EEPROM_ERASE_SECTOR_NUMBER
#elif defined(STM32F3)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_ADDRESS
#elif defined(STM32F4)
#define EEPROM_ERASE EEPROM_ERASE_SECTOR_NUMBER
#elif defined(STM32F7)
#define EEPROM_ERASE EEPROM_ERASE_SECTOR_NUMBER
#elif defined(STM32H5)
#define EEPROM_ERASE EEPROM_ERASE_SECTOR_NUMBER
#elif defined(STM32H7)
#define EEPROM_ERASE EEPROM_ERASE_SECTOR_NUMBER
#elif defined(STM32G0)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#elif defined(STM32G4)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#elif defined(STM32U0)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#elif defined(STM32U5)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#elif defined(STM32L0)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#elif defined(STM32L1)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#elif defined(STM32L4)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#elif defined(STM32L5)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#elif defined(STM32WB)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#elif defined(STM32WBA)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#undef FLASH_BANK_1
#elif defined(STM32WL)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#elif defined(STM32C0)
#define EEPROM_ERASE EEPROM_ERASE_PAGE_NUMBER
#endif

#ifndef EEPROM_ERASE
#error "Not Supported MCU!"
#endif
#ifdef __cplusplus
}
#endif

#endif /* __EEPROM_STM32_H__ */