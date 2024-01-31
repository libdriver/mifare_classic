### 1. Board

#### 1.1 Board Info

Board Name: Raspberry Pi 4B.

IIC Pin: SCL/SDA GPIO3/GPIO2.

SPI Pin: SCLK/MOSI/MISO/CS GPIO11/GPIO10/GPIO9/GPIO8.

UART Pin: TX/RX GPIO14/GPIO15.

GPIO Pin: INT GPIO17.

RESET Pin: RESET GPIO27.

### 2. Install

#### 2.1 Dependencies

Install the necessary dependencies.

```shell
sudo apt-get install libgpiod-dev pkg-config cmake -y
```

#### 2.2 Makefile

Build the project.

```shell
make
```

Install the project and this is optional.

```shell
sudo make install
```

Uninstall the project and this is optional.

```shell
sudo make uninstall
```

#### 2.3 CMake

Build the project.

```shell
mkdir build && cd build 
cmake .. 
make
```

Install the project and this is optional.

```shell
sudo make install
```

Uninstall the project and this is optional.

```shell
sudo make uninstall
```

Test the project and this is optional.

```shell
make test
```

Find the compiled library in CMake. 

```cmake
find_package(mifare_classic REQUIRED)
```


### 3. MIFARE_CLASSIC

#### 3.1 Command Instruction

1. Show mifare_classic chip and driver information.

   ```shell
   mifare_classic (-i | --information)
   ```

2. Show mifare_classic help.

   ```shell
   mifare_classic (-h | --help)
   ```

3. Show mifare_classic pin connections of the current board.

   ```shell
   mifare_classic (-p | --port)
   ```

4. Run mifare_classic card test.

   ```shell
   mifare_classic (-t card | --test=card)
   ```

5. Run chip halt function.

   ```shell
   mifare_classic (-e halt | --example=halt)
   ```

6. Run wake up the chip function.

   ```shell
   mifare_classic (-e wake-up | --example=wake-up)
   ```

7. Run read block function, addr is the read block address and it is hexadecimal, authentication is the authentication keys and it is hexadecimal with 6 bytes(strlen=12).

   ```shell
   mifare_classic (-e read | --example=read) [--key-type=<A | B>] [--key=<authentication>] [--block=<addr>]
   ```

8. Run write block function, addr is the write block address and it is hexadecimal, authentication is the authentication keys and it is hexadecimal with 6 bytes(strlen=12), data is the write data and it is hexadecimal with 16 bytes(strlen=32).

   ```shell
   mifare_classic (-e write | --example=write) [--key-type=<A | B>] [--key=<authentication>] [--block=<addr>] [--data=<hex>]
   ```

9. Run init as a value block function, addr is the set block address and it is hexadecimal, authentication is the authentication keys and it is hexadecimal with 6 bytes(strlen=12), dec is the inted value.

   ```shell
   mifare_classic (-e value-init | --example=value-init) [--key-type=<A | B>] [--key=<authentication>] [--block=<addr>] [--value=<dec>]
   ```

10. Run write value to the block function, addr is the set block address and it is hexadecimal, authentication is the authentication keys and it is hexadecimal with 6 bytes(strlen=12), dec is the write value.

    ```shell
    mifare_classic (-e value-write | --example=value-write) [--key-type=<A | B>] [--key=<authentication>] [--block=<addr>] [--value=<dec>]
    ```

11. Run read value from the block function, addr is the set block address and it is hexadecimal, authentication is the authentication keys and it is hexadecimal with 6 bytes(strlen=12).

    ```shell
    mifare_classic (-e value-read | --example=value-read) [--key-type=<A | B>] [--key=<authentication>] [--block=<addr>]
    ```

12. Run increment value function, addr is the set block address and it is hexadecimal, authentication is the authentication keys and it is hexadecimal with 6 bytes(strlen=12), dec is the increment value.

    ```shell
    mifare_classic (-e value-increment | --example=value-increment) [--key-type=<A | B>] [--key=<authentication>] [--block=<addr>] [--value=<dec>]
    ```

13. Run decrement value function, addr is the set block address and it is hexadecimal, authentication is the authentication keys and it is hexadecimal with 6 bytes(strlen=12), dec is the decrement value.

    ```shell
    mifare_classic (-e value-decrement | --example=value-decrement) [--key-type=<A | B>] [--key=<authentication>] [--block=<addr>] [--value=<dec>]
    ```

#### 3.2 Command Example

```shell
./mifare_classic -i

mifare_classic: chip is NXP MIFARE Classic EV1.
mifare_classic: manufacturer is NXP.
mifare_classic: interface is RF.
mifare_classic: driver version is 1.0.
mifare_classic: min supply voltage is 3.3V.
mifare_classic: max supply voltage is 4.0V.
mifare_classic: max current is 30.00mA.
mifare_classic: max temperature is 70.0C.
mifare_classic: min temperature is -25.0C.
```

