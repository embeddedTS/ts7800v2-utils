bin_PROGRAMS = ts7800ctl accel-test eth_phy_peekpoke led7800 peekpoke rtc7800 load_fpga_flash pwmctl isa_peekpoke keypad lcdmesg syscon_peekpoke

GITCOMMIT:= $(shell git describe --abbrev=12 --dirty --always)

ts7800ctl_SOURCES = ts7800ctl.c gpiolib.c gpiolib.h i2c-dev.h 
ts7800ctl_CPPFLAGS = -Wall -DGITCOMMIT="\"${GITCOMMIT}\""

accel_test_SOURCES = accel-test.c 
accel_test_CPPFLAGS = -Wall -DGITCOMMIT="\"${GITCOMMIT}\""

eth_phy_peekpoke_SOURCES = eth_phy_peekpoke.c
eth_phy_peekpoke_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""

led7800_SOURCES = led7800.c i2c-dev.h
led7800_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""

peekpoke_SOURCES = peekpoke.c peekpoke.h
peekpoke_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""

rtc7800_SOURCES = rtc7800.c
rtc7800_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""

load_fpga_flash_SOURCES = load_fpga_flash.c
load_fpga_flash_CPPFLAGS =  -DGITCOMMIT="\"${GITCOMMIT}\""

pwmctl_SOURCES = pwmctl.c
pwmctl_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""

lcdmesg_SOURCES = lcdmesg.c fpga.c
lcdmesg_CPPFLAGS = -lgpiod -DGITCOMMIT="\"${GITCOMMIT}\""

keypad_SOURCES = keypad.c
keypad_CPPFLAGS = -lgpiod -DGITCOMMIT="\"${GITCOMMIT}\""

syscon_peekpoke_SOURCES = syscon_peekpoke.c fpga.c
syscon_peekpoke_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""

isa_peekpoke_SOURCES = isa_peekpoke.c fpga.c
isa_peekpoke_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""
