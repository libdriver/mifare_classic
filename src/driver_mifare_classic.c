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
 * @file      driver_mifare_classic.c
 * @brief     driver mifare classic source file
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

#include "driver_mifare_classic.h"

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "NXP MIFARE Classic EV1"        /**< chip name */
#define MANUFACTURER_NAME         "NXP"                           /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        3.3f                            /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        4.0f                            /**< chip max supply voltage */
#define MAX_CURRENT               30.0f                           /**< chip max current */
#define TEMPERATURE_MIN           -25.0f                          /**< chip min operating temperature */
#define TEMPERATURE_MAX           70.0f                           /**< chip max operating temperature */
#define DRIVER_VERSION            1000                            /**< driver version */

/**
 * @brief chip command definition
 */
#define MIFARE_CLASSIC_COMMAND_REQUEST                          0x26           /**< request command */
#define MIFARE_CLASSIC_COMMAND_WAKE_UP                          0x52           /**< wake up command */
#define MIFARE_CLASSIC_COMMAND_ANTICOLLISION_CL1                0x9320U        /**< anti collision cl1 command */
#define MIFARE_CLASSIC_COMMAND_SELECT_CL1                       0x9370U        /**< select cl1 command */
#define MIFARE_CLASSIC_COMMAND_ANTICOLLISION_CL2                0x9520U        /**< anti collision cl2 command */
#define MIFARE_CLASSIC_COMMAND_SELECT_CL2                       0x9570U        /**< select cl2 command */
#define MIFARE_CLASSIC_COMMAND_HALT                             0x5000U        /**< halt command */
#define MIFARE_CLASSIC_COMMAND_AUTHENTICATION_WITH_KEY_A        0x60           /**< authentication with key a command */
#define MIFARE_CLASSIC_COMMAND_AUTHENTICATION_WITH_KEY_B        0x61           /**< authentication with key b command */
#define MIFARE_CLASSIC_COMMAND_PERSONALIZE_UID_USAGE            0x40           /**< personalize uid usage command */
#define MIFARE_CLASSIC_COMMAND_SET_MOD_TYPE                     0x43           /**< set mod type command */
#define MIFARE_CLASSIC_COMMAND_MIFARE_READ                      0x30           /**< read command */
#define MIFARE_CLASSIC_COMMAND_MIFARE_WRITE                     0xA0           /**< write command */
#define MIFARE_CLASSIC_COMMAND_MIFARE_DECREMENT                 0xC0           /**< decrement command */
#define MIFARE_CLASSIC_COMMAND_MIFARE_INCREMENT                 0xC1           /**< increment command */
#define MIFARE_CLASSIC_COMMAND_MIFARE_RESTORE                   0xC2           /**< restore command */
#define MIFARE_CLASSIC_COMMAND_MIFARE_TRANSFER                  0xB0           /**< transfer command */

/**
 * @brief     crc calculation
 * @param[in] *p points to a data buffer
 * @param[in] len is the data length
 * @note      none
 */
