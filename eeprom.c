/* BEGIN Header */
/**
 ******************************************************************************
 * \file            eeprom.c
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

/* Includes ------------------------------------------------------------------*/

#include "eeprom.h"
#include "eeprom_STM32.h"

/* Macros --------------------------------------------------------------------*/

#if !defined(EEPROM_PAGE0_ADDRESS) || !defined(EEPROM_PAGE0_NUM) || !defined(EEPROM_PAGE_SIZE) || !defined(EEPROM_VAR_NUM)
#error "EEPROM_PAGE0_ADDRESS, EEPROM_PAGE0_NUM and EEPROM_PAGE_SIZE must be defined!"
#endif

/* Calculation of Page 1 characteristics if not explicitly defined */
#ifndef EEPROM_PAGE1_OFFSET
#define EEPROM_PAGE1_OFFSET ((uint32_t)1)
#endif

#ifndef EEPROM_PAGE1_ADDRESS
#define EEPROM_PAGE1_ADDRESS ((uint32_t)EEPROM_PAGE0_ADDRESS + EEPROM_PAGE1_OFFSET * EEPROM_PAGE_SIZE)
#endif

#ifndef EEPROM_PAGE1_NUM
#define EEPROM_PAGE1_NUM (EEPROM_PAGE0_NUM + EEPROM_PAGE1_OFFSET)
#endif

/* Memory erase struct definition */

#ifdef FLASH_VOLTAGE_RANGE_3
#define FLASH_ERASE_SET_VOLTAGE() (pEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3)
#else
#define FLASH_ERASE_SET_VOLTAGE() /* No action */
#endif

#if EEPROM_ERASE == EEPROM_ERASE_PAGE_ADDRESS
#define EEPROM_PAGE0_ID EEPROM_PAGE0_ADDRESS
#define EEPROM_PAGE1_ID EEPROM_PAGE1_ADDRESS
#define FLASH_ERASE_INIT(address)                                                                                                                              \
    do {                                                                                                                                                       \
        pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;                                                                                                          \
        pEraseInit.PageAddress = address;                                                                                                                      \
        pEraseInit.NbPages = 1;                                                                                                                                \
        FLASH_ERASE_SET_VOLTAGE();                                                                                                                             \
    } while (0)
#elif EEPROM_ERASE == EEPROM_ERASE_PAGE_NUMBER
#define EEPROM_PAGE0_ID EEPROM_PAGE0_NUM
#define EEPROM_PAGE1_ID EEPROM_PAGE1_NUM
#define FLASH_ERASE_INIT(address)                                                                                                                              \
    do {                                                                                                                                                       \
        pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;                                                                                                          \
        pEraseInit.Page = address;                                                                                                                             \
        pEraseInit.NbPages = 1;                                                                                                                                \
        FLASH_ERASE_SET_VOLTAGE();                                                                                                                             \
    } while (0)
#else /* EEPROM_ERASE == EEPROM_ERASE_SECTOR_NUMBER */
#define EEPROM_PAGE0_ID EEPROM_PAGE0_NUM
#define EEPROM_PAGE1_ID EEPROM_PAGE1_NUM
#define FLASH_ERASE_INIT(address)                                                                                                                              \
    do {                                                                                                                                                       \
        pEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;                                                                                                        \
        pEraseInit.Sector = address;                                                                                                                           \
        pEraseInit.NbSectors = 1;                                                                                                                              \
        FLASH_ERASE_SET_VOLTAGE();                                                                                                                             \
    } while (0)
#endif

#define FLASH_READ(address)   (*(__IO uint16_t*)(address))
#define FLASH_READ32(address) (*(__IO uint32_t*)(address))

#define OP_READ_VALID_PAGE    ((uint8_t)0x00)
#define OP_WRITE_VALID_PAGE   ((uint8_t)0x01)

#define EEPROM_NO_VALID_PAGE  ((uint16_t)0x00AB)

#define EEPROM_PAGE_CLEARED   ((uint16_t)0xFFFF)
#define EEPROM_PAGE_ACTIVE    ((uint16_t)0x0000)
#define EEPROM_PAGE_RECEIVING ((uint16_t)0xEEEE)