```shell
./mifare_classic -p

mifare_classic: SPI interface SCK connected to GPIO11(BCM).
mifare_classic: SPI interface MISO connected to GPIO9(BCM).
mifare_classic: SPI interface MOSI connected to GPIO10(BCM).
mifare_classic: SPI interface CS connected to GPIO8(BCM).
mifare_classic: IIC interface SCL connected to GPIO3(BCM).
mifare_classic: IIC interface SDA connected to GPIO2(BCM).
mifare_classic: UART interface TX connected to GPIO14(BCM).
mifare_classic: UART interface RX connected to GPIO15(BCM).
mifare_classic: INT connected to GPIO17(BCM).
mifare_classic: RESET connected to GPIO27(BCM).
```

```shell
./mifare_classic -t card

mifare_classic: chip is NXP MIFARE Classic EV1.
mifare_classic: manufacturer is NXP.
mifare_classic: interface is RF.
mifare_classic: driver version is 1.0.
mifare_classic: min supply voltage is 3.3V.
mifare_classic: max supply voltage is 4.0V.
mifare_classic: max current is 30.00mA.
mifare_classic: max temperature is 70.0C.
mifare_classic: min temperature is -25.0C.
mifare_classic: start card test.
mifare_classic: find S70 card.
mifare_classic: id is 0xC9 0x73 0xBA 0x36.
mifare_classic: authentication block 0 ok.
mifare_classic: block 0 is 0xC9 0x73 0xBA 0x36 0x36 0x18 0x02 0x00 0x46 0x44 0x53 0x37 0x30 0x56 0x30 0x31 
mifare_classic: authentication block 1 ok.
mifare_classic: write block 1: 0x67 0xC6 0x69 0x73 0x51 0xFF 0x4A 0xEC 0x29 0xCD 0xBA 0xAB 0xF2 0xFB 0xE3 0x46 
mifare_classic: read block 1: 0x67 0xC6 0x69 0x73 0x51 0xFF 0x4A 0xEC 0x29 0xCD 0xBA 0xAB 0xF2 0xFB 0xE3 0x46 
mifare_classic: check data ok.
mifare_classic: authentication block 2 ok.
mifare_classic: init value is 0.
mifare_classic: read value is 0.
mifare_classic: write value is -10.
mifare_classic: read value is -10.
mifare_classic: increment 6.
mifare_classic: read value is -4.
mifare_classic: increment 6.
mifare_classic: read value is 2.
mifare_classic: increment 6.
mifare_classic: read value is 8.
mifare_classic: decrement 5.
mifare_classic: read value is 3.
mifare_classic: decrement 5.
mifare_classic: read value is -2.
mifare_classic: decrement 5.
mifare_classic: read value is -7.
mifare_classic: read block 2: 0xF9 0xFF 0xFF 0xFF 0x06 0x00 0x00 0x00 0xF9 0xFF 0xFF 0xFF 0x02 0xFD 0x02 0xFD 
mifare_classic: block0 to sector is 0.
mifare_classic: block15 to sector is 3.
mifare_classic: block30 to sector is 7.
mifare_classic: block45 to sector is 11.
mifare_classic: block60 to sector is 15.
mifare_classic: block75 to sector is 18.
mifare_classic: block90 to sector is 22.
mifare_classic: block105 to sector is 26.
mifare_classic: block120 to sector is 30.
mifare_classic: block135 to sector is 32.
mifare_classic: block150 to sector is 33.
mifare_classic: block165 to sector is 34.
mifare_classic: block180 to sector is 35.
mifare_classic: block195 to sector is 36.
mifare_classic: sector0 block count is 4.
mifare_classic: sector5 block count is 4.
mifare_classic: sector10 block count is 4.
mifare_classic: sector15 block count is 4.
mifare_classic: sector20 block count is 4.
mifare_classic: sector25 block count is 4.
mifare_classic: sector30 block count is 4.
mifare_classic: sector35 block count is 16.
mifare_classic: sector0: block 0 - 3.
mifare_classic: sector5: block 20 - 23.
mifare_classic: sector10: block 40 - 43.
mifare_classic: sector15: block 60 - 63.
mifare_classic: sector20: block 80 - 83.
mifare_classic: sector25: block 100 - 103.
mifare_classic: sector30: block 120 - 123.
mifare_classic: sector35: block 176 - 191.
mifare_classic: authentication block 3 ok.
mifare_classic: permission block0 block1 block2 block3 0x00 0x00 0x00 0x01.
mifare_classic: user data is 0x00.
mifare_classic: keyb is 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF.
mifare_classic: finish card test.
```

