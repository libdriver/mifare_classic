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
 * @file      raspberrypi4b_driver_mifare_classic_interface.c
 * @brief     raspberrypi4b driver mifare_classic interface source file
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

#include "driver_mifare_classic_interface.h"
#include "driver_mfrc522_basic.h"
#include "gpio.h"
#include <unistd.h>
#include <stdarg.h>

uint8_t (*g_gpio_irq)(void) = NULL;        /**< gpio irq function address */

#ifdef USE_DRIVER_MFRC522
/**
 * @brief     interface receive callback
 * @param[in] type irq type
 * @note      none
 */
static void a_receive_callback(uint16_t type)
{
    switch (type)
    {
        case MFRC522_INTERRUPT_MFIN_ACT :
        {
            mfrc522_interface_debug_print("mfrc522: irq mfin act.\n");
            
            break;
        }
        case MFRC522_INTERRUPT_CRC :
        {
            break;
        }
        case MFRC522_INTERRUPT_TX :
        {
            break;
        }
        case MFRC522_INTERRUPT_RX :
        {
            break;
        }
        case MFRC522_INTERRUPT_IDLE :
        {
            break;
        }
        case MFRC522_INTERRUPT_HI_ALERT :
        {
            break;
        }
        case MFRC522_INTERRUPT_LO_ALERT :
        {
            break;
        }
        case MFRC522_INTERRUPT_ERR :
        {
            mfrc522_interface_debug_print("mfrc522: irq err.\n");
            
            break;
        }
        case MFRC522_INTERRUPT_TIMER :
        {
            break;
        }
        default :
        {
            mfrc522_interface_debug_print("mfrc522: irq unknown code.\n");
            
            break;
        }
    }
}
#else
    #error "mifare_classic no driver"
#endif

/**
 * @brief  interface contactless init
 * @return status code
 *         - 0 success
 *         - 1 contactless init failed
 * @note   none
 */
uint8_t mifare_classic_interface_contactless_init(void)
{
    if (gpio_interrupt_init() != 0)
    {
        return 1;
    }
    g_gpio_irq = mfrc522_interrupt_irq_handler;

#ifdef USE_DRIVER_MFRC522
    if (mfrc522_basic_init(MFRC522_INTERFACE_SPI, 0x00, a_receive_callback) != 0)
    {
        return 1;
    }
#else
    #error "mifare_classic no driver"
#endif

    return 0;
}

/**
 * @brief  interface contactless deinit
 * @return status code
 *         - 0 success
 *         - 1 contactless deinit failed
 * @note   none
 */
uint8_t mifare_classic_interface_contactless_deinit(void)
{
    if (gpio_interrupt_deinit() != 0)
    {
        return 1;
    }
    g_gpio_irq = NULL;

#ifdef USE_DRIVER_MFRC522
    if (mfrc522_basic_deinit() != 0)
    {
        return 1;
    }
#else
    #error "mifare_classic no driver"
#endif
    
    return 0;
}

/**
 * @brief         interface contactless transceiver
 * @param[in]     *in_buf pointer to a input buffer
 * @param[in]     in_len input length
 * @param[out]    *out_buf pointer to a output buffer
 * @param[in,out] *out_len pointer to a output length buffer
 * @return        status code
 *                - 0 success
 *                - 1 contactless transceiver failed
 * @note          none
 */
uint8_t mifare_classic_interface_contactless_transceiver(uint8_t *in_buf, uint8_t in_len, uint8_t *out_buf, uint8_t *out_len)
{
#ifdef USE_DRIVER_MFRC522
    if (mfrc522_basic_transceiver(in_buf, in_len, out_buf, out_len) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
#else
    #error "mifare_classic no driver"
#endif
}

/**
 * @brief     interface delay ms
 * @param[in] ms time
 * @note      none
 */
void mifare_classic_interface_delay_ms(uint32_t ms)
{
    usleep(1000 * ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
void mifare_classic_interface_debug_print(const char *const fmt, ...)
{
    char str[256];
    va_list args;
    
    memset((char *)str, 0, sizeof(char) * 256); 
    va_start(args, fmt);
    vsnprintf((char *)str, 255, (char const *)fmt, args);
    va_end(args);
    
    (void)printf((uint8_t *)str);
}
