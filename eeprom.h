/* BEGIN Header */
/**
 ******************************************************************************
 * \file            eeprom.h
 * \author          Andrea Vivani
 * \brief           STM32 EEPROM emulation driver
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
#ifndef __EEPROM_H__
#define __EEPROM_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "eepromConfig.h"
#include "main.h"

/* Macros --------------------------------------------------------------------*/

/* Typedefs ------------------------------------------------------------------*/
/*
* EEPROM return status
*/
typedef enum { EEPROM_SUCCESS = 0, EEPROM_ERROR = 1, EEPROM_PAGE_FULL = 2, EEPROM_NO_VALID_PAGE = 3 } EEPROM_retStatus_t;

/* Function prototypes -------------------------------------------------------*/

/**
 * \brief           Initialize EEPROM emulation
 *
 * \return          EEPROM_SUCCESS if initialization is successful, EEPROM_ERROR otherwise
 */
EEPROM_retStatus_t EEPROM_Init(void);

/**
 * \brief           Read variable from EEPROM emulation
 *
 * \param[in]       virtAddress: virtual address of data to be read
 * \param[out]      value: pointer to output value
 *
 * \return          EEPROM_SUCCESS if read was successful, EEPROM_NO_VALID_PAGE if no valid page was found, EEPROM_ERROR otherwise
 */
EEPROM_retStatus_t EEPROM_ReadVariable(uint16_t virtAddress, uint16_t* value);

/**
 * \brief           Write variable to EEPROM emulation
 *
 * \param[in]       virtAddress: virtual address of data to be written
 * \param[in]       value: value to be written
 *
 * \return          EEPROM_SUCCESS if write was successful, EEPROM_NO_VALID_PAGE if no valid page was found, EEPROM_ERROR otherwise
 */
EEPROM_retStatus_t EEPROM_WriteVariable(uint16_t virtAddress, uint16_t value);

#ifdef __cplusplus
}
#endif

#endif /* __EEPROM_H__ */