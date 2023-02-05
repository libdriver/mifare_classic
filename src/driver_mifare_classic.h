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
 * @file      driver_mifare_classic.h
 * @brief     driver mifare classic header file
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

#ifndef DRIVER_MIFARE_CLASSIC_H
#define DRIVER_MIFARE_CLASSIC_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup mifare_classic_driver mifare classic driver function
 * @brief    mifare classic driver modules
 * @{
 */

/**
 * @addtogroup mifare_classic_basic_driver
 * @{
 */

/**
 * @brief mifare_classic type enumeration definition
 */
typedef enum
{
    MIFARE_CLASSIC_TYPE_INVALID = 0x00,        /**< invalid */
    MIFARE_CLASSIC_TYPE_S50     = 0x01,        /**< s50 */
    MIFARE_CLASSIC_TYPE_S70     = 0x02,        /**< s70 */
} mifare_classic_type_t;

/**
 * @brief mifare_classic load modulation enumeration definition
 */
typedef enum
{
    MIFARE_CLASSIC_LOAD_MODULATION_NORMAL = 0x00,        /**< normal load modulation */
    MIFARE_CLASSIC_LOAD_MODULATION_STRONG = 0x01,        /**< strong load modulation */
} mifare_classic_load_modulation_t;

/**
 * @brief mifare_classic personalized uid enumeration definition
 */
typedef enum
{
    MIFARE_CLASSIC_PERSONALIZED_UID_0 = 0x00,        /**< anti-collision and selection with the double size UID according to ISO/IEC
                                                          14443-3 */
    MIFARE_CLASSIC_PERSONALIZED_UID_1 = 0x40,        /**< anti-collision and selection with the double size UID according to ISO/IEC
                                                          14443-3 and optional usage of a selection process shortcut */
    MIFARE_CLASSIC_PERSONALIZED_UID_2 = 0x20,        /**< anti-collision and selection with a single size random ID according to ISO/IEC
                                                          14443-3 */
    MIFARE_CLASSIC_PERSONALIZED_UID_3 = 0x60,        /**< anti-collision and selection with a single size NUID according to ISO/IEC
                                                          14443-3 where the NUID is calculated out of the 7-byte UID */
} mifare_classic_personalized_uid_t;

/**
 * @brief mifare_classic authentication key enumeration definition
 */
typedef enum
{
    MIFARE_CLASSIC_AUTHENTICATION_KEY_A = 0x00,        /**< authentication key a */
    MIFARE_CLASSIC_AUTHENTICATION_KEY_B = 0x01,        /**< authentication key b */
} mifare_classic_authentication_key_t;

/**
 * @brief mifare_classic handle structure definition
 */
typedef struct mifare_classic_handle_s
{
    uint8_t (*contactless_init)(void);                                             /**< point to a contactless_init function address */
    uint8_t (*contactless_deinit)(void);                                           /**< point to a contactless_deinit function address */
    uint8_t (*contactless_transceiver)(uint8_t *in_buf, uint8_t in_len, 
                                       uint8_t *out_buf, uint8_t *out_len);        /**< point to a contactless_transceiver function address */
    void (*delay_ms)(uint32_t ms);                                                 /**< point to a delay_ms function address */
    void (*debug_print)(const char *const fmt, ...);                               /**< point to a debug_print function address */
    uint8_t type;                                                                  /**< classic type */
    uint8_t inited;                                                                /**< inited flag */
} mifare_classic_handle_t;

/**
 * @brief mifare_classic information structure definition
 */
typedef struct mifare_classic_info_s
{
    char chip_name[32];                /**< chip name */
    char manufacturer_name[32];        /**< manufacturer name */
    char interface[8];                 /**< chip interface name */
    float supply_voltage_min_v;        /**< chip min supply voltage */
    float supply_voltage_max_v;        /**< chip max supply voltage */
    float max_current_ma;              /**< chip max current */
    float temperature_min;             /**< chip min operating temperature */
    float temperature_max;             /**< chip max operating temperature */
    uint32_t driver_version;           /**< driver version */
} mifare_classic_info_t;

/**
 * @}
 */

/**
 * @defgroup mifare_classic_link_driver mifare classic link driver function
 * @brief    mifare classic link driver modules
 * @ingroup  mifare_classic_driver
 * @{
 */

/**
 * @brief     initialize mifare_classic_handle_t structure
 * @param[in] HANDLE points to a mifare_classic handle structure
 * @param[in] STRUCTURE is mifare_classic_handle_t
 * @note      none
 */
#define DRIVER_MIFARE_CLASSIC_LINK_INIT(HANDLE, STRUCTURE)                  memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     link contactless_init function
 * @param[in] HANDLE points to a mifare_classic handle structure
 * @param[in] FUC points to a contactless_init function address
 * @note      none
 */
#define DRIVER_MIFARE_CLASSIC_LINK_CONTACTLESS_INIT(HANDLE, FUC)           (HANDLE)->contactless_init = FUC

/**
 * @brief     link contactless_deinit function
 * @param[in] HANDLE points to a mifare_classic handle structure
 * @param[in] FUC points to a contactless_deinit function address
 * @note      none
 */
#define DRIVER_MIFARE_CLASSIC_LINK_CONTACTLESS_DEINIT(HANDLE, FUC)         (HANDLE)->contactless_deinit = FUC

/**
 * @brief     link contactless_transceiver function
 * @param[in] HANDLE points to a mifare_classic handle structure
 * @param[in] FUC points to a contactless_transceiver function address
 * @note      none
 */
#define DRIVER_MIFARE_CLASSIC_LINK_CONTACTLESS_TRANSCEIVER(HANDLE, FUC)    (HANDLE)->contactless_transceiver = FUC

/**
 * @brief     link delay_ms function
 * @param[in] HANDLE points to a mifare_classic handle structure
 * @param[in] FUC points to a delay_ms function address
 * @note      none
 */
#define DRIVER_MIFARE_CLASSIC_LINK_DELAY_MS(HANDLE, FUC)                   (HANDLE)->delay_ms = FUC

/**
 * @brief     link debug_print function
 * @param[in] HANDLE points to a mifare_classic handle structure
 * @param[in] FUC points to a debug_print function address
 * @note      none
 */
#define DRIVER_MIFARE_CLASSIC_LINK_DEBUG_PRINT(HANDLE, FUC)                (HANDLE)->debug_print = FUC

/**
 * @}
 */

/**
 * @defgroup mifare_classic_basic_driver mifare classic basic driver function
 * @brief    mifare classic basic driver modules
 * @ingroup  mifare_classic_driver
 * @{
 */

/**
 * @brief      get chip information
 * @param[out] *info points to a mifare_classic info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t mifare_classic_info(mifare_classic_info_t *info);

/**
 * @brief     initialize the chip
 * @param[in] *handle points to a mifare_classic handle structure
 * @return    status code
 *            - 0 success
 *            - 1 contactless initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 * @note      none
 */
uint8_t mifare_classic_init(mifare_classic_handle_t *handle);

/**
 * @brief     close the chip
 * @param[in] *handle points to a mifare_classic handle structure
 * @return    status code
 *            - 0 success
 *            - 1 contactless deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t mifare_classic_deinit(mifare_classic_handle_t *handle);

/**
 * @brief      mifare request
 * @param[in]  *handle points to a mifare_classic handle structure
 * @param[out] *type points to a type buffer
 * @return     status code
 *             - 0 success
 *             - 1 request failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 output_len is invalid
 *             - 5 type is invalid
 * @note       none
 */
uint8_t mifare_classic_request(mifare_classic_handle_t *handle, mifare_classic_type_t *type);

/**
 * @brief      mifare wake up
 * @param[in]  *handle points to a mifare_classic handle structure
 * @param[out] *type points to a type buffer
 * @return     status code
 *             - 0 success
 *             - 1 wake up failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 output_len is invalid
 *             - 5 type is invalid
 * @note       none
 */
uint8_t mifare_classic_wake_up(mifare_classic_handle_t *handle, mifare_classic_type_t *type);

/**
 * @brief      mifare halt
 * @param[in]  *handle points to a mifare_classic handle structure
 * @return     status code
 *             - 0 success
 *             - 1 halt failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t mifare_classic_halt(mifare_classic_handle_t *handle);

/**
 * @brief     mifare set the load modulation
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] mod is the load modulation
 * @return    status code
 *            - 0 success
 *            - 1 set modulation failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 ack error
 * @note      none
 */
uint8_t mifare_classic_set_modulation(mifare_classic_handle_t *handle, mifare_classic_load_modulation_t mod);

/**
 * @brief     mifare set the personalized uid
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] type is the personalized uid type
 * @return    status code
 *            - 0 success
 *            - 1 set personalized uid failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 ack error
 * @note      none
 */
uint8_t mifare_classic_set_personalized_uid(mifare_classic_handle_t *handle, mifare_classic_personalized_uid_t type);

/**
 * @brief      mifare anti collision cl1
 * @param[in]  *handle points to a mifare_classic handle structure
 * @param[out] *id points to an id buffer
 * @return     status code
 *             - 0 success
 *             - 1 anti collision cl1 failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 output_len is invalid
 *             - 5 check error
 * @note       none
 */
uint8_t mifare_classic_anticollision_cl1(mifare_classic_handle_t *handle, uint8_t id[4]);

/**
 * @brief      mifare anti collision cl2
 * @param[in]  *handle points to a mifare_classic handle structure
 * @param[out] *id points to an id buffer
 * @return     status code
 *             - 0 success
 *             - 1 anti collision cl2 failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 output_len is invalid
 *             - 5 check error
 * @note       none
 */
uint8_t mifare_classic_anticollision_cl2(mifare_classic_handle_t *handle, uint8_t id[4]);

/**
 * @brief     mifare select cl1
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] *id points to an id buffer
 * @return    status code
 *            - 0 success
 *            - 1 select cl1 failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 sak error
 * @note      none
 */
uint8_t mifare_classic_select_cl1(mifare_classic_handle_t *handle, uint8_t id[4]);

/**
 * @brief     mifare select cl2
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] *id points to an id buffer
 * @return    status code
 *            - 0 success
 *            - 1 select cl2 failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 sak error
 * @note      none
 */
uint8_t mifare_classic_select_cl2(mifare_classic_handle_t *handle, uint8_t id[4]);

/**
 * @brief     mifare authentication
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] *id points to an id buffer
 * @param[in] block is the block of authentication
 * @param[in] key_type is the authentication key type
 * @param[in] *key points to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 authentication failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t mifare_classic_authentication(mifare_classic_handle_t *handle, uint8_t id[4], uint8_t block,
                                      mifare_classic_authentication_key_t key_type, uint8_t key[6]);

/**
 * @brief      mifare read
 * @param[in]  *handle points to a mifare_classic handle structure
 * @param[in]  block is the block of read
 * @param[out] *data points to a data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 output_len is invalid
 *             - 5 crc error
 * @note       none
 */
uint8_t mifare_classic_read(mifare_classic_handle_t *handle, uint8_t block, uint8_t data[16]);

/**
 * @brief     mifare write
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] block is the block of write
 * @param[in] *data points to a data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 ack error
 * @note      none
 */
uint8_t mifare_classic_write(mifare_classic_handle_t *handle, uint8_t block, uint8_t data[16]);

/**
 * @brief     mifare init one block as a value block
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] block is the set block
 * @param[in] value is the inited value
 * @param[in] addr is the set addr
 * @return    status code
 *            - 0 success
 *            - 1 value init failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 ack error
 * @note      none
 */
uint8_t mifare_classic_value_init(mifare_classic_handle_t *handle, uint8_t block, int32_t value, uint8_t addr);

/**
 * @brief     mifare value write
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] block is the set block
 * @param[in] value is the write value
 * @param[in] addr is the set addr
 * @return    status code
 *            - 0 success
 *            - 1 value write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 ack error
 * @note      none
 */
uint8_t mifare_classic_value_write(mifare_classic_handle_t *handle, uint8_t block, int32_t value, uint8_t addr);

/**
 * @brief      mifare value read
 * @param[in]  *handle points to a mifare_classic handle structure
 * @param[in]  block is the block of read
 * @param[out] *value points to a value buffer
 * @param[out] *addr points to an address buffer
 * @return     status code
 *             - 0 success
 *             - 1 value read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 output_len is invalid
 *             - 5 crc error
 *             - 6 value is invalid
 *             - 7 block is invalid
 * @note       none
 */
uint8_t mifare_classic_value_read(mifare_classic_handle_t *handle, uint8_t block, int32_t *value, uint8_t *addr);

/**
 * @brief     mifare increment
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] block is the block of increment
 * @param[in] value is the increment value
 * @return    status code
 *            - 0 success
 *            - 1 increment failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 ack error
 *            - 6 invalid operation
 * @note      none
 */
uint8_t mifare_classic_increment(mifare_classic_handle_t *handle, uint8_t block, uint32_t value);

/**
 * @brief     mifare decrement
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] block is the block of decrement
 * @param[in] value is the decrement value
 * @return    status code
 *            - 0 success
 *            - 1 decrement failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 ack error
 *            - 6 invalid operation
 * @note      none
 */
uint8_t mifare_classic_decrement(mifare_classic_handle_t *handle, uint8_t block, uint32_t value);

/**
 * @brief     mifare transfer
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] block is the block of transfer
 * @return    status code
 *            - 0 success
 *            - 1 transfer failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 ack error
 *            - 6 invalid operation
 * @note      none
 */
uint8_t mifare_classic_transfer(mifare_classic_handle_t *handle, uint8_t block);

/**
 * @brief     mifare restore
 * @param[in] *handle points to a mifare_classic handle structure
 * @param[in] block is the block of restore
 * @return    status code
 *            - 0 success
 *            - 1 restore failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 ack error
 *            - 6 invalid operation
 * @note      none
 */
uint8_t mifare_classic_restore(mifare_classic_handle_t *handle, uint8_t block);

/**
 * @brief      mifare block number to sector number
 * @param[in]  *handle points to a mifare_classic handle structure
 * @param[in]  block is the block number
 * @param[out] *sector points to a sector number buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t mifare_classic_block_to_sector(mifare_classic_handle_t *handle, uint8_t block, uint8_t *sector);

/**
 * @brief      mifare get the sector's block counter
 * @param[in]  *handle points to a mifare_classic handle structure
 * @param[in]  sector is the sector number
 * @param[out] *count points to a count buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t mifare_classic_sector_block_count(mifare_classic_handle_t *handle, uint8_t sector, uint8_t *count);

/**
 * @brief      mifare get the sector's first block
 * @param[in]  *handle points to a mifare_classic handle structure
 * @param[in]  sector is the sector number
 * @param[out] *block points to a block number buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t mifare_classic_sector_first_block(mifare_classic_handle_t *handle, uint8_t sector, uint8_t *block);

/**
 * @brief      mifare get the sector's last block
 * @param[in]  *handle points to a mifare_classic handle structure
 * @param[in]  sector is the sector number
 * @param[out] *block points to a block number buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t mifare_classic_sector_last_block(mifare_classic_handle_t *handle, uint8_t sector, uint8_t *block);

/**
 * @brief     mifare set the sector permission
 * @param[in] *handle points to a mifare_classic handle structure
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
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 output_len is invalid
 *            - 5 ack error
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
uint8_t mifare_classic_set_sector_permission(mifare_classic_handle_t *handle,
                                             uint8_t sector, uint8_t key_a[6], 
                                             uint8_t block_0_0_4, uint8_t block_1_5_9,
                                             uint8_t block_2_10_14, uint8_t block_3_15,
                                             uint8_t user_data, uint8_t key_b[6]);

/**
 * @brief      mifare get the sector permission
 * @param[in]  *handle points to a mifare_classic handle structure
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
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 output_len is invalid
 *             - 5 crc error
 *             - 6 data is invalid
 * @note       none
 */
uint8_t mifare_classic_get_sector_permission(mifare_classic_handle_t *handle,
                                             uint8_t sector, uint8_t *block_0_0_4, uint8_t *block_1_5_9,
                                             uint8_t *block_2_10_14, uint8_t *block_3_15,
                                             uint8_t *user_data, uint8_t key_b[6]);

/**
 * @}
 */

/**
 * @defgroup mifare_classic_extern_driver mifare classic extern driver function
 * @brief    mifare classic extern driver modules
 * @ingroup  mifare_classic_driver
 * @{
 */

/**
 * @brief         transceiver data
 * @param[in]     *handle points to a mifare_classic handle structure
 * @param[in]     *in_buf points to an input buffer
 * @param[in]     in_len is the input length
 * @param[out]    *out_buf points to an output buffer
 * @param[in,out] *out_len points to an output length buffer
 * @return        status code
 *                - 0 success
 *                - 1 transceiver failed
 * @note          none
 */
uint8_t mifare_classic_transceiver(mifare_classic_handle_t *handle, uint8_t *in_buf, uint8_t in_len, uint8_t *out_buf, uint8_t *out_len);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
