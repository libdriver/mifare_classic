[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver MIFARE_Classic

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/mifare_classic/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

NXP Semiconductors has developed the MIFARE Classic EV1 contactless IC MF1S50yyX/V1 to be used in a contactless smart card according to ISO/IEC 14443 Type A.The MIFARE Classic EV1 with 1K memory MF1S50yyX/V1 IC is used in applications like public transport ticketing and can also be used for various other applications.MIFARE_Classic can be used in Public transportation, Electronic toll collection, School and campus cards and so on.

LibDriver MIFARE_Classic is the full function driver of MIFARE_Classic launched by LibDriver.It provides the function of read block, write block, value increment, value decrement and so on. LibDriver is MISRA compliant.

This repository includes submodule and you shold add "--recursive" to clone the whole project.

```shell
git clone https://github.com/libdriver/mifare_classic.git --recursive
```

### Table of Contents

  - [Instruction](#Instruction)
  - [Install](#Install)
  - [Usage](#Usage)
    - [example basic](#example-basic)
  - [Document](#Document)
  - [Contributing](#Contributing)
  - [License](#License)
  - [Contact Us](#Contact-Us)

### Instruction

/src includes LibDriver MIFARE_Classic source files.

/interface includes LibDriver MIFARE_Classic RF platform independent template.

/test includes LibDriver MIFARE_Classic driver test code and this code can test the chip necessary function simply.

/example includes LibDriver MIFARE_Classic sample code.

/doc includes LibDriver MIFARE_Classic offline document.

/datasheet includes MIFARE_Classic datasheet.

/project includes the common Linux and MCU development board sample code. All projects use the shell script to debug the driver and the detail instruction can be found in each project's README.md.

/misra includes the LibDriver MISRA code scanning results.

### Install

Reference /interface RF platform independent template and finish your platform RF driver.

Add the /src directory, the interface driver for your platform, and your own drivers to your project, if you want to use the default example drivers, add the /example directory to your project.

### Usage

You can refer to the examples in the /example directory to complete your own driver. If you want to use the default programming examples, here's how to use them.

#### example basic

```C
#include "driver_mifare_classic_basic.h"

uint8_t res;
mifare_classic_authentication_key_t key_type;
uint8_t key[6];
mifare_classic_type_t type; 
uint8_t id[4];
uint8_t data[16];

/* init */
res = mifare_classic_basic_init();
if (res != 0)
{
    return 1;
}

...
    
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

...

key_type = MIFARE_CLASSIC_AUTHENTICATION_KEY_A;
key[0] = 0xFF;
key[1] = 0xFF;
key[2] = 0xFF;
key[3] = 0xFF;
key[4] = 0xFF;
key[5] = 0xFF;

...
    
/* read */
res = mifare_classic_basic_read(1, data, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

/* write */
res = mifare_classic_basic_write(1, data, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

/* value init */
res = mifare_classic_basic_value_init(2, 2, 2, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

/* write the value */
res = mifare_classic_basic_value_write(2, 100, 2, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}    

...

/* read the value */
res = mifare_classic_basic_value_read(2, &value, &addr, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

/* increment the value */
res = mifare_classic_basic_value_increment(2, 10, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

/* decrement the value */
res = mifare_classic_basic_value_decrement(2, 20, key_type, key);
if (res != 0)
{
    (void)mifare_classic_basic_deinit();

    return 1;
}

...

(void)mifare_classic_basic_deinit();

return 0;    
```

### Document

Online documents: [https://www.libdriver.com/docs/mifare_classic/index.html](https://www.libdriver.com/docs/mifare_classic/index.html).

Offline documents: /doc/html/index.html.

### Contributing

Please refer to CONTRIBUTING.md.

### License

Copyright (c) 2015 - present LibDriver All rights reserved



The MIT License (MIT) 



Permission is hereby granted, free of charge, to any person obtaining a copy

of this software and associated documentation files (the "Software"), to deal

in the Software without restriction, including without limitation the rights

to use, copy, modify, merge, publish, distribute, sublicense, and/or sell

copies of the Software, and to permit persons to whom the Software is

furnished to do so, subject to the following conditions: 



The above copyright notice and this permission notice shall be included in all

copies or substantial portions of the Software. 



THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,

FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE

AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER

LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,

OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE

SOFTWARE. 

### Contact Us

Please send an e-mail to lishifenging@outlook.com.