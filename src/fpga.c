// c. 2022 embeddedTS.com 
//
// PCI base address is determined at boot-time, and isn't always the same.
// The TS-7800-V2 uses the PCI bus to talk to its FPGA.  

// This library acquires a temporary memory mapped pointer to the FPGA
//  resource, either ISA or Syscon, and handles a read or write at that
//  resource address.

#include <stdio.h>
#include <stdint.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "fpga.h"

// all the syscon access functions.

uint8_t syscon_peek8(uint32_t *syscon, size_t offs) {
        return *(volatile uint8_t *)(syscon + (offs/4));
}

uint16_t syscon_peek16(uint32_t *syscon, size_t offs) {
        return *(volatile uint16_t *)(syscon + (offs/4));
}

uint32_t syscon_peek32(uint32_t *syscon, size_t offs) {
        return *(volatile uint32_t *)(syscon + (offs/4));
}

uint64_t syscon_peek64(uint32_t *syscon, size_t offs) {
        return *(volatile uint64_t *)(syscon + (offs/4));
}

void syscon_poke32(uint32_t *syscon, size_t offs, uint32_t val) {
        *(volatile uint32_t *)(syscon + (offs/4)) = val;
}

void syscon_poke64(uint32_t *syscon, size_t offs, uint64_t val) {
        *(volatile uint64_t *)(syscon + (offs/4)) = val;
}

void syscon_poke16(uint32_t *syscon, size_t offs, uint16_t val) {
        *(volatile uint16_t *)(syscon + (offs/4)) = val;
}

void syscon_poke8(uint32_t *syscon, size_t offs, uint8_t val) {
        *(volatile uint8_t *)(syscon + (offs/4)) = val;
}

// all the ISA access functions:
uint8_t isa_io_peek8(uint32_t *isa, uint8_t offs){
	return *(volatile uint8_t *)(isa + (offs + 0x2000000)/4);
}
uint8_t isa_io_poke8(uint32_t *isa, uint8_t offs, uint8_t val){
	*(volatile uint8_t *)(isa + (offs + 0x2000000)/4) = val;
}
uint16_t isa_io_peek16(uint32_t *isa, uint8_t offs){
	return *(volatile uint16_t *)(isa + (offs + 0x3000000)/4);
}
uint16_t isa_io_poke16(uint32_t *isa, uint8_t offs, uint16_t val){
	*(volatile uint16_t *)(isa + (offs + 0x3000000)/4) = val;
}

uint8_t isa_mem_peek8(uint32_t *isa, uint8_t offs){
	return *(volatile uint8_t *)(isa + (offs/4));
}
uint8_t isa_mem_poke8(uint32_t *isa, uint8_t offs, uint8_t val){
	*(volatile uint8_t *)(isa + (offs/4));
}
uint16_t isa_mem_peek16(uint32_t *isa, uint8_t offs){
	return *(volatile uint16_t *)(isa + (offs + 0x1000000/2));
}
uint16_t isa_mem_poke16(uint32_t *isa, uint8_t offs, uint16_t val){
	*(volatile uint16_t *)(isa + (offs + 0x1000000/2));
}

uint32_t* syscon_init(void)
{
        int fd;
	uint32_t *fpga;
	fd = open("/sys/bus/pci/devices/0000:02:00.0/resource2", O_RDWR|O_SYNC);
        if (fd == -1)
                return NULL;
        fpga = (uint32_t *)mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(!fpga)
		return NULL;
	else{
		close(fd);
        	return fpga;
	}
}

uint32_t* isa_init(void)
{
	int fd;
	uint32_t* addr;
	fd = open("/sys/bus/pci/devices/0000:02:00.0/resource3", O_RDWR|O_SYNC);
	if(fd == -1)
		return NULL;
	addr = (uint32_t *)mmap(0, 0x4000000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (!addr)
		return NULL;
	else{
		close(fd);
		return addr;
	}
}