/* Private functions ---------------------------------------------------------*/
static EEPROM_retStatus_t EEPROM_IsPageErased(uint32_t address) {
    uint32_t endAddress;
    uint16_t addressValue = 0x5555;

    /* Compute page end-address */
    endAddress = (uint32_t)(address + (EEPROM_PAGE_SIZE - 4U));
    /* Check each active page address starting from end */
    while (address <= endAddress) {
        /* Get the current location content to be compared with virtual address */
        addressValue = FLASH_READ(address);
        /* Compare the read address with the virtual address */
        if (addressValue != EEPROM_PAGE_CLEARED) {
            /* In case variable value is read, return error */
            return EEPROM_ERROR;
        }
        /* Next address location */
        address += 4;
    }
    return EEPROM_SUCCESS;
}

static EEPROM_retStatus_t EEPROM_Format(void) {
    HAL_StatusTypeDef flashStatus = HAL_OK;
    uint32_t eraseError = 0;
    FLASH_EraseInitTypeDef pEraseInit;

    FLASH_ERASE_INIT(EEPROM_PAGE0_ID);

    /* Erase Page0 */
    if (EEPROM_IsPageErased(EEPROM_PAGE0_ADDRESS) != EEPROM_SUCCESS) {
        HAL_FLASH_Unlock();
        flashStatus = HAL_FLASHEx_Erase(&pEraseInit, &eraseError);
        HAL_FLASH_Lock();
        /* If erase operation was failed, a Flash error code is returned */
        if (flashStatus != HAL_OK) {
            return EEPROM_ERROR;
        }
    }
    /* Set Page0 as valid page: Write EEPROM_PAGE_ACTIVE at Page0 base address */
    HAL_FLASH_Unlock();
    flashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, EEPROM_PAGE0_ADDRESS, EEPROM_PAGE_ACTIVE);
    HAL_FLASH_Lock();
    /* If program operation was failed, a Flash error code is returned */
    if (flashStatus != HAL_OK) {
        return EEPROM_ERROR;
    }

    FLASH_ERASE_INIT(EEPROM_PAGE1_ID);
    /* Erase Page1 */
    if (EEPROM_IsPageErased(EEPROM_PAGE1_ADDRESS) != EEPROM_SUCCESS) {
        HAL_FLASH_Unlock();
        flashStatus = HAL_FLASHEx_Erase(&pEraseInit, &eraseError);
        HAL_FLASH_Lock();
        /* If erase operation was failed, a Flash error code is returned */
        if (flashStatus != HAL_OK) {
            return EEPROM_ERROR;
        }
    }

    return EEPROM_SUCCESS;
}

static uint32_t EEPROM_FindValidPage(uint8_t Operation) {
    uint16_t pageStatus0 = 6, pageStatus1 = 6;

    /* Get page0 and page1 actual status */
    pageStatus0 = FLASH_READ(EEPROM_PAGE0_ADDRESS);
    pageStatus1 = FLASH_READ(EEPROM_PAGE1_ADDRESS);

    /* Write or read operation */
    switch (Operation) {
        case OP_WRITE_VALID_PAGE: /* ---- Write operation ---- */
            if (pageStatus1 == EEPROM_PAGE_ACTIVE) {
                /* Page0 receiving data */
                if (pageStatus0 == EEPROM_PAGE_RECEIVING) {
                    return EEPROM_PAGE0_ID; /* Page0 valid */
                } else {
                    return EEPROM_PAGE1_ID; /* Page1 valid */
                }
            } else if (pageStatus0 == EEPROM_PAGE_ACTIVE) {
                /* Page1 receiving data */
                if (pageStatus1 == EEPROM_PAGE_RECEIVING) {
                    return EEPROM_PAGE1_ID; /* Page1 valid */
                } else {
                    return EEPROM_PAGE0_ID; /* Page0 valid */
                }
            } else {
                return EEPROM_NO_VALID_PAGE; /* No valid Page */
            }

        case OP_READ_VALID_PAGE: /* ---- Read operation ---- */
            if (pageStatus0 == EEPROM_PAGE_ACTIVE) {
                return EEPROM_PAGE0_ID; /* Page0 valid */
            } else if (pageStatus1 == EEPROM_PAGE_ACTIVE) {
                return EEPROM_PAGE1_ID; /* Page1 valid */
            } else {
                return EEPROM_NO_VALID_PAGE; /* No valid Page */
            }

        default: return EEPROM_PAGE0_ID; /* Page0 valid */
    }
}

