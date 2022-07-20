/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
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
 * @file      driver_mifare_classic_basic.h
 * @brief     driver mifare classic basic header file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2022-06-30
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2022/06/30  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#ifndef DRIVER_MIFARE_CLASSIC_BASIC_H
#define DRIVER_MIFARE_CLASSIC_BASIC_H

#include "driver_mifare_classic_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup mifare_classic_example_driver mifare classic example driver function
 * @brief    mifare classic example driver modules
 * @ingroup  mifare_classic_driver
 * @{
 */

/**
 * @brief mifare classic basic example default definition
 */
#define MIFARE_CLASSIC_BASIC_DEFAULT_SEARCH_DELAY_MS        200        /**< 5Hz */

/**
 * @brief  basic example init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t mifare_classic_basic_init(void);

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t mifare_classic_basic_deinit(void);

/**
 * @brief      basic example search
 * @param[out] *type points to a type buffer
 * @param[out] *id points to an id buffer
 * @param[in]  timeout is the check times
 * @return     status code
 *             - 0 success
 *             - 1 timeout
 * @note       none
 */
uint8_t mifare_classic_basic_search(mifare_classic_type_t *type, uint8_t id[4], int32_t timeout);

/**
 * @brief      basic example read
 * @param[in]  block is the read block
 * @param[out] *data points to a data buffer
 * @param[in]  key_type is the authentication key type
 * @param[in]  *key points to a key buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t mifare_classic_basic_read(uint8_t block, uint8_t data[16],
                                  mifare_classic_authentication_key_t key_type, uint8_t key[6]);

/**
 * @brief     basic example write
 * @param[in] block is the written block
 * @param[in] *data points to a data buffer
 * @param[in] key_type is the authentication key type
 * @param[in] *key points to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t mifare_classic_basic_write(uint8_t block, uint8_t data[16],
                                   mifare_classic_authentication_key_t key_type, uint8_t key[6]);

/**
 * @brief     basic example init as a value
 * @param[in] block is the set block
 * @param[in] value is the inited value
 * @param[in] addr is the inited address
 * @param[in] key_type is the authentication key type
 * @param[in] *key points to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 value init failed
 * @note      none
 */
uint8_t mifare_classic_basic_value_init(uint8_t block, int32_t value, uint8_t addr,
                                        mifare_classic_authentication_key_t key_type, uint8_t key[6]);

/**
 * @brief     basic example write value
 * @param[in] block is the written block
 * @param[in] value is the written value
 * @param[in] addr is the written address
 * @param[in] key_type is the authentication key type
 * @param[in] *key points to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 value written failed
 * @note      none
 */
uint8_t mifare_classic_basic_value_write(uint8_t block, int32_t value, uint8_t addr,
                                         mifare_classic_authentication_key_t key_type, uint8_t key[6]);

/**
 * @brief      basic example read value
 * @param[in]  block is the read block
 * @param[out] *value points to a read value buffer
 * @param[out] *addr points to a read address buffer
 * @param[in]  key_type is the authentication key type
 * @param[in]  *key points to a key buffer
 * @return     status code
 *             - 0 success
 *             - 1 value read failed
 * @note       none
 */
uint8_t mifare_classic_basic_value_read(uint8_t block, int32_t *value, uint8_t *addr,
                                         mifare_classic_authentication_key_t key_type, uint8_t key[6]);

/**
 * @brief     basic example decrement value
 * @param[in] block is the decrement block
 * @param[in] value is the decrement value
 * @param[in] key_type is the authentication key type
 * @param[in] *key points to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 value decrement failed
 * @note      none
 */
uint8_t mifare_classic_basic_value_decrement(uint8_t block, uint32_t value,
                                             mifare_classic_authentication_key_t key_type, uint8_t key[6]);

/**
 * @brief     basic example increment value
 * @param[in] block is the increment block
 * @param[in] value is the increment value
 * @param[in] key_type is the authentication key type
 * @param[in] *key points to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 value increment failed
 * @note      none
 */
uint8_t mifare_classic_basic_value_increment(uint8_t block, uint32_t value,
                                             mifare_classic_authentication_key_t key_type, uint8_t key[6]);

/**
 * @brief  basic example halt
 * @return status code
 *         - 0 success
 *         - 1 halt failed
 * @note   none
 */
uint8_t mifare_classic_basic_halt(void);

/**
 * @brief  basic example wake up
 * @return status code
 *         - 0 success
 *         - 1 wake up failed
 * @note   none
 */
uint8_t mifare_classic_basic_wake_up(void);

/**
 * @brief     basic example set the sector permission
 * @param[in] key_type is the authentication key type
 * @param[in] *key points to a key buffer
 * @param[in] sector is the set sector
 * @param[in] *key_a points to a key a buffer
 * @param[in] block_0_0_4 is the block0(block0-4) permission
 * @param[in] block_1_5_9 is the block1(block5-9) permission
 * @param[in] block_2_10_14 is the block2(block10-14) permission
 * @param[in] block_3_15 is the block3(block15) permission
 * @param[in] user_data is the user data
 * @param[in] *key_b points to a key b buffer
 * @return    status code
 *            - 0 success
 *            - 1 set sector permission failed
 * @note      block_0_0_4, block_1_5_9, block_2_10_14, permission(c1_c2_c3) definition is below
 *            c1  c2  c3        read        write        increment        decrement/transfer/restore
 *            0   0   0        keya|b       keya|b         keya|b                  keya|b
 *            0   1   0        keya|b       never          never                   never
 *            1   0   0        keya|b       keyb           never                   never
 *            1   1   0        keya|b       keyb           keyb                    keya|b
 *            0   0   1        keya|b       never          never                   keya|b
 *            0   1   1        keyb         keyb           never                   never
 *            1   0   1        keyb         never          never                   never
 *            1   1   1        never        never          never                   never
 *                                                                                      
 *            block_3_15 permission(c1_c2_c3) definition is below
 *            c1  c2  c3    keya_read    keya_write    access_read    access_write    keyb_read    keyb_write
 *            0   0   0      never        keya            keya           never          keya          keya
 *            0   1   0      never        never           keya           never          keya          never
 *            1   0   0      never        keyb           keya|b          never          never         keyb
 *            1   1   0      never        never          keya|b          never          never         never
 *            0   0   1      never        keya            keya           keya           keya          keya
 *            0   1   1      never        keyb           keya|b          keyb           never         keyb
 *            1   0   1      never        never          keya|b          keyb           never         never
 *            1   1   1      never        never          keya|b          never          never         never
 */
uint8_t mifare_classic_basic_set_permission(mifare_classic_authentication_key_t key_type, uint8_t key[6],
                                            uint8_t sector, uint8_t key_a[6], uint8_t block_0_0_4, uint8_t block_1_5_9,
                                            uint8_t block_2_10_14, uint8_t block_3_15, uint8_t user_data, uint8_t key_b[6]);

/**
 * @brief      basic example get the sector permission
 * @param[in]  key_type is the authentication key type
 * @param[in]  *key points to a key buffer
 * @param[in]  sector is the get sector
 * @param[out] *block_0_0_4 points to a block0(block0-4) permission buffer
 * @param[out] *block_1_5_9 points to a block1(block5-9) permission buffer
 * @param[out] *block_2_10_14 points to a block2(block10-14) permission buffer
 * @param[out] *block_3_15 points to a block3(block15) permission buffer
 * @param[out] *user_data points to a user data buffer
 * @param[out] *key_b points to a key b buffer
 * @return     status code
 *             - 0 success
 *             - 1 get sector permission failed
 * @note       none
 */
uint8_t mifare_classic_basic_get_permission(mifare_classic_authentication_key_t key_type, uint8_t key[6],
                                            uint8_t sector, uint8_t *block_0_0_4, uint8_t *block_1_5_9,
                                            uint8_t *block_2_10_14, uint8_t *block_3_15, uint8_t *user_data, uint8_t key_b[6]);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
