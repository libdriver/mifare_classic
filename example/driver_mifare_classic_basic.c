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
 * @file      driver_mifare_classic_basic.c
 * @brief     driver mifare classic basic source file
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

#include "driver_mifare_classic_basic.h"

static mifare_classic_handle_t gs_handle;        /**< mifare_classic handle */
static uint8_t gs_id[4];                         /**< local id */

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
static void s_debug_print(const char *const fmt, ...)
{
    (void)fmt;
    
    return;
}

/**
 * @brief  basic example init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t mifare_classic_basic_init(void)
{
    uint8_t res;
    
    /* link function */
    DRIVER_MIFARE_CLASSIC_LINK_INIT(&gs_handle, mifare_classic_handle_t);
    DRIVER_MIFARE_CLASSIC_LINK_CONTACTLESS_INIT(&gs_handle, mifare_classic_interface_contactless_init);
    DRIVER_MIFARE_CLASSIC_LINK_CONTACTLESS_DEINIT(&gs_handle, mifare_classic_interface_contactless_deinit);
    DRIVER_MIFARE_CLASSIC_LINK_CONTACTLESS_TRANSCEIVER(&gs_handle, mifare_classic_interface_contactless_transceiver);
    DRIVER_MIFARE_CLASSIC_LINK_DELAY_MS(&gs_handle, mifare_classic_interface_delay_ms);
#ifndef NO_DEBUG
    DRIVER_MIFARE_CLASSIC_LINK_DEBUG_PRINT(&gs_handle, mifare_classic_interface_debug_print);
#else
    DRIVER_MIFARE_CLASSIC_LINK_DEBUG_PRINT(&gs_handle, s_debug_print);
