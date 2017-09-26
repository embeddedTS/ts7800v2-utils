utils = eth_phy_peekpoke led7800 peekpoke rtc7800 ts7800ctl 

CC ?= arm-marvell-linux-uclibcgnueabi-gcc
CFLAGS =  -Wall -O -mcpu=arm9 -Wl,--rpath,/slib -Wl,-dynamic-linker,/slib/ld-uClibc.so.0

all:  $(utils)

clean: 
	rm -f $(utils) 