static EEPROM_retStatus_t EEPROM_VerifyPageAndWrite(uint16_t virtAddress, uint16_t data) {
    HAL_StatusTypeDef flashStatus = HAL_OK;
    uint32_t validPage = EEPROM_PAGE0_ID;
    uint32_t address = EEPROM_PAGE0_ADDRESS, endAddress = EEPROM_PAGE0_ADDRESS + EEPROM_PAGE_SIZE;

    /* Get valid Page for write operation */
    validPage = EEPROM_FindValidPage(OP_WRITE_VALID_PAGE);

    /* Check if there is no valid page */
    if (validPage == EEPROM_NO_VALID_PAGE) {
        return EEPROM_NO_VALID_PAGE;
    }

    /* Get the valid Page start Address */
    if (validPage == EEPROM_PAGE0_ID) {
        address = EEPROM_PAGE0_ADDRESS;
    } else if (validPage == EEPROM_PAGE1_ID) {
        address = EEPROM_PAGE1_ADDRESS;
    } else {
        return EEPROM_NO_VALID_PAGE;
    }

    /* Get the valid Page end Address */
    endAddress = (uint32_t)(address + (EEPROM_PAGE_SIZE - 4U));

    /* Check each active page address starting from beginning */
    while (address <= endAddress) {
        /* Verify if Address and Address+2 contents are 0xFFFFFFFF */
        if (FLASH_READ32(address) == 0xFFFFFFFF) {
            /* Set variable data */
            HAL_FLASH_Unlock();
            flashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, data);
            HAL_FLASH_Lock();
            /* If program operation was failed, a Flash error code is returned */
            if (flashStatus != HAL_OK) {
                return EEPROM_ERROR;
            }
            /* Set variable virtual address */
            HAL_FLASH_Unlock();
            flashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + 2, virtAddress);
            HAL_FLASH_Lock();
            /* Return program operation status */
            if (flashStatus != HAL_OK) {
                return EEPROM_ERROR;
            }
            return EEPROM_SUCCESS;
        } else {
            /* Next address location */
            address += 4;
        }
    }

    /* Return PAGE_FULL in case the valid page is full */
    return EEPROM_PAGE_FULL;
}

static EEPROM_retStatus_t EEPROM_PageTransfer(uint16_t virtAddress, uint16_t data) {
    HAL_StatusTypeDef flashStatus = HAL_OK;
    EEPROM_retStatus_t eepromStatus = EEPROM_SUCCESS, readStatus = EEPROM_SUCCESS;
    FLASH_EraseInitTypeDef pEraseInit;
    uint32_t eraseError = 0;
    uint16_t tmpData = 0, ii = 0;
    uint32_t newPageAddress = EEPROM_PAGE0_ADDRESS;
    uint32_t oldPageId = 0;
    uint32_t validPage = EEPROM_PAGE0_ID;

    /* Get active Page for read operation */
    validPage = EEPROM_FindValidPage(OP_READ_VALID_PAGE);

    if (validPage == EEPROM_PAGE1_ID) /* Page1 valid */
    {
        /* New page address where variable will be moved to */
        newPageAddress = EEPROM_PAGE0_ADDRESS;

        /* Old page ID where variable will be taken from */
        oldPageId = EEPROM_PAGE1_ID;
    } else if (validPage == EEPROM_PAGE0_ID) /* Page0 valid */
    {
        /* New page address  where variable will be moved to */
        newPageAddress = EEPROM_PAGE1_ADDRESS;

        /* Old page ID where variable will be taken from */
        oldPageId = EEPROM_PAGE0_ID;
    } else {
        return EEPROM_NO_VALID_PAGE; /* No valid Page */
    }

    /* Set the new Page status to EEPROM_PAGE_RECEIVING status */
    HAL_FLASH_Unlock();
    flashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, newPageAddress, EEPROM_PAGE_RECEIVING);
    HAL_FLASH_Lock();
    /* If program operation was failed, a Flash error code is returned */
    if (flashStatus != HAL_OK) {
        return EEPROM_ERROR;
    }

    /* Write the variable passed as parameter in the new active page */
    eepromStatus = EEPROM_VerifyPageAndWrite(virtAddress, data);
    /* If program operation was failed, a Flash error code is returned */
    if (eepromStatus != EEPROM_SUCCESS) {
        return eepromStatus;
    }

    /* Transfer process: transfer variables from old to the new active page */
    for (ii = 0; ii < EEPROM_VAR_NUM; ii++) {
        if (ii != virtAddress) /* Check each variable except the one passed as parameter */
        {
            /* Read the other last variable updates */
            readStatus = EEPROM_ReadVariable(ii, &tmpData);
            /* In case variable corresponding to the virtual address was found */
            if (readStatus == EEPROM_SUCCESS) {
                /* Transfer the variable to the new active page */
                eepromStatus = EEPROM_VerifyPageAndWrite(ii, tmpData);
                /* If program operation was failed, a Flash error code is returned */
                if (eepromStatus != EEPROM_SUCCESS) {
                    return eepromStatus;
                }
            }
        }
    }

    FLASH_ERASE_INIT(oldPageId);

    /* Erase the old Page: Set old Page status to ERASED status */
    HAL_FLASH_Unlock();
    flashStatus = HAL_FLASHEx_Erase(&pEraseInit, &eraseError);
    HAL_FLASH_Lock();
    /* If erase operation was failed, a Flash error code is returned */
    if (flashStatus != HAL_OK) {
        return EEPROM_ERROR;
    }

    /* Set new Page status to EEPROM_PAGE_ACTIVE status */
    HAL_FLASH_Unlock();
    flashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, newPageAddress, EEPROM_PAGE_ACTIVE);
    HAL_FLASH_Unlock();
    /* If program operation was failed, a Flash error code is returned */
    if (flashStatus != HAL_OK) {
        return EEPROM_ERROR;
    }

    /* Return last operation flash status */
    return EEPROM_SUCCESS;
}

