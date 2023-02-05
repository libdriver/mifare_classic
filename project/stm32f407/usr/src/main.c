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
#include "getopt.h"
#include <math.h>
#include <stdlib.h>

/**
 * @brief global var definition
 */
uint8_t g_buf[256];                        /**< uart buffer */
volatile uint16_t g_len;                   /**< uart buffer length */

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
    int c;
    int longindex = 0;
    const char short_options[] = "hipe:t:";
    const struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"information", no_argument, NULL, 'i'},
        {"port", no_argument, NULL, 'p'},
        {"example", required_argument, NULL, 'e'},
        {"test", required_argument, NULL, 't'},
        {"block", required_argument, NULL, 1},
        {"data", required_argument, NULL, 2},
        {"key", required_argument, NULL, 3},
        {"key-type", required_argument, NULL, 4},
        {"value", required_argument, NULL, 5},
        {NULL, 0, NULL, 0},
    };
    char type[33] = "unknown";
    int32_t value = 0x00000000;
    uint8_t block = 0x00;
    uint8_t data[16] = {0};
    uint8_t key[6] = {0};
    mifare_classic_authentication_key_t key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_A;
    
    /* if no params */
    if (argc == 1)
    {
        /* goto the help */
        goto help;
    }
    
    /* init 0 */
    optind = 0;
    
    /* parse */
    do
    {
        /* parse the args */
        c = getopt_long(argc, argv, short_options, long_options, &longindex);
        
        /* judge the result */
        switch (c)
        {
            /* help */
            case 'h' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "h");
                
                break;
            }
            
            /* information */
            case 'i' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "i");
                
                break;
            }
            
            /* port */
            case 'p' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "p");
                
                break;
            }
            
            /* example */
            case 'e' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "e_%s", optarg);
                
                break;
            }
            
            /* test */
            case 't' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "t_%s", optarg);
                
                break;
            }
            
            /* block */
            case 1 :
            {
                char *p;
                uint16_t l;
                uint16_t i;
                uint64_t hex_data;

                /* set the data */
                l = strlen(optarg);

                /* check the header */
                if (l >= 2)
                {
                    if (strncmp(optarg, "0x", 2) == 0)
                    {
                        p = optarg + 2;
                        l -= 2;
                    }
                    else if (strncmp(optarg, "0X", 2) == 0)
                    {
                        p = optarg + 2;
                        l -= 2;
                    }
                    else
                    {
                        p = optarg;
                    }
                }
                else
                {
                    p = optarg;
                }
                
                /* init 0 */
                hex_data = 0;

                /* loop */
                for (i = 0; i < l; i++)
                {
                    if ((p[i] <= '9') && (p[i] >= '0'))
                    {
                        hex_data += (p[i] - '0') * (uint32_t)pow(16, l - i - 1);
                    }
                    else if ((p[i] <= 'F') && (p[i] >= 'A'))
                    {
                        hex_data += ((p[i] - 'A') + 10) * (uint32_t)pow(16, l - i - 1);
                    }
                    else if ((p[i] <= 'f') && (p[i] >= 'a'))
                    {
                        hex_data += ((p[i] - 'a') + 10) * (uint32_t)pow(16, l - i - 1);
                    }
                    else
                    {
                        return 5;
                    }
                }
                
                /* set the block */
                block = hex_data & 0xFF;
                
                break;
            }
            
            /* data */
            case 2 :
            {
                uint8_t size;
                uint8_t i;
                char *p;
                
                /* get the size */
                size = (uint8_t)strlen(optarg);
                
                /* check the size */
                if ((size == 32) || (size == 34))
                {
                    /* check ok */
                }
                else
                {
                    return 5;
                }

                /* check the size */
                if (strncmp(optarg, "0x", 2) == 0)
                {
                    p = optarg + 2;
                    size -= 2;
                }
                else if (strncmp(optarg, "0X", 2) == 0)
                {
                    p = optarg + 2;
                    size -= 2;
                }
                else
                {
                    p = optarg;
                }
                
                /* check the size */
                if (size != 32)
                {
                    return 5;
                }

                /* set the data */
                for (i = 0; i < size; i += 2)
                {
                    if (('A' <= p[i]) && (p[i] <= 'Z'))
                    {
                        data[i / 2] = p[i] - 'A' + 10;
                        data[i / 2] *= 16;
                    }
                    else if (('a' <= p[i]) && (p[i] <= 'z'))
                    {
                        data[i / 2] = p[i] - 'a' + 10;
                        data[i / 2] *= 16;
                    }
                    else if (('0' <= p[i]) && (p[i] <= '9'))
                    {
                        data[i / 2] = p[i] - '0';
                        data[i / 2] *= 16;
                    }
                    else
                    {
                        return 5;
                    }
                    if (('A' <= p[i + 1]) && (p[i + 1] <= 'Z'))
                    {
                        data[i / 2 ] += p[i + 1] - 'A' + 10;
                    }
                    else if (('a' <= p[i + 1]) && (p[i + 1] <= 'z'))
                    {
                        data[i / 2 ] += p[i + 1] - 'a' + 10;
                    }
                    else if (('0' <= p[i + 1]) && (p[i + 1] <= '9'))
                    {
                        data[i / 2 ] += p[i + 1] - '0';
                    }
                    else
                    {
                        return 5;
                    }
                }

                break;
            }
            
            /* key */
            case 3 :
            {
                uint8_t size;
                uint8_t i;
                char *p;
                
                /* get the size */
                size = (uint8_t)strlen(optarg);
                
                /* check the size */
                if ((size == 12) || (size == 14))
                {
                    /* check ok */
                }
                else
                {
                    return 5;
                }

                /* check the size */
                if (strncmp(optarg, "0x", 2) == 0)
                {
                    p = optarg + 2;
                    size -= 2;
                }
                else if (strncmp(optarg, "0X", 2) == 0)
                {
                    p = optarg + 2;
                    size -= 2;
                }
                else
                {
                    p = optarg;
                }
                
                /* check the size */
                if (size != 12)
                {
                    return 5;
                }

                /* copy data */
                for (i = 0; i < size; i += 2)
                {
                    if (('A' <= p[i]) && (p[i] <= 'Z'))
                    {
                        key[i / 2] = p[i] - 'A' + 10;
                        key[i / 2] *= 16;
                    }
                    else if (('a' <= p[i]) && (p[i] <= 'z'))
                    {
                        key[i / 2] = p[i] - 'a' + 10;
                        key[i / 2] *= 16;
                    }
                    else if (('0' <= p[i]) && (p[i] <= '9'))
                    {
                        key[i / 2] = p[i] - '0';
                        key[i / 2] *= 16;
                    }
                    else
                    {
                        return 5;
                    }
                    if (('A' <= p[i + 1]) && (p[i + 1] <= 'Z'))
                    {
                        key[i / 2 ] += p[i + 1] - 'A' + 10;
                    }
                    else if (('a' <= p[i + 1]) && (p[i + 1] <= 'z'))
                    {
                        key[i / 2 ] += p[i + 1] - 'a' + 10;
                    }
                    else if (('0' <= p[i + 1]) && (p[i + 1] <= '9'))
                    {
                        key[i / 2 ] += p[i + 1] - '0';
                    }
                    else
                    {
                        return 5;
                    }
                }

                break;
            }
             
            /* key-type */
            case 4 :
            {
                /* set the key type */
                if (strcmp("A", optarg) == 0)
                {
                    key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_A;
                }
                else if (strcmp("B", optarg) == 0)
                {
                    key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_B;
                }
                else
                {
                    return 5;
                }

                break;
            }
            
            /* value */
            case 5 :
            {
                /* set the value */
                value = atol(optarg);

                break;
            }
            
            /* the end */
            case -1 :
            {
                break;
            }
            
            /* others */
            default :
            {
                return 5;
            }
        }
    } while (c != -1);

    /* run the function */
    if (strcmp("t_card", type) == 0)
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
    else if (strcmp("e_halt", type) == 0)
    {
        uint8_t res;
        
        /* basic init */
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

        /* output */
        mifare_classic_interface_debug_print("mifare_classic: halt.\n");
        
        /* basic deinit */
        (void)mifare_classic_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_wake-up", type) == 0)
    {
        uint8_t res;
        
        /* basic init */
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

        /* output */
        mifare_classic_interface_debug_print("mifare_classic: wake up.\n");
        
        /* basic deinit */
        (void)mifare_classic_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_read", type) == 0)
    {
        mifare_classic_type_t chip_type; 
        uint8_t res;
        uint8_t i;
        uint8_t id[4];
        
        /* basic init */
        res = mifare_classic_basic_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* search */
        res = mifare_classic_basic_search(&chip_type, id, 50);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        
        /* output */
        if (chip_type == MIFARE_CLASSIC_TYPE_S50)
        {
            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
        }
        else if (chip_type == MIFARE_CLASSIC_TYPE_S70)
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
        res = mifare_classic_basic_read(block, data, key_type, key);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        
        /* output */
        mifare_classic_interface_debug_print("mifare_classic: block is ");
        for (i = 0; i < 16; i++)
        {
            mifare_classic_interface_debug_print("0x%02X ", data[i]);
        }
        mifare_classic_interface_debug_print("\n");
        
        /* basic deinit */
        (void)mifare_classic_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_write", type) == 0)
    {
        mifare_classic_type_t chip_type; 
        uint8_t res;
        uint8_t i;
        uint8_t id[4];

        /* basic init */
        res = mifare_classic_basic_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* search */
        res = mifare_classic_basic_search(&chip_type, id, 50);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        
        /* output */
        if (chip_type == MIFARE_CLASSIC_TYPE_S50)
        {
            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
        }
        else if (chip_type == MIFARE_CLASSIC_TYPE_S70)
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
        
        /* output */
        mifare_classic_interface_debug_print("mifare_classic: data is ");
        for (i = 0; i < 16; i++)
        {
            mifare_classic_interface_debug_print("0x%02X ", data[i]);
        }
        mifare_classic_interface_debug_print("\n");
        
        /* write */
        res = mifare_classic_basic_write(block, data, key_type, key);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        
        /* basic deinit */
        (void)mifare_classic_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_value-init", type) == 0)
    {
        mifare_classic_type_t chip_type; 
        uint8_t res;
        uint8_t i;
        uint8_t id[4];

        /* basic init */
        res = mifare_classic_basic_init();
        if (res != 0)
        {
            return 1;
        }

        /* search */
        res = mifare_classic_basic_search(&chip_type, id, 50);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        
        /* output */
        if (chip_type == MIFARE_CLASSIC_TYPE_S50)
        {
            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
        }
        else if (chip_type == MIFARE_CLASSIC_TYPE_S70)
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
        
        /* value init */
        res = mifare_classic_basic_value_init(block, value, block, key_type, key);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        mifare_classic_interface_debug_print("mifare_classic: init block%d as value %d.\n", block, value);
        
        /* basic deinit */
        (void)mifare_classic_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_value-write", type) == 0)
    {
        mifare_classic_type_t chip_type; 
        uint8_t res;
        uint8_t i;
        uint8_t id[4];

        /* basic init */
        res = mifare_classic_basic_init();
        if (res != 0)
        {
            return 1;
        }

        /* search */
        res = mifare_classic_basic_search(&chip_type, id, 50);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        
        /* output */
        if (chip_type == MIFARE_CLASSIC_TYPE_S50)
        {
            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
        }
        else if (chip_type == MIFARE_CLASSIC_TYPE_S70)
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
        
        /* value write */
        res = mifare_classic_basic_value_write(block, value, block, key_type, key);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        mifare_classic_interface_debug_print("mifare_classic: write block%d %d.\n", block, value);
        
        /* basic deinit */
        (void)mifare_classic_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_value-read", type) == 0)
    {
        mifare_classic_type_t chip_type; 
        uint8_t addr;
        uint8_t res;
        uint8_t i;
        uint8_t id[4];
        int32_t value;

        /* basic init */
        res = mifare_classic_basic_init();
        if (res != 0)
        {
            return 1;
        }

        /* search */
        res = mifare_classic_basic_search(&chip_type, id, 50);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        
        /* output */
        if (chip_type == MIFARE_CLASSIC_TYPE_S50)
        {
            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
        }
        else if (chip_type == MIFARE_CLASSIC_TYPE_S70)
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
        
        /* read value */
        res = mifare_classic_basic_value_read(block, &value, &addr, key_type, key);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        mifare_classic_interface_debug_print("mifare_classic: read block%d value %d.\n", block, value);
        
        /* basic deinit */
        (void)mifare_classic_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_value-increment", type) == 0)
    {
        mifare_classic_type_t chip_type; 
        uint8_t res;
        uint8_t i;
        uint8_t id[4];

        /* basic init */
        res = mifare_classic_basic_init();
        if (res != 0)
        {
            return 1;
        }

        /* search */
        res = mifare_classic_basic_search(&chip_type, id, 50);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        
        /* output */
        if (chip_type == MIFARE_CLASSIC_TYPE_S50)
        {
            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
        }
        else if (chip_type == MIFARE_CLASSIC_TYPE_S70)
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
        
        /* value increment */
        res = mifare_classic_basic_value_increment(block, value, key_type, key);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        mifare_classic_interface_debug_print("mifare_classic: increment block%d %d.\n", block, value);
        
        /* basic deinit */
        (void)mifare_classic_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_value-decrement", type) == 0)
    {
        mifare_classic_type_t chip_type; 
        uint8_t res;
        uint8_t i;
        uint8_t id[4];

        /* basic init */
        res = mifare_classic_basic_init();
        if (res != 0)
        {
            return 1;
        }

        /* search */
        res = mifare_classic_basic_search(&chip_type, id, 50);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }
        
        /* output */
        if (chip_type == MIFARE_CLASSIC_TYPE_S50)
        {
            mifare_classic_interface_debug_print("mifare_classic: find S50 card.\n");
        }
        else if (chip_type == MIFARE_CLASSIC_TYPE_S70)
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
        
        /* value decrement */
        res = mifare_classic_basic_value_decrement(block, value, key_type, key);
        if (res != 0)
        {
            (void)mifare_classic_basic_deinit();
            
            return 1;
        }

        /* output */
        mifare_classic_interface_debug_print("mifare_classic: decrement block%d %d.\n", block, value);
        
        /* basic deint */
        (void)mifare_classic_basic_deinit();
        
        return 0;
    }
    else if (strcmp("h", type) == 0)
    {
        help:
        mifare_classic_interface_debug_print("Usage:\n");
        mifare_classic_interface_debug_print("  mifare_classic (-i | --information)\n");
        mifare_classic_interface_debug_print("  mifare_classic (-h | --help)\n");
        mifare_classic_interface_debug_print("  mifare_classic (-p | --port)\n");
        mifare_classic_interface_debug_print("  mifare_classic (-t card | --test=card)\n");
        mifare_classic_interface_debug_print("  mifare_classic (-e halt | --example=halt)\n");
        mifare_classic_interface_debug_print("  mifare_classic (-e wake-up | --example=wake-up)\n");
        mifare_classic_interface_debug_print("  mifare_classic (-e read | --example=read) [--key-type=<A | B>] [--key=<authentication>]\n");
        mifare_classic_interface_debug_print("                 [--block=<addr>]\n");
        mifare_classic_interface_debug_print("  mifare_classic (-e write | --example=write) [--key-type=<A | B>] [--key=<authentication>]\n");
        mifare_classic_interface_debug_print("                 [--block=<addr>] [--data=<hex>]\n");
        mifare_classic_interface_debug_print("  mifare_classic (-e value-init | --example=value-init) [--key-type=<A | B>] [--key=<authentication>]\n");
        mifare_classic_interface_debug_print("                 [--block=<addr>] [--value=<dec>]\n");
        mifare_classic_interface_debug_print("  mifare_classic (-e value-write | --example=value-write) [--key-type=<A | B>] [--key=<authentication>]\n");
        mifare_classic_interface_debug_print("                 [--block=<addr>] [--value=<dec>]\n");
        mifare_classic_interface_debug_print("  mifare_classic (-e value-read | --example=value-read) [--key-type=<A | B>] [--key=<authentication>]\n");
        mifare_classic_interface_debug_print("                 [--block=<addr>]\n");
        mifare_classic_interface_debug_print("  mifare_classic (-e value-increment | --example=value-increment) [--key-type=<A | B>] [--key=<authentication>]\n");
        mifare_classic_interface_debug_print("                 [--block=<addr>] [--value=<dec>]\n");
        mifare_classic_interface_debug_print("  mifare_classic (-e value-decrement | --example=value-decrement) [--key-type=<A | B>] [--key=<authentication>]\n");
        mifare_classic_interface_debug_print("                 [--block=<addr>] [--value=<dec>]\n");
        mifare_classic_interface_debug_print("\n");
        mifare_classic_interface_debug_print("Options:\n");
        mifare_classic_interface_debug_print("      --block=<addr>            Set the block address and it is hexadecimal.([default: 0x00])\n");
        mifare_classic_interface_debug_print("      --data=<hex>              Set the input data and it is hexadecimal with 16 bytes(strlen=32).([default: 0x0123456789ABCDEF0123456789ABCDEF])\n");
        mifare_classic_interface_debug_print("  -e <halt | wake-up | read | write | value-init | value-write | value-read | value-increment\n");
        mifare_classic_interface_debug_print("     | value-decrement>, --example=<halt | wake-up | read | write | value-init | value-write\n");
        mifare_classic_interface_debug_print("     | value-read | value-increment | value-decrement>\n");
        mifare_classic_interface_debug_print("                                Run the driver example.\n");
        mifare_classic_interface_debug_print("  -h, --help                    Show the help.\n");
        mifare_classic_interface_debug_print("  -i, --information             Show the chip information.\n");
        mifare_classic_interface_debug_print("      --key=<authentication>    Set the key of authentication and it is hexadecimal with 6 bytes(strlen=12).([default: 0xFFFFFFFFFFFF])\n");
        mifare_classic_interface_debug_print("      --key-type=<A | B>        Set the key type of authentication.([default: A])\n");
        mifare_classic_interface_debug_print("  -p, --port                    Display the pin connections of the current board.\n");
        mifare_classic_interface_debug_print("  -t <card>, --test=<card>      Run the driver test.\n");
        mifare_classic_interface_debug_print("      --value=<dec>             Set the input value.([default: 0])\n");

        return 0;
    }
    else if (strcmp("i", type) == 0)
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
    else if (strcmp("p", type) == 0)
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
    
    /* uart init */
    uart_init(115200);
    
    /* shell init && register mifare_classic function */
    shell_init();
    shell_register("mifare_classic", mifare_classic);
    uart_print("mifare_classic: welcome to libdriver mifare_classic.\n");
    
    while (1)
    {
        /* read uart */
        g_len = uart_read(g_buf, 256);
        if (g_len != 0)
        {
            /* run shell */
            res = shell_parse((char *)g_buf, g_len);
            if (res == 0)
            {
                /* run success */
            }
            else if (res == 1)
            {
                uart_print("mifare_classic: run failed.\n");
            }
            else if (res == 2)
            {
                uart_print("mifare_classic: unknown command.\n");
            }
            else if (res == 3)
            {
                uart_print("mifare_classic: length is too long.\n");
            }
            else if (res == 4)
            {
                uart_print("mifare_classic: pretreat failed.\n");
            }
            else if (res == 5)
            {
                uart_print("mifare_classic: param is invalid.\n");
            }
            else
            {
                uart_print("mifare_classic: unknown status code.\n");
            }
            uart_flush();
        }
        delay_ms(100);
    }
}
