CC     := gcc
SRC    := $(wildcard ./interface/src/*.c) \
		  $(wildcard ./driver/src/*.c) \
		  $(wildcard ./src/*.c) \
		  $(wildcard ../../src/*.c) \
		  $(wildcard ../../test/*.c) \
		  $(wildcard ../../example/*.c) \
		  $(wildcard ../../reader/mfrc522/src/*.c) \
		  $(wildcard ../../reader/mfrc522/example/*.c) \
		  $(wildcard ../../reader/mfrc522/project/raspberrypi4b/driver/src/*.c) \
		  $(wildcard ../../reader/mfrc522/project/raspberrypi4b/interface/src/*.c)
LIBS   := -lpthread \
		  -lgpiod
CFLAGS := -O3 \
		  -D USE_DRIVER_MFRC522 \
		  -D NO_DEBUG \
		  -I ./interface/inc/ \
		  -I ../../interface/ \
		  -I ../../src/ \
		  -I ../../test/ \
		  -I ../../example/ \
		  -I ../../reader/mfrc522/src/ \
		  -I ../../reader/mfrc522/interface/ \
		  -I ../../reader/mfrc522/example/ \
		  -I ../../reader/mfrc522/project/raspberrypi4b/driver/ \
		  -I ../../reader/mfrc522/project/raspberrypi4b/interface/inc/ 
mifare_classic : $(SRC)
		 "$(CC)" $(CFLAGS) $^ $(LIBS) -o $@