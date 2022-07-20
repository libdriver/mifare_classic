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
 * @file      main.c
 * @brief     main source file
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
#include "driver_mifare_classic_card_test.h"
#include "shell.h"
#include "clock.h"
#include "delay.h"
#include "uart.h"
#include "gpio.h"
#include <stdlib.h>

/**
 * @brief global var definition
 */
uint8_t g_buf[256];                        /**< uart buffer */
uint16_t g_len;                            /**< uart buffer length */

/**
 * @brief     mifare_classic full function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *             - 0 success
 *             - 1 run failed
 *             - 5 param is invalid
 * @note      none
 */
uint8_t mifare_classic(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
        goto help;
    }
    else if (argc == 2)
    {
        if (strcmp("-i", argv[1]) == 0)
        {
            mifare_classic_info_t info;
            
            /* print mifare_classic info */
            mifare_classic_info(&info);
            mifare_classic_interface_debug_print("mifare_classic: chip is %s.\n", info.chip_name);
            mifare_classic_interface_debug_print("mifare_classic: manufacturer is %s.\n", info.manufacturer_name);
            mifare_classic_interface_debug_print("mifare_classic: interface is %s.\n", info.interface);
            mifare_classic_interface_debug_print("mifare_classic: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
            mifare_classic_interface_debug_print("mifare_classic: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
            mifare_classic_interface_debug_print("mifare_classic: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
            mifare_classic_interface_debug_print("mifare_classic: max current is %0.2fmA.\n", info.max_current_ma);
            mifare_classic_interface_debug_print("mifare_classic: max temperature is %0.1fC.\n", info.temperature_max);
            mifare_classic_interface_debug_print("mifare_classic: min temperature is %0.1fC.\n", info.temperature_min);
            
            return 0;
        }
        else if (strcmp("-p", argv[1]) == 0)
        {
            /* print pin connection */
            mifare_classic_interface_debug_print("mifare_classic: SPI interface SCK connected to GPIOA PIN5.\n");
            mifare_classic_interface_debug_print("mifare_classic: SPI interface MISO connected to GPIOA PIN6.\n");
            mifare_classic_interface_debug_print("mifare_classic: SPI interface MOSI connected to GPIOA PIN7.\n");
            mifare_classic_interface_debug_print("mifare_classic: SPI interface CS connected to GPIOA PIN4.\n");
            mifare_classic_interface_debug_print("mifare_classic: IIC interface SCL connected to GPIOB PIN8.\n");
            mifare_classic_interface_debug_print("mifare_classic: IIC interface SDA connected to GPIOB PIN9.\n");
            mifare_classic_interface_debug_print("mifare_classic: UART interface TX connected to GPIOA PIN2.\n");
            mifare_classic_interface_debug_print("mifare_classic: UART interface RX connected to GPIOA PIN3.\n");
            mifare_classic_interface_debug_print("mifare_classic: INT connected to GPIOB PIN0.\n");
            mifare_classic_interface_debug_print("mifare_classic: RESET connected to GPIOA PIN8.\n");
            
            return 0;
        }
        else if (strcmp("-h", argv[1]) == 0)
        {
            /* show mifare_classic help */
            help:
            
            mifare_classic_interface_debug_print("mifare_classic -i\n\tshow mifare_classic chip and driver information.\n");
            mifare_classic_interface_debug_print("mifare_classic -h\n\tshow mifare_classic help.\n");
            mifare_classic_interface_debug_print("mifare_classic -p\n\tshow mifare_classic pin connections of the current board.\n");
            mifare_classic_interface_debug_print("mifare_classic -t card\n\trun mifare_classic card test.\n");
            mifare_classic_interface_debug_print("mifare_classic -c halt\n\tchip halt.\n");
            mifare_classic_interface_debug_print("mifare_classic -c wake_up\n\twake up the chip.\n");
            mifare_classic_interface_debug_print("mifare_classic -c -k (a | b) <keys> read <block>\n\tread the block."
                                                 "block is the read block.keys is the authentication keys.\n");
            mifare_classic_interface_debug_print("mifare_classic -c -k (a | b) <keys> write <block> <n>\n\twrite the block."
                                                 "block is the written block.keys is the authentication keys.n is the written data.\n");
            mifare_classic_interface_debug_print("mifare_classic -c -k (a | b) <keys> value -init <block> <n>\n\tinit as a value block."
                                                 "block is the set block.keys is the authentication keys.n is the inted value.\n");
            mifare_classic_interface_debug_print("mifare_classic -c -k (a | b) <keys> value -write <block> <n>\n\twrite a value to the block."
                                                 "block is the set block.keys is the authentication keys.n is the written value.\n");
            mifare_classic_interface_debug_print("mifare_classic -c -k (a | b) <keys> value -read <block>\n\tread a value from the block."
                                                 "block is the set block.keys is the authentication keys.\n");
            mifare_classic_interface_debug_print("mifare_classic -c -k (a | b) <keys> value -increment <block> <n>\n\tincrement the value."
                                                 "block is the set block.keys is the authentication keys.n is the increment value.\n");
            mifare_classic_interface_debug_print("mifare_classic -c -k (a | b) <keys> value -decrement <block> <n>\n\tdecrement the value."
                                                 "block is the set block.keys is the authentication keys.n is the decrement value.\n");
            
            return 0;
        }
        else
        {
            return 5;
        }
    }
    else if (argc == 3)
    {
        if (strcmp("-t", argv[1]) == 0)
        {
            if (strcmp("card", argv[2]) == 0)
            {
                uint8_t res;
                
                /* run the card test */
                res = mifare_classic_card_test();
                if (res != 0)
                {
                    return 1;
                }
                
                return 0;
            }
            else
            {
                return 5;
            }
        }
        else if (strcmp("-c", argv[1]) == 0)
        {
            if (strcmp("halt", argv[2]) == 0)
            {
                uint8_t res;
                
                /* init */
                res = mifare_classic_basic_init();
                if (res != 0)
                {
                    return 1;
                }
                
                /* halt */
                res = mifare_classic_basic_halt();
                if (res != 0)
                {
                    (void)mifare_classic_basic_deinit();
                    
                    return 1;
                }
                mifare_classic_interface_debug_print("mifare_classic: halt.\n");
                
                (void)mifare_classic_basic_deinit();
                
                return 0;
            }
            else if (strcmp("wake_up", argv[2]) == 0)
            {
                uint8_t res;
                
                /* init */
                res = mifare_classic_basic_init();
                if (res != 0)
                {
                    return 1;
                }
                
                /* wake_up */
                res = mifare_classic_basic_wake_up();
                if (res != 0)
                {
                    (void)mifare_classic_basic_deinit();
                    
                    return 1;
                }
                mifare_classic_interface_debug_print("mifare_classic: wake up.\n");
                
                (void)mifare_classic_basic_deinit();
                
                return 0;
            }
            else
            {
                return 5;
            }
        }
        else
        {
            return 5;
        }
    }
    else if (argc == 7)
    {
        if (strcmp("-c", argv[1]) == 0)
        {
            if (strcmp("-k", argv[2]) == 0)
            {
                mifare_classic_authentication_key_t key_type;
                uint8_t i;
                uint8_t res;
                uint8_t size;
                uint8_t key[6];
                
                if (strcmp("a", argv[3]) == 0)
                {
                    key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_A;
                }
                else if (strcmp("b", argv[3]) == 0)
                {
                    key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_B;
                }
                else
                {
                    return 5;
                }
                
                size = (uint8_t)strlen(argv[4]);
                for (i = 0; i < size; i += 2)
                {
                    if (('A' <= argv[4][i]) && (argv[4][i] <= 'Z'))
                    {
                        key[i / 2] = argv[4][i] - 'A' + 10;
                        key[i / 2] *= 16;
                    }
                    else if (('a' <= argv[4][i]) && (argv[4][i] <= 'z'))
                    {
                        key[i / 2] = argv[4][i] - 'a' + 10;
                        key[i / 2] *= 16;
                    }
                    else if (('0' <= argv[4][i]) && (argv[4][i] <= '9'))
                    {
                        key[i / 2] = argv[4][i] - '0';
                        key[i / 2] *= 16;
                    }
                    else
                    {
                        return 5;
                    }
                    
                    if (('A' <= argv[4][i + 1]) && (argv[4][i + 1] <= 'Z'))
                    {
                        key[i / 2 ] += argv[4][i + 1] - 'A' + 10;
                    }
                    else if (('a' <= argv[4][i + 1]) && (argv[4][i + 1] <= 'z'))
                    {
                        key[i / 2 ] += argv[4][i + 1] - 'a' + 10;
                    }
                    else if (('0' <= argv[4][i + 1]) && (argv[4][i + 1] <= '9'))
                    {
                        key[i / 2 ] += argv[4][i + 1] - '0';
                    }
                    else
                    {
                        return 5;
                    }
                }
                
                if (strcmp("read", argv[5]) == 0)
                {
                    mifare_classic_type_t type; 
                    uint8_t id[4];
                    uint8_t data[16];
                    
                    /* init */
                    res = mifare_classic_basic_init();
                    if (res != 0)
                    {
                        return 1;
                    }
                    
                    /* search */
                    res = mifare_classic_basic_search(&type, id, 50);
                    if (res != 0)
                    {
                        (void)mifare_classic_basic_deinit();
                        
                        return 1;
                    }
                    
                    /* ouput the type */
                    if (type == MIFARE_CLASSIC_TYPE_S50)
                    {
                        mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
                    }
                    else if (type == MIFARE_CLASSIC_TYPE_S70)
                    {
                        mifare_classic_interface_debug_print("mifare_classic: find S70 card.\n");
                    }
                    else
                    {
                        mifare_classic_interface_debug_print("mifare_classic: invalid type.\n");
                        (void)mifare_classic_basic_deinit();
                        
                        return 1;
                    }
                    mifare_classic_interface_debug_print("mifare_classic: id is ");
                    for (i = 0; i < 4; i++)
                    {
                        mifare_classic_interface_debug_print("0x%02X ", id[i]);
                    }
                    mifare_classic_interface_debug_print("\n");
                    
                    /* read */
                    res = mifare_classic_basic_read((uint8_t)atoi(argv[6]), data, key_type, key);
                    if (res != 0)
                    {
                        (void)mifare_classic_basic_deinit();
                        
                        return 1;
                    }
                    
                    mifare_classic_interface_debug_print("mifare_classic: block is ");
                    for (i = 0; i < 16; i++)
                    {
                        mifare_classic_interface_debug_print("0x%02X ", data[i]);
                    }
                    mifare_classic_interface_debug_print("\n");
                    
                    (void)mifare_classic_basic_deinit();
                    
                    return 0;
                }
                else
                {
                    return 5;
                }
            }
            else
            {
                return 5;
            }
        }
        else
        {
            return 5;
        }
    }
    else if (argc == 8)
    {
        if (strcmp("-c", argv[1]) == 0)
        {
            if (strcmp("-k", argv[2]) == 0)
            {
                mifare_classic_authentication_key_t key_type;
                uint8_t i;
                uint8_t res;
                uint8_t size;
                uint8_t key[6];
                
                if (strcmp("a", argv[3]) == 0)
                {
                    key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_A;
                }
                else if (strcmp("b", argv[3]) == 0)
                {
                    key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_B;
                }
                else
                {
                    return 5;
                }
                
                size = (uint8_t)strlen(argv[4]);
                for (i = 0; i < size; i += 2)
                {
                    if (('A' <= argv[4][i]) && (argv[4][i] <= 'Z'))
                    {
                        key[i / 2] = argv[4][i] - 'A' + 10;
                        key[i / 2] *= 16;
                    }
                    else if (('a' <= argv[4][i]) && (argv[4][i] <= 'z'))
                    {
                        key[i / 2] = argv[4][i] - 'a' + 10;
                        key[i / 2] *= 16;
                    }
                    else if (('0' <= argv[4][i]) && (argv[4][i] <= '9'))
                    {
                        key[i / 2] = argv[4][i] - '0';
                        key[i / 2] *= 16;
                    }
                    else
                    {
                        return 5;
                    }
                    
                    if (('A' <= argv[4][i + 1]) && (argv[4][i + 1] <= 'Z'))
                    {
                        key[i / 2 ] += argv[4][i + 1] - 'A' + 10;
                    }
                    else if (('a' <= argv[4][i + 1]) && (argv[4][i + 1] <= 'z'))
                    {
                        key[i / 2 ] += argv[4][i + 1] - 'a' + 10;
                    }
                    else if (('0' <= argv[4][i + 1]) && (argv[4][i + 1] <= '9'))
                    {
                        key[i / 2 ] += argv[4][i + 1] - '0';
                    }
                    else
                    {
                        return 5;
                    }
                }
                
                if (strcmp("write", argv[5]) == 0)
                {
                    mifare_classic_type_t type; 
                    uint8_t id[4];
                    uint8_t data[16];
                    
                    size = (uint8_t)strlen(argv[7]);
                    for (i = 0; i < size; i += 2)
                    {
                        if (('A' <= argv[7][i]) && (argv[7][i] <= 'Z'))
                        {
                            data[i / 2] = argv[7][i] - 'A' + 10;
                            data[i / 2] *= 16;
                        }
                        else if (('a' <= argv[7][i]) && (argv[7][i] <= 'z'))
                        {
                            data[i / 2] = argv[7][i] - 'a' + 10;
                            data[i / 2] *= 16;
                        }
                        else if (('0' <= argv[7][i]) && (argv[7][i] <= '9'))
                        {
                            data[i / 2] = argv[7][i] - '0';
                            data[i / 2] *= 16;
                        }
                        else
                        {
                            return 5;
                        }
                        
                        if (('A' <= argv[7][i + 1]) && (argv[7][i + 1] <= 'Z'))
                        {
                            data[i / 2 ] += argv[7][i + 1] - 'A' + 10;
                        }
                        else if (('a' <= argv[7][i + 1]) && (argv[7][i + 1] <= 'z'))
                        {
                            data[i / 2 ] += argv[7][i + 1] - 'a' + 10;
                        }
                        else if (('0' <= argv[7][i + 1]) && (argv[7][i + 1] <= '9'))
                        {
                            data[i / 2 ] += argv[7][i + 1] - '0';
                        }
                        else
                        {
                            return 5;
                        }
                    }
                    
                    /* init */
                    res = mifare_classic_basic_init();
                    if (res != 0)
                    {
                        return 1;
                    }
                    
                    /* search */
                    res = mifare_classic_basic_search(&type, id, 50);
                    if (res != 0)
                    {
                        (void)mifare_classic_basic_deinit();
                        
                        return 1;
                    }
                    
                    /* ouput the type */
                    if (type == MIFARE_CLASSIC_TYPE_S50)
                    {
                        mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
                    }
                    else if (type == MIFARE_CLASSIC_TYPE_S70)
                    {
                        mifare_classic_interface_debug_print("mifare_classic: find S70 card.\n");
                    }
                    else
                    {
                        mifare_classic_interface_debug_print("mifare_classic: invalid type.\n");
                        (void)mifare_classic_basic_deinit();
                        
                        return 1;
                    }
                    mifare_classic_interface_debug_print("mifare_classic: id is ");
                    for (i = 0; i < 4; i++)
                    {
                        mifare_classic_interface_debug_print("0x%02X ", id[i]);
                    }
                    mifare_classic_interface_debug_print("\n");
                    
                    mifare_classic_interface_debug_print("mifare_classic: data is ");
                    for (i = 0; i < 16; i++)
                    {
                        mifare_classic_interface_debug_print("0x%02X ", data[i]);
                    }
                    mifare_classic_interface_debug_print("\n");
                    
                    /* write */
                    res = mifare_classic_basic_write((uint8_t)atoi(argv[6]), data, key_type, key);
                    if (res != 0)
                    {
                        (void)mifare_classic_basic_deinit();
                        
                        return 1;
                    }
                    
                    (void)mifare_classic_basic_deinit();
                    
                    return 0;
                }
                else if (strcmp("value", argv[5]) == 0)
                {
                    mifare_classic_type_t type; 
                    uint8_t id[4];
                    uint8_t addr;
                    int32_t value;
                    
                    if (strcmp("-read", argv[6]) == 0)
                    {
                        /* init */
                        res = mifare_classic_basic_init();
                        if (res != 0)
                        {
                            return 1;
                        }
                        /* search */
                        res = mifare_classic_basic_search(&type, id, 50);
                        if (res != 0)
                        {
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        
                        /* ouput the type */
                        if (type == MIFARE_CLASSIC_TYPE_S50)
                        {
                            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
                        }
                        else if (type == MIFARE_CLASSIC_TYPE_S70)
                        {
                            mifare_classic_interface_debug_print("mifare_classic: find S70 card.\n");
                        }
                        else
                        {
                            mifare_classic_interface_debug_print("mifare_classic: invalid type.\n");
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        mifare_classic_interface_debug_print("mifare_classic: id is ");
                        for (i = 0; i < 4; i++)
                        {
                            mifare_classic_interface_debug_print("0x%02X ", id[i]);
                        }
                        mifare_classic_interface_debug_print("\n");
                        
                        /* read the value */
                        res = mifare_classic_basic_value_read((uint8_t)atoi(argv[7]), &value, &addr, key_type, key);
                        if (res != 0)
                        {
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        mifare_classic_interface_debug_print("mifare_classic: read block%d value %d.\n", (uint8_t)atoi(argv[7]), value);
                        
                        (void)mifare_classic_basic_deinit();
                        
                        return 0;
                    }
                    else
                    {
                        return 5;
                    }
                }
                else
                {
                    return 5;
                }
            }
            else
            {
                return 5;
            }
        }
        else
        {
            return 5;
        }
    }
    else if (argc == 9)
    {
        if (strcmp("-c", argv[1]) == 0)
        {
            if (strcmp("-k", argv[2]) == 0)
            {
                mifare_classic_authentication_key_t key_type;
                uint8_t i;
                uint8_t res;
                uint8_t size;
                uint8_t key[6];
                
                if (strcmp("a", argv[3]) == 0)
                {
                    key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_A;
                }
                else if (strcmp("b", argv[3]) == 0)
                {
                    key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_B;
                }
                else
                {
                    return 5;
                }
                
                size = (uint8_t)strlen(argv[4]);
                for (i = 0; i < size; i += 2)
                {
                    if (('A' <= argv[4][i]) && (argv[4][i] <= 'Z'))
                    {
                        key[i / 2] = argv[4][i] - 'A' + 10;
                        key[i / 2] *= 16;
                    }
                    else if (('a' <= argv[4][i]) && (argv[4][i] <= 'z'))
                    {
                        key[i / 2] = argv[4][i] - 'a' + 10;
                        key[i / 2] *= 16;
                    }
                    else if (('0' <= argv[4][i]) && (argv[4][i] <= '9'))
                    {
                        key[i / 2] = argv[4][i] - '0';
                        key[i / 2] *= 16;
                    }
                    else
                    {
                        return 5;
                    }
                    
                    if (('A' <= argv[4][i + 1]) && (argv[4][i + 1] <= 'Z'))
                    {
                        key[i / 2 ] += argv[4][i + 1] - 'A' + 10;
                    }
                    else if (('a' <= argv[4][i + 1]) && (argv[4][i + 1] <= 'z'))
                    {
                        key[i / 2 ] += argv[4][i + 1] - 'a' + 10;
                    }
                    else if (('0' <= argv[4][i + 1]) && (argv[4][i + 1] <= '9'))
                    {
                        key[i / 2 ] += argv[4][i + 1] - '0';
                    }
                    else
                    {
                        return 5;
                    }
                }
                
                if (strcmp("value", argv[5]) == 0)
                {
                    mifare_classic_type_t type; 
                    uint8_t id[4];
                    
                    if (strcmp("-init", argv[6]) == 0)
                    {
                        /* init */
                        res = mifare_classic_basic_init();
                        if (res != 0)
                        {
                            return 1;
                        }
                        /* search */
                        res = mifare_classic_basic_search(&type, id, 50);
                        if (res != 0)
                        {
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        
                        /* ouput the type */
                        if (type == MIFARE_CLASSIC_TYPE_S50)
                        {
                            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
                        }
                        else if (type == MIFARE_CLASSIC_TYPE_S70)
                        {
                            mifare_classic_interface_debug_print("mifare_classic: find S70 card.\n");
                        }
                        else
                        {
                            mifare_classic_interface_debug_print("mifare_classic: invalid type.\n");
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        mifare_classic_interface_debug_print("mifare_classic: id is ");
                        for (i = 0; i < 4; i++)
                        {
                            mifare_classic_interface_debug_print("0x%02X ", id[i]);
                        }
                        mifare_classic_interface_debug_print("\n");
                        
                        res = mifare_classic_basic_value_init((uint8_t)atoi(argv[7]), atoi(argv[8]), (uint8_t)atoi(argv[7]),
                                                              key_type, key);
                        if (res != 0)
                        {
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        mifare_classic_interface_debug_print("mifare_classic: init block%d as value %d.\n", (uint8_t)atoi(argv[7]), atoi(argv[8]));
                        
                        (void)mifare_classic_basic_deinit();
                        
                        return 0;
                    }
                    else if (strcmp("-write", argv[6]) == 0)
                    {
                        /* init */
                        res = mifare_classic_basic_init();
                        if (res != 0)
                        {
                            return 1;
                        }
                        /* search */
                        res = mifare_classic_basic_search(&type, id, 50);
                        if (res != 0)
                        {
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        
                        /* ouput the type */
                        if (type == MIFARE_CLASSIC_TYPE_S50)
                        {
                            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
                        }
                        else if (type == MIFARE_CLASSIC_TYPE_S70)
                        {
                            mifare_classic_interface_debug_print("mifare_classic: find S70 card.\n");
                        }
                        else
                        {
                            mifare_classic_interface_debug_print("mifare_classic: invalid type.\n");
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        mifare_classic_interface_debug_print("mifare_classic: id is ");
                        for (i = 0; i < 4; i++)
                        {
                            mifare_classic_interface_debug_print("0x%02X ", id[i]);
                        }
                        mifare_classic_interface_debug_print("\n");
                        
                        res = mifare_classic_basic_value_write((uint8_t)atoi(argv[7]), atoi(argv[8]), (uint8_t)atoi(argv[7]),
                                                               key_type, key);
                        if (res != 0)
                        {
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        mifare_classic_interface_debug_print("mifare_classic: write block%d %d.\n", (uint8_t)atoi(argv[7]), atoi(argv[8]));
                        
                        (void)mifare_classic_basic_deinit();
                        
                        return 0;
                    }
                    else if (strcmp("-increment", argv[6]) == 0)
                    {
                        /* init */
                        res = mifare_classic_basic_init();
                        if (res != 0)
                        {
                            return 1;
                        }
                        /* search */
                        res = mifare_classic_basic_search(&type, id, 50);
                        if (res != 0)
                        {
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        
                        /* ouput the type */
                        if (type == MIFARE_CLASSIC_TYPE_S50)
                        {
                            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
                        }
                        else if (type == MIFARE_CLASSIC_TYPE_S70)
                        {
                            mifare_classic_interface_debug_print("mifare_classic: find S70 card.\n");
                        }
                        else
                        {
                            mifare_classic_interface_debug_print("mifare_classic: invalid type.\n");
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        mifare_classic_interface_debug_print("mifare_classic: id is ");
                        for (i = 0; i < 4; i++)
                        {
                            mifare_classic_interface_debug_print("0x%02X ", id[i]);
                        }
                        mifare_classic_interface_debug_print("\n");
                        
                        res = mifare_classic_basic_value_increment((uint8_t)atoi(argv[7]), (uint32_t)atoi(argv[8]), key_type, key);
                        if (res != 0)
                        {
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        mifare_classic_interface_debug_print("mifare_classic: increment block%d %d.\n", (uint8_t)atoi(argv[7]), (uint32_t)atoi(argv[8]));
                        
                        (void)mifare_classic_basic_deinit();
                        
                        return 0;
                    }
                    else if (strcmp("-decrement", argv[6]) == 0)
                    {
                        /* init */
                        res = mifare_classic_basic_init();
                        if (res != 0)
                        {
                            return 1;
                        }
                        /* search */
                        res = mifare_classic_basic_search(&type, id, 50);
                        if (res != 0)
                        {
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        
                        /* ouput the type */
                        if (type == MIFARE_CLASSIC_TYPE_S50)
                        {
                            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
                        }
                        else if (type == MIFARE_CLASSIC_TYPE_S70)
                        {
                            mifare_classic_interface_debug_print("mifare_classic: find S70 card.\n");
                        }
                        else
                        {
                            mifare_classic_interface_debug_print("mifare_classic: invalid type.\n");
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        mifare_classic_interface_debug_print("mifare_classic: id is ");
                        for (i = 0; i < 4; i++)
                        {
                            mifare_classic_interface_debug_print("0x%02X ", id[i]);
                        }
                        mifare_classic_interface_debug_print("\n");
                        
                        res = mifare_classic_basic_value_decrement((uint8_t)atoi(argv[7]), (uint32_t)atoi(argv[8]), key_type, key);
                        if (res != 0)
                        {
                            (void)mifare_classic_basic_deinit();
                            
                            return 1;
                        }
                        mifare_classic_interface_debug_print("mifare_classic: decrement block%d %d.\n", (uint8_t)atoi(argv[7]), (uint32_t)atoi(argv[8]));
                        
                        (void)mifare_classic_basic_deinit();
                        
                        return 0;
                    }
                    else
                    {
                        return 5;
                    }
                }
                else
                {
                    return 5;
                }
            }
            else
            {
                return 5;
            }
        }
        else
        {
            return 5;
        }
    }
    /* param is invalid */
    else
    {
        return 5;
    }
}

/**
 * @brief main function
 * @note  none
 */
int main(void)
{
    uint8_t res;
    
    /* stm32f407 clock init and hal init */
    clock_init();
    
    /* delay init */
    delay_init();
    
    /* uart1 init */
    uart1_init(115200);
    
    /* shell init && register mifare_classic fuction */
    shell_init();
    shell_register("mifare_classic", mifare_classic);
    uart1_print("mifare_classic: welcome to libdriver mifare_classic.\n");
    
    while (1)
    {
        /* read uart */
        g_len = uart1_read(g_buf, 256);
        if (g_len)
        {
            /* run shell */
            res = shell_parse((char *)g_buf, g_len);
            if (res == 0)
            {
                /* run success */
            }
            else if (res == 1)
            {
                uart1_print("mifare_classic: run failed.\n");
            }
            else if (res == 2)
            {
                uart1_print("mifare_classic: unknow command.\n");
            }
            else if (res == 3)
            {
                uart1_print("mifare_classic: length is too long.\n");
            }
            else if (res == 4)
            {
                uart1_print("mifare_classic: pretreat failed.\n");
            }
            else if (res == 5)
            {
                uart1_print("mifare_classic: param is invalid.\n");
            }
            else
            {
                uart1_print("mifare_classic: unknow status code.\n");
            }
            uart1_flush();
        }
        delay_ms(100);
    }
}