/* Functions -----------------------------------------------------------------*/

EEPROM_retStatus_t EEPROM_Init(void) {
    uint16_t pageStatus0, pageStatus1;
    HAL_StatusTypeDef flashStatus = HAL_OK;
    EEPROM_retStatus_t eepromStatus = EEPROM_SUCCESS, readStatus = EEPROM_SUCCESS;
    FLASH_EraseInitTypeDef pEraseInit;
    uint32_t eraseError = 0;
    uint16_t tmpData = 0, ii = 0;
    int16_t x = -1;

#ifdef HAL_ICACHE_MODULE_ENABLED
    /* disabling ICACHE if enabled*/
    HAL_ICACHE_Disable();
#endif

    /* Get pages status */
    pageStatus0 = FLASH_READ(EEPROM_PAGE0_ADDRESS);
    pageStatus1 = FLASH_READ(EEPROM_PAGE1_ADDRESS);

    /* Set erase configuration */
    FLASH_ERASE_INIT(EEPROM_PAGE0_ID);

    /* Check for invalid header states and repair if necessary */
    switch (pageStatus0) {
        case EEPROM_PAGE_CLEARED:
            if (pageStatus1 == EEPROM_PAGE_ACTIVE) { /* Page0 erased, Page1 valid */
                /* Erase Page0 */
                if (EEPROM_IsPageErased(EEPROM_PAGE0_ADDRESS) != EEPROM_SUCCESS) {
                    HAL_FLASH_Unlock();
                    flashStatus = HAL_FLASHEx_Erase(&pEraseInit, &eraseError);
                    HAL_FLASH_Lock();
                }
            } else if (pageStatus1 == EEPROM_PAGE_RECEIVING) { /* Page0 erased, Page1 receive */
                /* Erase Page0 */
                if (EEPROM_IsPageErased(EEPROM_PAGE0_ADDRESS) != EEPROM_SUCCESS) {
                    HAL_FLASH_Unlock();
                    flashStatus = HAL_FLASHEx_Erase(&pEraseInit, &eraseError);
                    HAL_FLASH_Lock();
                }
                /* Mark Page1 as valid */
                if (flashStatus == HAL_OK) {
                    HAL_FLASH_Unlock();
                    flashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, EEPROM_PAGE1_ADDRESS, EEPROM_PAGE_ACTIVE);
                    HAL_FLASH_Lock();
                }
            } else { /* First EEPROM access (Page0&1 are erased) or invalid state -> format EEPROM */
                /* Erase both Page0 and Page1 and set Page0 as valid page */
                eepromStatus = EEPROM_Format();
            }
            break;

        case EEPROM_PAGE_RECEIVING:
            if (pageStatus1 == EEPROM_PAGE_ACTIVE) { /* Page0 receive, Page1 valid */
                /* Transfer data from Page1 to Page0 */
                for (ii = 0; ii < EEPROM_VAR_NUM; ii++) {
                    if (FLASH_READ(EEPROM_PAGE0_ADDRESS + 6) == ii) {
                        x = ii;
                    }
                    if (ii != x) {
                        /* Read the last variables' updates */
                        readStatus = EEPROM_ReadVariable(ii, &tmpData);
                        /* In case variable corresponding to the virtual address was found */
                        if (readStatus == EEPROM_SUCCESS) {
                            /* Transfer the variable to the Page0 */
                            eepromStatus = EEPROM_VerifyPageAndWrite(ii, tmpData);
                            /* If program operation was failed, an error is returned */
                            if (eepromStatus != EEPROM_SUCCESS) {
#ifdef HAL_ICACHE_MODULE_ENABLED
                                HAL_ICACHE_Enable();
#endif
                                return eepromStatus;
                            }
                        }
                    }
                }
                /* Mark Page0 as valid */
                HAL_FLASH_Unlock();
                flashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, EEPROM_PAGE0_ADDRESS, EEPROM_PAGE_ACTIVE);
                HAL_FLASH_Lock();
                FLASH_ERASE_INIT(EEPROM_PAGE1_ID);
                /* Erase Page1 */
                if ((flashStatus == HAL_OK) && (EEPROM_IsPageErased(EEPROM_PAGE1_ADDRESS)) != EEPROM_SUCCESS) {
                    HAL_FLASH_Unlock();
                    flashStatus = HAL_FLASHEx_Erase(&pEraseInit, &eraseError);
                    HAL_FLASH_Lock();
                }
            } else if (pageStatus1 == EEPROM_PAGE_CLEARED) { /* Page0 receive, Page1 erased */
                FLASH_ERASE_INIT(EEPROM_PAGE1_ID);
                /* Erase Page1 */
                if (EEPROM_IsPageErased(EEPROM_PAGE1_ADDRESS) != EEPROM_SUCCESS) {
                    HAL_FLASH_Unlock();
                    flashStatus = HAL_FLASHEx_Erase(&pEraseInit, &eraseError);
                    HAL_FLASH_Lock();
                }
                /* Mark Page0 as valid */
                if (flashStatus == HAL_OK) {
                    HAL_FLASH_Unlock();
                    flashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, EEPROM_PAGE0_ADDRESS, EEPROM_PAGE_ACTIVE);
                    HAL_FLASH_Lock();
                }
            } else { /* Invalid state -> format eeprom */
                /* Erase both Page0 and Page1 and set Page0 as valid page */
                eepromStatus = EEPROM_Format();
            }
            break;

        case EEPROM_PAGE_ACTIVE:
            if (pageStatus1 == EEPROM_PAGE_ACTIVE) { /* Invalid state -> format eeprom */
                /* Erase both Page0 and Page1 and set Page0 as valid page */
                eepromStatus = EEPROM_Format();
            } else if (pageStatus1 == EEPROM_PAGE_CLEARED) { /* Page0 valid, Page1 erased */
                FLASH_ERASE_INIT(EEPROM_PAGE1_ID);
                /* Erase Page1 */
                if (EEPROM_IsPageErased(EEPROM_PAGE1_ADDRESS) != EEPROM_SUCCESS) {
                    HAL_FLASH_Unlock();
                    flashStatus = HAL_FLASHEx_Erase(&pEraseInit, &eraseError);
                    HAL_FLASH_Lock();
                }
            } else { /* Page0 valid, Page1 receive */
                /* Transfer data from Page0 to Page1 */
                for (ii = 0; ii < EEPROM_VAR_NUM; ii++) {
                    if (FLASH_READ(EEPROM_PAGE1_ADDRESS + 6) == ii) {
                        x = ii;
                    }
                    if (ii != x) {
                        /* Read the last variables' updates */
                        readStatus = EEPROM_ReadVariable(ii, &tmpData);
                        /* In case variable corresponding to the virtual address was found */
                        if (readStatus == EEPROM_SUCCESS) {
                            /* Transfer the variable to the Page1 */
                            eepromStatus = EEPROM_VerifyPageAndWrite(ii, tmpData);
                            /* If program operation was failed, an error is returned */
                            if (eepromStatus != EEPROM_SUCCESS) {
#ifdef HAL_ICACHE_MODULE_ENABLED
                                HAL_ICACHE_Enable();
#endif
                                return eepromStatus;
                            }
                        }
                    }
                }
                eepromStatus = EEPROM_SUCCESS;
                /* Mark Page1 as valid */
                HAL_FLASH_Unlock();
                flashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, EEPROM_PAGE1_ADDRESS, EEPROM_PAGE_ACTIVE);
                HAL_FLASH_Lock();
                FLASH_ERASE_INIT(EEPROM_PAGE0_ID);
                /* Erase Page0 */
                if ((flashStatus == HAL_OK) && (EEPROM_IsPageErased(EEPROM_PAGE0_ADDRESS)) != EEPROM_SUCCESS) {
                    HAL_FLASH_Unlock();
                    flashStatus = HAL_FLASHEx_Erase(&pEraseInit, &eraseError);
                    HAL_FLASH_Lock();
                }
            }
            break;

        default: /* Any other state -> format eeprom */
            /* Erase both Page0 and Page1 and set Page0 as valid page */
            eepromStatus = EEPROM_Format();
            break;
    }