```shell
./mifare_classic -e halt

mifare_classic: halt.
```

```shell
./mifare_classic -e wake-up

mifare_classic: wake up.
```

```shell
./mifare_classic -e read --key-type=A --key=FFFFFFFFFFFF --block=0

mifare_classic: find S70 card.
mifare_classic: id is 0xC9 0x73 0xBA 0x36 
mifare_classic: block is 0xC9 0x73 0xBA 0x36 0x36 0x18 0x02 0x00 0x46 0x44 0x53 0x37 0x30 0x56 0x30 0x31 
```

```shell
./mifare_classic -e write --key-type=A --key=FFFFFFFFFFFF --block=1 --data=0123456789ABCDEF0123456789ABCDEF

mifare_classic: find S70 card.
mifare_classic: id is 0xC9 0x73 0xBA 0x36 
mifare_classic: data is 0x01 0x23 0x45 0x67 0x89 0xAB 0xCD 0xEF 0x01 0x23 0x45 0x67 0x89 0xAB 0xCD 0xEF 
```

```shell
./mifare_classic -e value-init --key-type=A --key=FFFFFFFFFFFF --block=2 --value=10

mifare_classic: find S70 card.
mifare_classic: id is 0xC9 0x73 0xBA 0x36 
mifare_classic: init block2 as value 10.
```

```shell
./mifare_classic -e value-write --key-type=A --key=FFFFFFFFFFFF --block=2 --value=200

mifare_classic: find S70 card.
mifare_classic: id is 0xC9 0x73 0xBA 0x36 
mifare_classic: write block2 200.
```

```shell
./mifare_classic -e value-read --key-type=A --key=FFFFFFFFFFFF --block=2

mifare_classic: find S70 card.
mifare_classic: id is 0xC9 0x73 0xBA 0x36 
mifare_classic: read block2 value 200.
```

```shell
./mifare_classic -e value-increment --key-type=A --key=FFFFFFFFFFFF --block=2 --value=20

mifare_classic: find S70 card.
mifare_classic: id is 0xC9 0x73 0xBA 0x36 
mifare_classic: increment block2 20.
```

```shell
./mifare_classic -e value-decrement --key-type=A --key=FFFFFFFFFFFF --block=2 --value=10

mifare_classic: find S70 card.
mifare_classic: id is 0xC9 0x73 0xBA 0x36 
mifare_classic: decrement block2 10.
```

```shell
./mifare_classic -h

Usage:
  mifare_classic (-i | --information)
  mifare_classic (-h | --help)
  mifare_classic (-p | --port)
  mifare_classic (-t card | --test=card)
  mifare_classic (-e halt | --example=halt)
  mifare_classic (-e wake-up | --example=wake-up)
  mifare_classic (-e read | --example=read) [--key-type=<A | B>] [--key=<authentication>]
                 [--block=<addr>]
  mifare_classic (-e write | --example=write) [--key-type=<A | B>] [--key=<authentication>]
                 [--block=<addr>] [--data=<hex>]
  mifare_classic (-e value-init | --example=value-init) [--key-type=<A | B>] [--key=<authentication>]
                 [--block=<addr>] [--value=<dec>]
  mifare_classic (-e value-write | --example=value-write) [--key-type=<A | B>] [--key=<authentication>]
                 [--block=<addr>] [--value=<dec>]
  mifare_classic (-e value-read | --example=value-read) [--key-type=<A | B>] [--key=<authentication>]
                 [--block=<addr>]
  mifare_classic (-e value-increment | --example=value-increment) [--key-type=<A | B>] [--key=<authentication>]
                 [--block=<addr>] [--value=<dec>]
  mifare_classic (-e value-decrement | --example=value-decrement) [--key-type=<A | B>] [--key=<authentication>]
                 [--block=<addr>] [--value=<dec>]

Options:
      --block=<addr>            Set the block address and it is hexadecimal.([default: 0x00])
      --data=<hex>              Set the input data and it is hexadecimal with 16 bytes(strlen=32).([default: 0x0123456789ABCDEF0123456789ABCDEF])
  -e <halt | wake-up | read | write | value-init | value-write | value-read | value-increment
     | value-decrement>, --example=<halt | wake-up | read | write | value-init | value-write
     | value-read | value-increment | value-decrement>
                                Run the driver example.
  -h, --help                    Show the help.
  -i, --information             Show the chip information.
      --key=<authentication>    Set the key of authentication and it is hexadecimal with 6 bytes(strlen=12).([default: 0xFFFFFFFFFFFF])
      --key-type=<A | B>        Set the key type of authentication.([default: A])
  -p, --port                    Display the pin connections of the current board.
  -t <card>, --test=<card>      Run the driver test.
      --value=<dec>             Set the input value.([default: 0])
```
