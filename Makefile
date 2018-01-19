utils = eth_phy_peekpoke led7800 peekpoke rtc7800 ts7800ctl load_fpga_flash

CC ?= arm-marvell-linux-uclibcgnueabi-gcc
CFLAGS =  -Wall -O 

all:  $(utils)

clean: 
	rm -f $(utils) 