static void a_mifare_classic_iso14443a_crc(uint8_t *p, uint8_t len, uint8_t output[2])
{
    uint32_t w_crc = 0x6363;
    
    do 
    {
        uint8_t  bt;
        
        bt = *p++;                                                                                        /* get one byte */
        bt = (bt ^ (uint8_t)(w_crc & 0x00FF));                                                            /* xor */
        bt = (bt ^ (bt << 4));                                                                            /* xor */
        w_crc = (w_crc >> 8) ^ ((uint32_t) bt << 8) ^ ((uint32_t) bt << 3) ^ ((uint32_t) bt >> 4);        /* get the crc */
    } while (--len);                                                                                      /* len-- */

    output[0] = (uint8_t)(w_crc & 0xFF);                                                                  /* lsb */
    output[1] = (uint8_t)((w_crc >> 8) & 0xFF);                                                           /* msb */
}

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
uint8_t mifare_classic_init(mifare_classic_handle_t *handle)
{
    uint8_t res;
    
    if (handle == NULL)                                                                   /* check handle */
    {
        return 2;                                                                         /* return error */
    }
    if (handle->debug_print == NULL)                                                      /* check debug_print */
    {
        return 3;                                                                         /* return error */
    }
    if (handle->contactless_init == NULL)                                                 /* check contactless_init */
    {
        handle->debug_print("mifare_classic: contactless_init is null.\n");               /* contactless_init is null */
        
        return 3;                                                                         /* return error */
    }
    if (handle->contactless_deinit == NULL)                                               /* check contactless_deinit */
    {
        handle->debug_print("mifare_classic: contactless_deinit is null.\n");             /* contactless_deinit is null */
        
        return 3;                                                                         /* return error */
    }
    if (handle->contactless_transceiver == NULL)                                          /* check contactless_transceiver */
    {
        handle->debug_print("mifare_classic: contactless_transceiver is null.\n");        /* contactless_transceiver is null */
        
        return 3;                                                                         /* return error */
    }
    if (handle->delay_ms == NULL)                                                         /* check delay_ms */
    {
        handle->debug_print("mifare_classic: delay_ms is null.\n");                       /* delay_ms is null */
        
        return 3;                                                                         /* return error */
    }
    
    res = handle->contactless_init();                                                     /* contactless init */
    if (res != 0)                                                                         /* check the result */
    {
        handle->debug_print("mifare_classic: contactless init failed.\n");                /* contactless init failed */
        
        return 1;                                                                         /* return error */
    }
    handle->type = MIFARE_CLASSIC_TYPE_INVALID;                                           /* set the invalid type */
    handle->inited = 1;                                                                   /* flag inited */
    
    return 0;                                                                             /* success return 0 */
}

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
uint8_t mifare_classic_deinit(mifare_classic_handle_t *handle)
{
    uint8_t res;
    
    if (handle == NULL)                                                             /* check handle */
    {
        return 2;                                                                   /* return error */
    }
    if (handle->inited != 1)                                                        /* check handle initialization */
    {
        return 3;                                                                   /* return error */
    }
    
    res = handle->contactless_deinit();                                             /* contactless deinit */
    if (res != 0)                                                                   /* check the result */
    {
        handle->debug_print("mifare_classic: contactless deinit failed.\n");        /* contactless deinit failed */
        
        return 1;                                                                   /* return error */
    }
    handle->inited = 0;                                                             /* flag closed */
    
    return 0;                                                                       /* success return 0 */
}

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
uint8_t mifare_classic_request(mifare_classic_handle_t *handle, mifare_classic_type_t *type)
{
    uint8_t res;
    uint8_t input_len;
    uint8_t input_buf[1];
    uint8_t output_len;
    uint8_t output_buf[2];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 1;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_REQUEST;                                               /* set the command */
    output_len = 2;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 2)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if ((output_buf[0] == 0x04) && (output_buf[1] == 0x00))                                      /* check classic type */
    {
        *type = MIFARE_CLASSIC_TYPE_S50;                                                         /* s50 */
        handle->type = *type;                                                                    /* save the type */
        
        return 0;                                                                                /* success return 0 */
    }
    else if ((output_buf[0] == 0x02) && (output_buf[1] == 0x00))                                 /* check classic type */
    {
        *type = MIFARE_CLASSIC_TYPE_S70;                                                         /* s70 */
        handle->type = *type;                                                                    /* save the type */
        
        return 0;                                                                                /* success return 0 */
    }
    else
    {
        *type = MIFARE_CLASSIC_TYPE_INVALID;                                                     /* invalid */
        handle->type = *type;                                                                    /* save the type */
        handle->debug_print("mifare_classic: type is invalid.\n");                               /* type is invalid */
        
        return 5;                                                                                /* return error */
    }
}

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
uint8_t mifare_classic_wake_up(mifare_classic_handle_t *handle, mifare_classic_type_t *type)
{
    uint8_t res;
    uint8_t input_len;
    uint8_t input_buf[1];
    uint8_t output_len;
    uint8_t output_buf[2];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 1;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_WAKE_UP;                                               /* set the command */
    output_len = 2;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 2)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if ((output_buf[0] == 0x04) && (output_buf[1] == 0x00))                                      /* check classic type */
    {
        *type = MIFARE_CLASSIC_TYPE_S50;                                                         /* s50 */
        handle->type = *type;                                                                    /* save the type */
        
        return 0;                                                                                /* success return 0 */
    }
    else if ((output_buf[0] == 0x02) && (output_buf[1] == 0x00))                                 /* check classic type */
    {
        *type = MIFARE_CLASSIC_TYPE_S70;                                                         /* s70 */
        handle->type = *type;                                                                    /* save the type */
        
        return 0;                                                                                /* success return 0 */
    }
    else
    {
        *type = MIFARE_CLASSIC_TYPE_INVALID;                                                     /* invalid */
        handle->type = *type;                                                                    /* save the type */
        handle->debug_print("mifare_classic: type is invalid.\n");                               /* type is invalid */
        
        return 5;                                                                                /* return error */
    }
}

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
uint8_t mifare_classic_halt(mifare_classic_handle_t *handle)
{
    uint8_t input_len;
    uint8_t input_buf[4];
    uint8_t output_len;
    uint8_t output_buf[1];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = (MIFARE_CLASSIC_COMMAND_HALT >> 8) & 0xFF;                                    /* set the command */
    input_buf[1] = (MIFARE_CLASSIC_COMMAND_HALT >> 0) & 0xFF;                                    /* set the command */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    (void)handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    
    return 0;                                                                                    /* success return 0 */
    
}

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
uint8_t mifare_classic_set_modulation(mifare_classic_handle_t *handle, mifare_classic_load_modulation_t mod)
{
    uint8_t res;
    uint8_t input_len;
    uint8_t input_buf[4];
    uint8_t output_len;
    uint8_t output_buf[1];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_SET_MOD_TYPE;                                          /* set the command */
    input_buf[1] = mod;                                                                          /* set the mod */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if (output_buf[0] == 0xA)                                                                    /* check the result */
    {
        return 0;                                                                                /* success return 0 */
    }
    else
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
}

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
uint8_t mifare_classic_set_personalized_uid(mifare_classic_handle_t *handle, mifare_classic_personalized_uid_t type)
{
    uint8_t res;
    uint8_t input_len;
    uint8_t input_buf[4];
    uint8_t output_len;
    uint8_t output_buf[1];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_PERSONALIZE_UID_USAGE;                                          /* set the command */
    input_buf[1] = type;                                                                          /* set the mod */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if (output_buf[0] == 0xA)                                                                    /* check the result */
    {
        return 0;                                                                                /* success return 0 */
    }
    else
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
}

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
uint8_t mifare_classic_anticollision_cl1(mifare_classic_handle_t *handle, uint8_t id[4])
{
    uint8_t res;
    uint8_t i;
    uint8_t check;
    uint8_t input_len;
    uint8_t input_buf[2];
    uint8_t output_len;
    uint8_t output_buf[5];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 2;                                                                               /* set the input length */
    input_buf[0] = (MIFARE_CLASSIC_COMMAND_ANTICOLLISION_CL1 >> 8) & 0xFF;                       /* set the command */
    input_buf[1] = (MIFARE_CLASSIC_COMMAND_ANTICOLLISION_CL1 >> 0) & 0xFF;                       /* set the command */
    output_len = 5;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 5)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    check = 0;                                                                                   /* init 0 */
    for (i = 0; i < 4; i++)                                                                      /* run 4 times */
    {
        id[i] = output_buf[i];                                                                   /* get one id */
        check ^= output_buf[i];                                                                  /* xor */
    }
    if (check != output_buf[4])                                                                  /* check the result */
    {
        handle->debug_print("mifare_classic: check error.\n");                                   /* check error */
        
        return 5;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t mifare_classic_anticollision_cl2(mifare_classic_handle_t *handle, uint8_t id[4])
{
    uint8_t res;
    uint8_t i;
    uint8_t check;
    uint8_t input_len;
    uint8_t input_buf[2];
    uint8_t output_len;
    uint8_t output_buf[5];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 2;                                                                               /* set the input length */
    input_buf[0] = (MIFARE_CLASSIC_COMMAND_ANTICOLLISION_CL2 >> 8) & 0xFF;                       /* set the command */
    input_buf[1] = (MIFARE_CLASSIC_COMMAND_ANTICOLLISION_CL2 >> 0) & 0xFF;                       /* set the command */
    output_len = 5;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 5)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    check = 0;                                                                                   /* init 0 */
    for (i = 0; i < 4; i++)                                                                      /* run 4 times */
    {
        id[i] = output_buf[i];                                                                   /* get one id */
        check ^= output_buf[i];                                                                  /* xor */
    }
    if (check != output_buf[4])                                                                  /* check the result */
    {
        handle->debug_print("mifare_classic: check error.\n");                                   /* check error */
        
        return 5;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t mifare_classic_select_cl1(mifare_classic_handle_t *handle, uint8_t id[4])
{
    uint8_t res;
    uint8_t i;
    uint8_t input_len;
    uint8_t input_buf[9];
    uint8_t output_len;
    uint8_t output_buf[1];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 9;                                                                               /* set the input length */
    input_buf[0] = (MIFARE_CLASSIC_COMMAND_SELECT_CL1 >> 8) & 0xFF;                              /* set the command */
    input_buf[1] = (MIFARE_CLASSIC_COMMAND_SELECT_CL1 >> 0) & 0xFF;                              /* set the command */
    input_buf[6] = 0;                                                                            /* init 0 */
    for (i = 0; i < 4; i++)                                                                      /* run 4 times */
    {
        input_buf[2 + i] = id[i];                                                                /* get one id */
        input_buf[6] ^= id[i];                                                                   /* xor */
    }
    a_mifare_classic_iso14443a_crc(input_buf, 7, input_buf + 7);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if ((output_buf[0] == 0x08) || (output_buf[0] == 0x18))                                      /* check the sak */
    {
        return 0;                                                                                /* success return 0 */
    }
    else
    {
        handle->debug_print("mifare_classic: sak error.\n");                                     /* sak error */
        
        return 5;                                                                                /* return error */
    }
}

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
uint8_t mifare_classic_select_cl2(mifare_classic_handle_t *handle, uint8_t id[4])
{
    uint8_t res;
    uint8_t i;
    uint8_t input_len;
    uint8_t input_buf[9];
    uint8_t output_len;
    uint8_t output_buf[1];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 9;                                                                               /* set the input length */
    input_buf[0] = (MIFARE_CLASSIC_COMMAND_SELECT_CL2 >> 8) & 0xFF;                              /* set the command */
    input_buf[1] = (MIFARE_CLASSIC_COMMAND_SELECT_CL2 >> 0) & 0xFF;                              /* set the command */
    input_buf[6] = 0;                                                                            /* init 0 */
    for (i = 0; i < 4; i++)                                                                      /* run 4 times */
    {
        input_buf[2 + i] = id[i];                                                                /* get one id */
        input_buf[6] ^= id[i];                                                                   /* xor */
    }
    a_mifare_classic_iso14443a_crc(input_buf, 7, input_buf + 7);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if ((output_buf[0] == 0x08) || (output_buf[0] == 0x18))                                      /* check the sak */
    {
        return 0;                                                                                /* success return 0 */
    }
    else
    {
        handle->debug_print("mifare_classic: sak error.\n");                                     /* sak error */
        
        return 5;                                                                                /* return error */
    }
}

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
                                      mifare_classic_authentication_key_t key_type, uint8_t key[6])
{
    uint8_t res;
    uint8_t i;
    uint8_t input_len;
    uint8_t input_buf[12];
    uint8_t output_len;
    uint8_t output_buf[1];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 12;                                                                              /* set the input length */
    if (key_type == MIFARE_CLASSIC_AUTHENTICATION_KEY_A)                                         /* key a */
    {
        input_buf[0] = MIFARE_CLASSIC_COMMAND_AUTHENTICATION_WITH_KEY_A;                         /* set the command */
    }
    else                                                                                         /* key b */
    {
        input_buf[0] = MIFARE_CLASSIC_COMMAND_AUTHENTICATION_WITH_KEY_B;                         /* set the command */
    }
    input_buf[1] = block;                                                                        /* set the block */
    for (i = 0; i < 6; i++)                                                                      /* 6 times */
    {
        input_buf[2 + i] = key[i];                                                               /* copy the keys */
    }
    for (i = 0; i < 4; i++)                                                                      /* 4 times */
    {
        input_buf[8 + i] = id[i];                                                                /* copy the id */
    }
    
    output_len = 0;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: authentication failed.\n");                         /* authentication failed */
        
        return 1;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t mifare_classic_read(mifare_classic_handle_t *handle, uint8_t block, uint8_t data[16])
{
    uint8_t res;
    uint8_t input_len;
    uint8_t input_buf[4];
    uint8_t output_len;
    uint8_t output_buf[18];
    uint8_t crc_buf[2];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_READ;                                           /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf , 2, input_buf + 2);                                /* get the crc */
    output_len = 18;                                                                             /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 18)                                                                        /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    a_mifare_classic_iso14443a_crc(output_buf, 16, crc_buf);                                     /* get the crc */
    if ((output_buf[16] == crc_buf[0]) && (output_buf[17] == crc_buf[1]))                        /* check the crc */
    {
        memcpy(data, output_buf, 16);                                                            /* copy the data */
        
        return 0;                                                                                /* success return 0 */
    }
    else
    {
        handle->debug_print("mifare_classic: crc error.\n");                                     /* crc error */
        
        return 5;                                                                                /* return error */
    }
}

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
uint8_t mifare_classic_write(mifare_classic_handle_t *handle, uint8_t block, uint8_t data[16])
{
    uint8_t res;
    uint8_t i;
    uint8_t input_len;
    uint8_t input_buf[18];
    uint8_t output_len;
    uint8_t output_buf[1];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_WRITE;                                          /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    for (i = 0; i < 16; i ++)                                                                    /* 16 times */
    {
        input_buf[i] = data[i];                                                                  /* copy data */
    }
    a_mifare_classic_iso14443a_crc(input_buf, 16, input_buf + 16);                               /* get the crc */
    input_len = 18;                                                                              /* set the input length */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t mifare_classic_value_init(mifare_classic_handle_t *handle, uint8_t block, int32_t value, uint8_t addr)
{
    uint8_t res;
    uint8_t i;
    uint8_t input_len;
    uint8_t input_buf[18];
    uint8_t output_len;
    uint8_t output_buf[1];
    uint32_t v;
    uint32_t v_r;
    uint8_t data[16];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    v = (uint32_t)(value);                                                                       /* convert the value */
    v_r = (uint32_t)(~value);                                                                    /* revert the value */
    data[0] = (uint8_t)((v >> 0) & 0xFF);                                                        /* set the value */
    data[1] = (uint8_t)((v >> 8) & 0xFF);                                                        /* set the value */
    data[2] = (uint8_t)((v >> 16) & 0xFF);                                                       /* set the value */
    data[3] = (uint8_t)((v >> 24) & 0xFF);                                                       /* set the value */
    data[4] = (uint8_t)((v_r >> 0) & 0xFF);                                                      /* set the value */
    data[5] = (uint8_t)((v_r >> 8) & 0xFF);                                                      /* set the value */
    data[6] = (uint8_t)((v_r >> 16) & 0xFF);                                                     /* set the value */
    data[7] = (uint8_t)((v_r >> 24) & 0xFF);                                                     /* set the value */
    data[8] = (uint8_t)((v >> 0) & 0xFF);                                                        /* set the value */
    data[9] = (uint8_t)((v >> 8) & 0xFF);                                                        /* set the value */
    data[10] = (uint8_t)((v >> 16) & 0xFF);                                                      /* set the value */
    data[11] = (uint8_t)((v >> 24) & 0xFF);                                                      /* set the value */
    data[12] = addr;                                                                             /* set the address */
    data[13] = (uint8_t)(~addr);                                                                 /* set the address */
    data[14] = addr;                                                                             /* set the address */
    data[15] = (uint8_t)(~addr);                                                                 /* set the address */
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_WRITE;                                          /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    for (i = 0; i < 16; i ++)                                                                    /* 16 times */
    {
        input_buf[i] = data[i];                                                                  /* copy data */
    }
    a_mifare_classic_iso14443a_crc(input_buf, 16, input_buf + 16);                               /* get the crc */
    input_len = 18;                                                                              /* set the input length */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t mifare_classic_value_write(mifare_classic_handle_t *handle, uint8_t block, int32_t value, uint8_t addr)
{
    uint8_t res;
    uint8_t i;
    uint8_t input_len;
    uint8_t input_buf[18];
    uint8_t output_len;
    uint8_t output_buf[1];
    uint32_t v;
    uint32_t v_r;
    uint8_t data[16];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    v = (uint32_t)(value);                                                                       /* convert the value */
    v_r = (uint32_t)(~value);                                                                    /* revert the value */
    data[0] = (uint8_t)((v >> 0) & 0xFF);                                                        /* set the value */
    data[1] = (uint8_t)((v >> 8) & 0xFF);                                                        /* set the value */
    data[2] = (uint8_t)((v >> 16) & 0xFF);                                                       /* set the value */
    data[3] = (uint8_t)((v >> 24) & 0xFF);                                                       /* set the value */
    data[4] = (uint8_t)((v_r >> 0) & 0xFF);                                                      /* set the value */
    data[5] = (uint8_t)((v_r >> 8) & 0xFF);                                                      /* set the value */
    data[6] = (uint8_t)((v_r >> 16) & 0xFF);                                                     /* set the value */
    data[7] = (uint8_t)((v_r >> 24) & 0xFF);                                                     /* set the value */
    data[8] = (uint8_t)((v >> 0) & 0xFF);                                                        /* set the value */
    data[9] = (uint8_t)((v >> 8) & 0xFF);                                                        /* set the value */
    data[10] = (uint8_t)((v >> 16) & 0xFF);                                                      /* set the value */
    data[11] = (uint8_t)((v >> 24) & 0xFF);                                                      /* set the value */
    data[12] = addr;                                                                             /* set the address */
    data[13] = (uint8_t)(~addr);                                                                 /* set the address */
    data[14] = addr;                                                                             /* set the address */
    data[15] = (uint8_t)(~addr);                                                                 /* set the address */
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_WRITE;                                          /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    for (i = 0; i < 16; i ++)                                                                    /* 16 times */
    {
        input_buf[i] = data[i];                                                                  /* copy data */
    }
    a_mifare_classic_iso14443a_crc(input_buf, 16, input_buf + 16);                               /* get the crc */
    input_len = 18;                                                                              /* set the input length */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t mifare_classic_value_read(mifare_classic_handle_t *handle, uint8_t block, int32_t *value, uint8_t *addr)
{
    uint8_t res;
    uint8_t input_len;
    uint8_t input_buf[4];
    uint8_t output_len;
    uint8_t output_buf[18];
    uint8_t crc_buf[2];
    uint8_t data[16];
    uint32_t value_0;
    uint32_t value_1;
    uint32_t value_2;
    uint8_t address_0;
    uint8_t address_1;
    uint8_t address_2;
    uint8_t address_3;
    uint32_t v;
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_READ;                                           /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf , 2, input_buf + 2);                                /* get the crc */
    output_len = 18;                                                                             /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 18)                                                                        /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    a_mifare_classic_iso14443a_crc(output_buf, 16, crc_buf);                                     /* get the crc */
    if ((output_buf[16] == crc_buf[0]) && (output_buf[17] == crc_buf[1]))                        /* check the crc */
    {
        uint8_t i;
        
        for (i = 0; i < 16; i++)                                                                 /* 16 times */
        {
            data[i] = output_buf[i];                                                             /* copy the data */
        }
        value_0 = ((uint32_t)data[0] << 0) | ((uint32_t)data[1] << 8) | 
                  ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);                         /* get the value 0 */
        value_1 = ((uint32_t)data[4] << 0) | ((uint32_t)data[5] << 8) | 
                  ((uint32_t)data[6] << 16) | ((uint32_t)data[7] << 24);                         /* get the value 1 */
        value_2 = ((uint32_t)data[8] << 0) | ((uint32_t)data[9] << 8) | 
                  ((uint32_t)data[10] << 16) | ((uint32_t)data[11] << 24);                       /* get the value 2 */
        address_0 = data[12];                                                                    /* get the address 0 */
        address_1 = (uint8_t)(~data[13]);                                                        /* get the address 1 */
        address_2 = data[14];                                                                    /* get the address 2 */
        address_3 = (uint8_t)(~data[15]);                                                        /* get the address 3 */
        
        if ((value_0 != value_2) || (value_0 != (uint32_t)(~value_1)))                           /* check the value */
        {
            handle->debug_print("mifare_classic: value is invalid.\n");                          /* value is invalid */
            
            return 6;                                                                            /* return error */
        }
        if ((address_0 != address_2) ||
            (address_1 != address_3) ||
            (address_0 != (uint8_t)(address_1))                                                  /* check the address */
            )
        {
            handle->debug_print("mifare_classic: block is invalid.\n");                          /* block is invalid */
            
            return 7;                                                                            /* return error */
        }
        v = value_0;                                                                             /* get the value */
        *value = (int32_t)(v);                                                                   /* set the value */
        *addr = address_0;                                                                       /* set the address */
        
        return 0;                                                                                /* success return 0 */
    }
    else
    {
        handle->debug_print("mifare_classic: crc error.\n");                                     /* crc error */
        
        return 5;                                                                                /* return error */
    }
}

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
uint8_t mifare_classic_increment(mifare_classic_handle_t *handle, uint8_t block, uint32_t value)
{
    uint8_t res;
    uint8_t input_len;
    uint8_t input_buf[6];
    uint8_t output_len;
    uint8_t output_buf[1];
    uint32_t v;
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }

    v = value;                                                                                   /* set the value */
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_INCREMENT;                                      /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if (output_buf[0] == 0x4)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: invalid operation.\n");                             /* invalid operation */
        
        return 6;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    input_len = 6;                                                                               /* set the input length */
    input_buf[0] = (v >> 0) & 0xFF;                                                              /* set the data */
    input_buf[1] = (v >> 8) & 0xFF;                                                              /* set the data */
    input_buf[2] = (v >> 16) & 0xFF;                                                             /* set the data */
    input_buf[3] = (v >> 24) & 0xFF;                                                             /* set the data */
    a_mifare_classic_iso14443a_crc(input_buf, 4, input_buf + 4);                                 /* get the crc */
    output_len = 0;                                                                              /* set the output length */
    (void)handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t mifare_classic_decrement(mifare_classic_handle_t *handle, uint8_t block, uint32_t value)
{
    uint8_t res;
    uint8_t input_len;
    uint8_t input_buf[6];
    uint8_t output_len;
    uint8_t output_buf[1];
    uint32_t v;
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }

    v = value;                                                                                   /* set the value */
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_DECREMENT;                                      /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if (output_buf[0] == 0x4)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: invalid operation.\n");                             /* invalid operation */
        
        return 6;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    input_len = 6;                                                                               /* set the input length */
    input_buf[0] = (v >> 0) & 0xFF;                                                              /* set the data */
    input_buf[1] = (v >> 8) & 0xFF;                                                              /* set the data */
    input_buf[2] = (v >> 16) & 0xFF;                                                             /* set the data */
    input_buf[3] = (v >> 24) & 0xFF;                                                             /* set the data */
    a_mifare_classic_iso14443a_crc(input_buf, 4, input_buf + 4);                                 /* get the crc */
    output_len = 0;                                                                              /* set the output length */
    (void)handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t mifare_classic_transfer(mifare_classic_handle_t *handle, uint8_t block)
{
    uint8_t res;
    uint8_t input_len;
    uint8_t input_buf[4];
    uint8_t output_len;
    uint8_t output_buf[1];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_TRANSFER;                                          /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if (output_buf[0] == 0x4)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: invalid operation.\n");                             /* invalid operation */
        
        return 6;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t mifare_classic_restore(mifare_classic_handle_t *handle, uint8_t block)
{
    uint8_t res;
    uint8_t input_len;
    uint8_t input_buf[6];
    uint8_t output_len;
    uint8_t output_buf[1];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_RESTORE;                                        /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if (output_buf[0] == 0x4)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: invalid operation.\n");                             /* invalid operation */
        
        return 6;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    input_len = 6;                                                                               /* set the input length */
    input_buf[0] = 0x00;                                                                         /* set the data */
    input_buf[1] = 0x00;                                                                         /* set the data */
    input_buf[2] = 0x00;                                                                         /* set the data */
    input_buf[3] = 0x00;                                                                         /* set the data */
    a_mifare_classic_iso14443a_crc(input_buf, 4, input_buf + 4);                                 /* get the crc */
    output_len = 0;                                                                              /* set the output length */
    (void)handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t mifare_classic_block_to_sector(mifare_classic_handle_t *handle, uint8_t block, uint8_t *sector)
{
    if (handle == NULL)                                  /* check handle */
    {
        return 2;                                        /* return error */
    }
    if (handle->inited != 1)                             /* check handle initialization */
    {
        return 3;                                        /* return error */
    }
    
    if (block < 32 * 4)                                  /* check the size*/
    {
        *sector = block / 4;                             /* s50 */
    }
    else
    {
        *sector = 32 + ((block - (32 * 4)) / 16);        /* s70 */
    }
    
    return 0;                                            /* success return 0 */
}

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
uint8_t mifare_classic_sector_block_count(mifare_classic_handle_t *handle, uint8_t sector, uint8_t *count)
{
    if (handle == NULL)             /* check handle */
    {
        return 2;                   /* return error */
    }
    if (handle->inited != 1)        /* check handle initialization */
    {
        return 3;                   /* return error */
    }
    
    if (sector < 32)                /* check the size*/
    {
        *count = 4;                 /* s50 */
    }
    else
    {
        *count = 16;                /* s70 */
    }
    
    return 0;                       /* success return 0 */
}

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
uint8_t mifare_classic_sector_first_block(mifare_classic_handle_t *handle, uint8_t sector, uint8_t *block)
{
    if (handle == NULL)                              /* check handle */
    {
        return 2;                                    /* return error */
    }
    if (handle->inited != 1)                         /* check handle initialization */
    {
        return 3;                                    /* return error */
    }
    
    if (sector < 32)                                 /* check the size*/
    {
        *block = sector * 4;                         /* s50 */
    }
    else
    {
        *block = 32 * 4 + (sector - 32) * 16;        /* s70 */
    }
    
    return 0;                                        /* success return 0 */
}

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
uint8_t mifare_classic_sector_last_block(mifare_classic_handle_t *handle, uint8_t sector, uint8_t *block)
{
    if (handle == NULL)                                    /* check handle */
    {
        return 2;                                          /* return error */
    }
    if (handle->inited != 1)                               /* check handle initialization */
    {
        return 3;                                          /* return error */
    }
    
    if (sector < 32)                                       /* check the size*/
    {
        *block = sector * 4;                               /* s50 */
    }
    else
    {
        *block = 32 * 4 + (sector - 32) * 16;              /* s70 */
    }
    *block = *block + ((sector < 32) ? 4 : 16) - 1;        /* get the last block */
    
    return 0;                                              /* success return 0 */
}

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
                                             uint8_t user_data, uint8_t key_b[6])
{
    uint8_t res;
    uint8_t block;
    uint8_t i;
    uint8_t part_1, part_2, part_3;
    uint8_t input_len;
    uint8_t input_buf[18];
    uint8_t output_len;
    uint8_t output_buf[1];
    uint8_t access_bits[4];
    uint8_t data[16];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    part_1 = (((block_3_15 >> 2) & 0x1) << 3) | (((block_2_10_14 >> 2) & 0x1) << 2) |
             (((block_1_5_9 >> 2) & 0x1) << 1) | (((block_0_0_4 >> 2) & 0x1) << 0);              /* set part 1 */
    part_2 = (((block_3_15 >> 1) & 0x1) << 3) | (((block_2_10_14 >> 1) & 0x1) << 2) |
             (((block_1_5_9 >> 1) & 0x1) << 1) | (((block_0_0_4 >> 1) & 0x1) << 0);              /* set part 2 */
    part_3 = (((block_3_15 >> 0) & 0x1) << 3) | (((block_2_10_14 >> 0) & 0x1) << 2) |
             (((block_1_5_9 >> 0) & 0x1) << 1) | (((block_0_0_4 >> 0) & 0x1) << 0);              /* set part 3 */
    access_bits[0] = ((0xF - part_2) << 4) | ((0xF - part_1) << 0);                              /* set the access bits */
    access_bits[1] = ((part_1 & 0xF) << 4) | ((0xF - part_3) << 0);                              /* set the access bits */
    access_bits[2] = ((part_3 & 0xF) << 4) | ((part_2 & 0xF) << 0);                              /* set the access bits */
    access_bits[3] = user_data;                                                                  /* set the user data */
    for (i = 0; i < 6; i++)                                                                      /* 6 times */
    {
        data[i] = key_a[i];                                                                      /* copy the key a */
    }
    for (i = 0; i < 4; i++)                                                                      /* 4 times */
    {
        data[i + 6] = access_bits[i];                                                            /* copy the access bits */
    }
    for (i = 0; i < 6; i++)                                                                      /* 6 times */
    {
        data[i + 10] = key_b[i];                                                                 /* copy the key b */
    }
    
    if (sector < 32)                                                                             /* check the size*/
    {
        block = sector * 4;                                                                      /* s50 */
    }
    else
    {
        block = 32 * 4 + (sector - 32) * 16;                                                     /* s70 */
    }
    block = block + ((sector < 32) ? 4 : 16) - 1;                                                /* get the last block */
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_WRITE;                                          /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf, 2, input_buf + 2);                                 /* get the crc */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 1)                                                                         /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    for (i = 0; i < 16; i ++)                                                                    /* 16 times */
    {
        input_buf[i] = data[i];                                                                  /* copy data */
    }
    a_mifare_classic_iso14443a_crc(input_buf, 16, input_buf + 16);                               /* get the crc */
    input_len = 18;                                                                              /* set the input length */
    output_len = 1;                                                                              /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_buf[0] != 0xA)                                                                    /* check the result */
    {
        handle->debug_print("mifare_classic: ack error.\n");                                     /* ack error */
        
        return 5;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
                                             uint8_t *user_data, uint8_t key_b[6])
{
    uint8_t res;
    uint8_t i;
    uint8_t block;
    uint8_t input_len;
    uint8_t input_buf[4];
    uint8_t output_len;
    uint8_t output_buf[18];
    uint8_t crc_buf[2];
    uint8_t data[16];
    uint8_t access_bits[4];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    if (sector < 32)                                                                             /* check the size*/
    {
        block = sector * 4;                                                                      /* s50 */
    }
    else
    {
        block = 32 * 4 + (sector - 32) * 16;                                                     /* s70 */
    }
    block = block + ((sector < 32) ? 4 : 16) - 1;                                                /* get the last block */
    
    input_len = 4;                                                                               /* set the input length */
    input_buf[0] = MIFARE_CLASSIC_COMMAND_MIFARE_READ;                                           /* set the command */
    input_buf[1] = block;                                                                        /* set the block */
    a_mifare_classic_iso14443a_crc(input_buf , 2, input_buf + 2);                                /* get the crc */
    output_len = 18;                                                                             /* set the output length */
    res = handle->contactless_transceiver(input_buf, input_len, output_buf, &output_len);        /* transceiver */
    if (res != 0)                                                                                /* check the result */
    {
        handle->debug_print("mifare_classic: contactless transceiver failed.\n");                /* contactless transceiver failed */
        
        return 1;                                                                                /* return error */
    }
    if (output_len != 18)                                                                        /* check the output_len */
    {
        handle->debug_print("mifare_classic: output_len is invalid.\n");                         /* output_len is invalid */
        
        return 4;                                                                                /* return error */
    }
    a_mifare_classic_iso14443a_crc(output_buf, 16, crc_buf);                                     /* get the crc */
    if ((output_buf[16] == crc_buf[0]) && (output_buf[17] == crc_buf[1]))                        /* check the crc */
    {
        uint8_t part_1;
        uint8_t part_2;
        uint8_t part_3;
        uint8_t part_1_r;
        uint8_t part_2_r;
        uint8_t part_3_r;
        
        memcpy(data, output_buf, 16);                                                            /* copy the data */
        
        for (i = 0; i < 6; i++)                                                                  /* 6 times */
        {
            key_b[i] = data[10 + i];                                                             /* copy the key b */
        }
        for (i = 0; i < 4; i++)                                                                  /* 4 times */
        {
            access_bits[i] = data[6 + i];                                                        /* copy the access bits */
        }
        part_2_r = (access_bits[0] >> 4) & 0xF;                                                  /* get the part2 revert */
        part_1_r = (access_bits[0] >> 0) & 0xF;                                                  /* get the part1 revert */
        part_1 = (access_bits[1] >> 4) & 0xF;                                                    /* get the part1 */
        part_3_r = (access_bits[1] >> 0) & 0xF;                                                  /* get the part3 revert */
        part_3 = (access_bits[2] >> 4) & 0xF;                                                    /* get the part3 */
        part_2 = (access_bits[2] >> 0) & 0xF;                                                    /* get the part2 */
        if (((part_1 + part_1_r) != 0xF) ||
            ((part_2 + part_2_r) != 0xF) ||
            ((part_3 + part_3_r) != 0xF))                                                        /* check the param */
        {
            handle->debug_print("mifare_classic: data is invalid.\n");                           /* data is invalid */
            
            return 6;                                                                            /* return error */
        }
        *block_0_0_4 = (((part_1 >> 0) & 0x01) << 2) | (((part_2 >> 0) & 0x01) << 1) |
                       (((part_3 >> 0) & 0x01) << 0);                                            /* get the block_0_0_4 */
        *block_1_5_9 = (((part_1 >> 1) & 0x01) << 2) | (((part_2 >> 1) & 0x01) << 1) |
                       (((part_3 >> 1) & 0x01) << 0);                                            /* get the block_1_5_9 */
        *block_2_10_14 = (((part_1 >> 2) & 0x01) << 2) | (((part_2 >> 2) & 0x01) << 1) |
                         (((part_3 >> 2) & 0x01) << 0);                                          /* get the block_2_10_14 */
        *block_3_15 = (((part_1 >> 3) & 0x01) << 2) | (((part_2 >> 3) & 0x01) << 1) |
                      (((part_3 >> 3) & 0x01) << 0);                                             /* get the block_3_15 */
        *user_data = access_bits[3];                                                             /* get the access bits */
        
        return 0;                                                                                /* success return 0 */
    }
    else
    {
        handle->debug_print("mifare_classic: crc error.\n");                                     /* crc error */
        
        return 5;                                                                                /* return error */
    }
}

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
uint8_t mifare_classic_transceiver(mifare_classic_handle_t *handle, uint8_t *in_buf, uint8_t in_len, uint8_t *out_buf, uint8_t *out_len)
{
    if (handle == NULL)                                                /* check handle */
    {
        return 2;                                                      /* return error */
    }
    if (handle->inited != 1)                                           /* check handle initialization */
    {
        return 3;                                                      /* return error */
    }
    
    if (handle->contactless_transceiver(in_buf, in_len, 
                                        out_buf, out_len) != 0)        /* transceiver data */
    {
        return 1;                                                      /* return error */
    }
    else
    {
        return 0;                                                      /* success return 0 */
    }
}

/**
 * @brief      get chip information
 * @param[out] *info points to a mifare_classic info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t mifare_classic_info(mifare_classic_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }
    
    memset(info, 0, sizeof(mifare_classic_info_t));                 /* initialize mifare_classic info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "RF", 8);                              /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */
    
    return 0;                                                       /* success return 0 */
}
