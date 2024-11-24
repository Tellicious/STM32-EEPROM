# STM32-EEPROM
 EEPROM Emulation library for STM32. It uses 2 different pages to limit flash wear.

### EEPROM Emulation Configuration
Configuration is perfomed via a file named `eepromConfig.h` (based on `eepromConfig_template.h`) that contains:
| Parameter              | Mandatory | Default                                                     | Description                                                                                                      |
| ---------------------- | --------- | ----------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------- |
| `STM32xx`              | yes       | -                                                           | Microcontroller family between the ones listed below                                                             |
| `EEPROM_PAGE0_ADDRESS` | yes       | -                                                           | Starting address of first page in flash                                                                          |
| `EEPROM_PAGE0_NUM`     | yes       | -                                                           | Page number of first page in flash                                                                               |
| `EEPROM__PAGE_SIZE`    | yes       | -                                                           | Size of flash pages used. It must be the same between Page 0 and Page 1                                          |
| `EEPROM_VAR_NUM`       | yes       | -                                                           | Number of variables of type `uint16_t` to be stored in EEPROM. Variable locations are 0-indexed                  |
| `EEPROM_PAGE1_OFFSET`  | no        | 1                                                           | Number of pages between Page 0 and Page 1, such that `EEPROM_PAGE1_NUM = EEPROM_PAGE0_NUM + EEPROM_PAGE1_OFFSET` |
| `EEPROM_PAGE1_ADDRESS` | no        | `EEPROM_PAGE1_NUM = EEPROM_PAGE0_NUM + EEPROM_PAGE1_OFFSET` | Starting address of second page in flash                                                                         |
| `EEPROM_PAGE1_NUM`     | no        | `EEPROM_PAGE0_NUM + EEPROM_PAGE1_OFFSET`                    | Page number of second page in flash                                                                              |

### List of available microcontroller families 
| STM32 Family |
| ------------ |
| STM32F0      |
| STM32F1      |
| STM32F2      |
| STM32F3      |
| STM32F4      |
| STM32F7      |
| STM32H5      |
| STM32H7      |
| STM32G0      |
| STM32G4      |
| STM32U0      |
| STM32U5      |
| STM32L0      |
| STM32L1      |
| STM32L4      |
| STM32L5      |
| STM32WB      |
| STM32WBA     |
| STM32WL      |
| STM32C0      |