### 1. chip

#### 1.1 chip info

chip name : Raspberry Pi 4B

iic pin: SCL/SDA GPIO3/GPIO2

spi pin: SCLK/MOSI/MISO/CS GPIO11/GPIO10/GPIO9/GPIO8

uart pin: TX/RX GPIO14/GPIO15

gpio pin: INT GPIO17

reset pin: RESET GPIO26

### 2. install

#### 2.1 install info

```shell
sudo apt-get install libgpiod-dev

make
```

### 3. mifare_classic

#### 3.1 command Instruction

​           mifare_classic is a basic command which can test all mifare_classic driver function:

​           -i          show mifare_classic chip and driver information.

​           -h        show mifare_classic help.

​           -p        show mifare_classic pin connections of the current board.

​           -t card        run mifare_classic card test. 

​           -c (halt | wake_up | -k (a | b) <keys> (read <block> | write <block> <data>| value (-init <block> <n> | -write <block> <n> | -read <block> | -increment <block> <n> | -decrement <block> <n>)))

​           -c halt                                                          chip halt.

​           -c wake_up                                                 wake up the chip.

​           -c -k (a | b) <keys> read <block>        read the block.block is the read block.keys is the authentication keys.

​           -c -k (a | b) <keys> write <block> <n>       write the block.block is the written block.keys is the authentication keys.n is the written data.

​           -c -k (a | b) <keys> value -init <block> <n>       init as a value block.block is the set block.keys is the authentication keys.n is the inted value.

​           -c -k (a | b) <keys> value -write <block> <n>       write a value to the block.block is the set block.keys is the authentication keys.n is the written value.

​           -c -k (a | b) <keys> value -read <block>       read a value from the block.block is the set block.keys is the authentication keys.

​           -c -k (a | b) <keys> value -increment  <block> <n>       increment the value.block is the set block.keys is the authentication keys.n is the increment value.

​           -c -k (a | b) <keys> value -decrement  <block> <n>      decrement the value.block is the set block.keys is the authentication keys.n is the decrement value.

#### 3.2 command example

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

mifare_classic: SPI interface SCK connected to GPIOA PIN5.
mifare_classic: SPI interface MISO connected to GPIOA PIN6.
mifare_classic: SPI interface MOSI connected to GPIOA PIN7.
mifare_classic: SPI interface CS connected to GPIOA PIN4.
mifare_classic: IIC interface SCL connected to GPIOB PIN8.
mifare_classic: IIC interface SDA connected to GPIOB PIN9.
mifare_classic: UART interface TX connected to GPIOA PIN2.
mifare_classic: UART interface RX connected to GPIOA PIN3.
mifare_classic: INT connected to GPIOB PIN0.
mifare_classic: RESET connected to GPIOA PIN8.
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
mifare_classic: find S50 card.
mifare_classic: id is 0x9C 0xA1 0x90 0x85.
mifare_classic: authentication block 0 ok.
mifare_classic: block 0 is 0x9C 0xA1 0x90 0x85 0x28 0x08 0x04 0x00 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 
mifare_classic: authentication block 1 ok.
mifare_classic: write block 1: 0x72 0x64 0x39 0x3D 0xEE 0xD2 0xB5 0x5A 0x39 0x68 0xE8 0xC9 0xAA 0x5C 0x9F 0x30 
mifare_classic: read block 1: 0x72 0x64 0x39 0x3D 0xEE 0xD2 0xB5 0x5A 0x39 0x68 0xE8 0xC9 0xAA 0x5C 0x9F 0x30 
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
./mifare_classic -c halt

mifare_classic: halt.
```

```shell
./mifare_classic -c wake_up

mifare_classic: wake up.
```

```shell
./mifare_classic -c -k a FFFFFFFFFFFF read 0 

mifare_classic: find S50 card.
mifare_classic: id is 0x9C 0xA1 0x90 0x85 
mifare_classic: block is 0x9C 0xA1 0x90 0x85 0x28 0x08 0x04 0x00 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 
```

```shell
./mifare_classic -c -k a FFFFFFFFFFFF write 1 0123456789ABCDEF0123456789ABCDEF

mifare_classic: find S50 card.
mifare_classic: id is 0x9C 0xA1 0x90 0x85 
mifare_classic: data is 0x01 0x23 0x45 0x67 0x89 0xAB 0xCD 0xEF 0x01 0x23 0x45 0x67 0x89 0xAB 0xCD 0xEF 
```

```shell
./mifare_classic -c -k a FFFFFFFFFFFF value -init 2 10 

mifare_classic: find S50 card.
mifare_classic: id is 0x9C 0xA1 0x90 0x85 
mifare_classic: init block2 as value 10.
```

```shell
./mifare_classic -c -k a FFFFFFFFFFFF value -write 2 100

mifare_classic: find S50 card.
mifare_classic: id is 0x9C 0xA1 0x90 0x85 
mifare_classic: write block2 100.
```

```shell
./mifare_classic -c -k a FFFFFFFFFFFF value -read 2

mifare_classic: find S50 card.
mifare_classic: id is 0x9C 0xA1 0x90 0x85 
mifare_classic: read block2 value 90.
```

```shell
./mifare_classic -c -k a FFFFFFFFFFFF value -increment 2 10

mifare_classic: find S50 card.
mifare_classic: id is 0x9C 0xA1 0x90 0x85 
mifare_classic: increment block2 10.
```

```shell
./mifare_classic -c -k a FFFFFFFFFFFF value -decrement 2 20

mifare_classic: find S50 card.
mifare_classic: id is 0x9C 0xA1 0x90 0x85 
mifare_classic: decrement block2 20.
```

```shell
./mifare_classic -h

mifare_classic -i
	show mifare_classic chip and driver information.
mifare_classic -h
	show mifare_classic help.
mifare_classic -p
	show mifare_classic pin connections of the current board.
mifare_classic -t card
	run mifare_classic card test.
mifare_classic -c halt
	chip halt.
mifare_classic -c wake_up
	wake up the chip.
mifare_classic -c -k (a | b) <keys> read <block>
	read the block.block is the read block.keys is the authentication keys.
mifare_classic -c -k (a | b) <keys> write <block> <n>
	write the block.block is the written block.keys is the authentication keys.n is the written data.
mifare_classic -c -k (a | b) <keys> value -init <block> <n>
	init as a value block.block is the set block.keys is the authentication keys.n is the inted value.
mifare_classic -c -k (a | b) <keys> value -write <block> <n>
	write a value to the block.block is the set block.keys is the authentication keys.n is the written value.
mifare_classic -c -k (a | b) <keys> value -read <block>
	read a value from the block.block is the set block.keys is the authentication keys.
mifare_classic -c -k (a | b) <keys> value -increment <block> <n>
	increment the value.block is the set block.keys is the authentication keys.n is the increment value.
mifare_classic -c -k (a | b) <keys> value -decrement <block> <n>
	decrement the value.block is the set block.keys is the authentication keys.n is the decrement value.
```