#ifdef HAL_ICACHE_MODULE_ENABLED
    HAL_ICACHE_Enable();
#endif
    return (((flashStatus != HAL_OK) || (eepromStatus != EEPROM_SUCCESS)) ? EEPROM_ERROR : EEPROM_SUCCESS);
}

EEPROM_retStatus_t EEPROM_ReadVariable(uint16_t virtAddress, uint16_t* value) {
    uint32_t validPage = EEPROM_PAGE0_ID;
    uint16_t addressValue = 0x5555;
    uint32_t address = EEPROM_PAGE0_ADDRESS, startAddress = EEPROM_PAGE0_ADDRESS;

    if (virtAddress >= EEPROM_VAR_NUM) {
        return EEPROM_ERROR;
    }

#ifdef HAL_ICACHE_MODULE_ENABLED
    /* disabling ICACHE if enabled*/
    HAL_ICACHE_Disable();
#endif

    /* Get active Page for read operation */
    validPage = EEPROM_FindValidPage(OP_READ_VALID_PAGE);

    /* Check if there is no valid page */
    if (validPage == EEPROM_NO_VALID_PAGE) {
#ifdef HAL_ICACHE_MODULE_ENABLED
        HAL_ICACHE_Enable();
#endif
        return EEPROM_NO_VALID_PAGE;
    }

    /* Get the valid Page start Address */
    if (validPage == EEPROM_PAGE0_ID) {
        startAddress = EEPROM_PAGE0_ADDRESS;
    } else if (validPage == EEPROM_PAGE1_ID) {
        startAddress = EEPROM_PAGE1_ADDRESS;
    } else {
#ifdef HAL_ICACHE_MODULE_ENABLED
        HAL_ICACHE_Enable();
#endif
        return EEPROM_NO_VALID_PAGE;
    }

    /* Get the valid Page end Address */
    address = (uint32_t)(startAddress + (EEPROM_PAGE_SIZE - 2U));

    /* Check each active page address starting from end */
    while (address > (startAddress + 2)) {
        /* Get the current location content to be compared with virtual address */
        addressValue = FLASH_READ(address);

        /* Compare the read address with the virtual address */
        if (addressValue == virtAddress) {
            /* Get content of address-2 which is variable value */
            *value = FLASH_READ(address - 2U);

#ifdef HAL_ICACHE_MODULE_ENABLED
            HAL_ICACHE_Enable();
#endif
            return EEPROM_SUCCESS;

            break;
        } else {
            /* Next address location */
            address -= 4;
        }
    }

#ifdef HAL_ICACHE_MODULE_ENABLED
    HAL_ICACHE_Enable();
#endif

    return EEPROM_ERROR;
}

EEPROM_retStatus_t EEPROM_WriteVariable(uint16_t virtAddress, uint16_t value) {
    EEPROM_retStatus_t retStatus = EEPROM_SUCCESS;

    if (virtAddress >= EEPROM_VAR_NUM) {
        return EEPROM_ERROR;
    }

#ifdef HAL_ICACHE_MODULE_ENABLED
    /* disabling ICACHE if enabled*/
    HAL_ICACHE_Disable();
#endif

    /* Write the variable virtual address and value in the EEPROM */
    retStatus = EEPROM_VerifyPageAndWrite(virtAddress, value);

    /* In case the EEPROM active page is full */
    if (retStatus == EEPROM_PAGE_FULL) {
        /* Perform Page transfer */
        retStatus = EEPROM_PageTransfer(virtAddress, value);
    }

#ifdef HAL_ICACHE_MODULE_ENABLED
    HAL_ICACHE_Enable();
#endif

    /* Return last operation status */
    return retStatus;
}