#endif
    
    /* init */
    res = mifare_classic_init(&gs_handle);
    if (res != 0)
    {
        mifare_classic_interface_debug_print("mifare_classic: init failed.\n");
        
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t mifare_classic_basic_deinit(void)
{
    uint8_t res;
    
    /* deinit */
    res = mifare_classic_deinit(&gs_handle);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example halt
 * @return status code
 *         - 0 success
 *         - 1 halt failed
 * @note   none
 */
uint8_t mifare_classic_basic_halt(void)
{
    uint8_t res;
    
    /* halt */
    res = mifare_classic_halt(&gs_handle);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example wake up
 * @return status code
 *         - 0 success
 *         - 1 wake up failed
 * @note   none
 */
uint8_t mifare_classic_basic_wake_up(void)
{
    uint8_t res;
    mifare_classic_type_t type;
    
    /* wake up */
    res = mifare_classic_wake_up(&gs_handle, &type);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example search
 * @param[out] *type pointer to a type buffer
 * @param[out] *id pointer to an id buffer
 * @param[in]  timeout check times
 * @return     status code
 *             - 0 success
 *             - 1 timeout
 * @note       none
 */
uint8_t mifare_classic_basic_search(mifare_classic_type_t *type, uint8_t id[4], int32_t timeout)
{
    uint8_t res;
    
    /* loop */
    while (1)
    {
        /* request */
        res = mifare_classic_request(&gs_handle, type);
        if (res == 0)
        {
            /* anti collision_cl1 */
            res = mifare_classic_anticollision_cl1(&gs_handle, id);
            if (res == 0)
            {
                /* cl1 */
                res = mifare_classic_select_cl1(&gs_handle, id);
                if (res == 0)
                {
                    memcpy(gs_id, id, 4);
                    
                    return 0;
                }
            }
        }
        
        /* delay */
        mifare_classic_interface_delay_ms(MIFARE_CLASSIC_BASIC_DEFAULT_SEARCH_DELAY_MS);
        
        /* check the timeout */
        if (timeout < 0)
        {
            /* never timeout */
            continue;
        }
        else
        {
            /* timeout */
            if (timeout == 0)
            {
                return 1;
            }
            else
            {
                /* timout-- */
                timeout--;
            }
        }
    }
}

/**
 * @brief      basic example read
 * @param[in]  block block of read
 * @param[out] *data pointer to a data buffer
 * @param[in]  key_type authentication key type
 * @param[in]  *key pointer to a key buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t mifare_classic_basic_read(uint8_t block, uint8_t data[16],
                                  mifare_classic_authentication_key_t key_type, uint8_t key[6])
{
    uint8_t res;
    uint8_t sector;
    uint8_t block_check;
    
    /* check the block */
    res = mifare_classic_block_to_sector(&gs_handle, block, &sector);
    if (res != 0)
    {
        return 1;
    }
    res = mifare_classic_sector_last_block(&gs_handle, sector, &block_check);
    if (res != 0)
    {
        return 1;
    }
    if (block == block_check)
    {
        return 1;
    }
    
    /* authentication */
    res = mifare_classic_authentication(&gs_handle, gs_id, block, key_type, key);
    if (res != 0)
    {
        return 1;
    }
    
    /* read */
    res = mifare_classic_read(&gs_handle, block, data);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     basic example write
 * @param[in] block block of write
 * @param[in] *data pointer to a data buffer
 * @param[in] key_type authentication key type
 * @param[in] *key pointer to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t mifare_classic_basic_write(uint8_t block, uint8_t data[16],
                                   mifare_classic_authentication_key_t key_type, uint8_t key[6])
{
    uint8_t res;
    uint8_t sector;
    uint8_t block_check;
    
    /* check the block */
    res = mifare_classic_block_to_sector(&gs_handle, block, &sector);
    if (res != 0)
    {
        return 1;
    }
    res = mifare_classic_sector_last_block(&gs_handle, sector, &block_check);
    if (res != 0)
    {
        return 1;
    }
    if (block == block_check)
    {
        return 1;
    }
    
    /* authentication */
    res = mifare_classic_authentication(&gs_handle, gs_id, block, key_type, key);
    if (res != 0)
    {
        return 1;
    }
    
    /* write */
    res = mifare_classic_write(&gs_handle, block, data);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     basic example init as a value
 * @param[in] block block of init
 * @param[in] value inited value
 * @param[in] addr address
 * @param[in] key_type authentication key type
 * @param[in] *key pointer to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 value init failed
 * @note      none
 */
uint8_t mifare_classic_basic_value_init(uint8_t block, int32_t value, uint8_t addr,
                                        mifare_classic_authentication_key_t key_type, uint8_t key[6])
{
    uint8_t res;
    uint8_t sector;
    uint8_t block_check;
    
    /* check the block */
    res = mifare_classic_block_to_sector(&gs_handle, block, &sector);
    if (res != 0)
    {
        return 1;
    }
    res = mifare_classic_sector_last_block(&gs_handle, sector, &block_check);
    if (res != 0)
    {
        return 1;
    }
    if (block == block_check)
    {
        return 1;
    }
    
    /* authentication */
    res = mifare_classic_authentication(&gs_handle, gs_id, block, key_type, key);
    if (res != 0)
    {
        return 1;
    }
    
    /* value init */
    res  = mifare_classic_value_init(&gs_handle, block, value, addr);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     basic example write value
 * @param[in] block block of write
 * @param[in] value written value
 * @param[in] addr address
 * @param[in] key_type authentication key type
 * @param[in] *key pointer to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 value written failed
 * @note      none
 */
uint8_t mifare_classic_basic_value_write(uint8_t block, int32_t value, uint8_t addr,
                                         mifare_classic_authentication_key_t key_type, uint8_t key[6])
{
    uint8_t res;
    uint8_t sector;
    uint8_t block_check;
    
    /* check the block */
    res = mifare_classic_block_to_sector(&gs_handle, block, &sector);
    if (res != 0)
    {
        return 1;
    }
    res = mifare_classic_sector_last_block(&gs_handle, sector, &block_check);
    if (res != 0)
    {
        return 1;
    }
    if (block == block_check)
    {
        return 1;
    }
    
    /* authentication */
    res = mifare_classic_authentication(&gs_handle, gs_id, block, key_type, key);
    if (res != 0)
    {
        return 1;
    }
    
    /* value write */
    res  = mifare_classic_value_write(&gs_handle, block, value, addr);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example read value
 * @param[in]  block block of read
 * @param[out] *value pointer to a read value buffer
 * @param[out] *addr pointer to a read address buffer
 * @param[in]  key_type authentication key type
 * @param[in]  *key pointer to a key buffer
 * @return     status code
 *             - 0 success
 *             - 1 value read failed
 * @note       none
 */
uint8_t mifare_classic_basic_value_read(uint8_t block, int32_t *value, uint8_t *addr,
                                         mifare_classic_authentication_key_t key_type, uint8_t key[6])
{
    uint8_t res;
    uint8_t sector;
    uint8_t block_check;
    
    /* check the block */
    res = mifare_classic_block_to_sector(&gs_handle, block, &sector);
    if (res != 0)
    {
        return 1;
    }
    res = mifare_classic_sector_last_block(&gs_handle, sector, &block_check);
    if (res != 0)
    {
        return 1;
    }
    if (block == block_check)
    {
        return 1;
    }
    
    /* authentication */
    res = mifare_classic_authentication(&gs_handle, gs_id, block, key_type, key);
    if (res != 0)
    {
        return 1;
    }
    
    /* value read */
    res  = mifare_classic_value_read(&gs_handle, block, value, addr);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     basic example increment value
 * @param[in] block block of increment
 * @param[in] value increment value
 * @param[in] key_type authentication key type
 * @param[in] *key pointer to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 value increment failed
 * @note      none
 */
uint8_t mifare_classic_basic_value_increment(uint8_t block, uint32_t value,
                                             mifare_classic_authentication_key_t key_type, uint8_t key[6])
{
    uint8_t res;
    uint8_t sector;
    uint8_t block_check;
    
    /* check the block */
    res = mifare_classic_block_to_sector(&gs_handle, block, &sector);
    if (res != 0)
    {
        return 1;
    }
    res = mifare_classic_sector_last_block(&gs_handle, sector, &block_check);
    if (res != 0)
    {
        return 1;
    }
    if (block == block_check)
    {
        return 1;
    }
    
    /* authentication */
    res = mifare_classic_authentication(&gs_handle, gs_id, block, key_type, key);
    if (res != 0)
    {
        return 1;
    }
    
    /* increment */
    res  = mifare_classic_increment(&gs_handle, block, value);
    if (res != 0)
    {
        return 1;
    }
    res = mifare_classic_transfer(&gs_handle, block);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     basic example decrement value
 * @param[in] block block of decrement
 * @param[in] value decrement value
 * @param[in] key_type authentication key type
 * @param[in] *key pointer to a key buffer
 * @return    status code
 *            - 0 success
 *            - 1 value decrement failed
 * @note      none
 */
uint8_t mifare_classic_basic_value_decrement(uint8_t block, uint32_t value,
                                             mifare_classic_authentication_key_t key_type, uint8_t key[6])
{
    uint8_t res;
    uint8_t sector;
    uint8_t block_check;
    
    /* check the block */
    res = mifare_classic_block_to_sector(&gs_handle, block, &sector);
    if (res != 0)
    {
        return 1;
    }
    res = mifare_classic_sector_last_block(&gs_handle, sector, &block_check);
    if (res != 0)
    {
        return 1;
    }
    if (block == block_check)
    {
        return 1;
    }
    
    /* authentication */
    res = mifare_classic_authentication(&gs_handle, gs_id, block, key_type, key);
    if (res != 0)
    {
        return 1;
    }
    
    /* decrement */
    res  = mifare_classic_decrement(&gs_handle, block, value);
    if (res != 0)
    {
        return 1;
    }
    res = mifare_classic_transfer(&gs_handle, block);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     basic example set the sector permission
 * @param[in] key_type authentication key type
 * @param[in] *key pointer to a key buffer
 * @param[in] sector set sector
 * @param[in] *key_a pointer to a key a buffer
 * @param[in] block_0_0_4 block0(block0-4) permission
 * @param[in] block_1_5_9 block1(block5-9) permission
 * @param[in] block_2_10_14 block2(block10-14) permission
 * @param[in] block_3_15 block3(block15) permission
 * @param[in] user_data user data
 * @param[in] *key_b pointer to a key b buffer
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
                                            uint8_t block_2_10_14, uint8_t block_3_15, uint8_t user_data, uint8_t key_b[6])
{
    uint8_t res;
    uint8_t block;
    
    /* get the last block */
    res = mifare_classic_sector_last_block(&gs_handle, sector, &block);
    if (res != 0)
    {
        return 1;
    }
    
    /* authentication */
    res = mifare_classic_authentication(&gs_handle, gs_id, block, key_type, key);
    if (res != 0)
    {
        return 1;
    }
    
    /* set sector permission */
    res = mifare_classic_set_sector_permission(&gs_handle, sector, key_a, block_0_0_4, block_1_5_9, 
                                               block_2_10_14, block_3_15, user_data, key_b);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example get the sector permission
 * @param[in]  key_type authentication key type
 * @param[in]  *key pointer to a key buffer
 * @param[in]  sector get sector
 * @param[out] *block_0_0_4 pointer to a block0(block0-4) permission buffer
 * @param[out] *block_1_5_9 pointer to a block1(block5-9) permission buffer
 * @param[out] *block_2_10_14 pointer to a block2(block10-14) permission buffer
 * @param[out] *block_3_15 pointer to a block3(block15) permission buffer
 * @param[out] *user_data pointer to a user data buffer
 * @param[out] *key_b pointer to a key b buffer
 * @return     status code
 *             - 0 success
 *             - 1 get sector permission failed
 * @note       none
 */
uint8_t mifare_classic_basic_get_permission(mifare_classic_authentication_key_t key_type, uint8_t key[6],
                                            uint8_t sector, uint8_t *block_0_0_4, uint8_t *block_1_5_9,
                                            uint8_t *block_2_10_14, uint8_t *block_3_15, uint8_t *user_data, uint8_t key_b[6])
{
    uint8_t res;
    uint8_t block;
    
    /* get the last block */
    res = mifare_classic_sector_last_block(&gs_handle, sector, &block);
    if (res != 0)
    {
        return 1;
    }
    
    /* authentication */
    res = mifare_classic_authentication(&gs_handle, gs_id, block, key_type, key);
    if (res != 0)
    {
        return 1;
    }
    
    /* set sector permission */
    res = mifare_classic_get_sector_permission(&gs_handle, sector, block_0_0_4, block_1_5_9, 
                                               block_2_10_14, block_3_15, user_data, key_b);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}
