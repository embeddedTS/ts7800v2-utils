// findpci_lib.c
// c. 2022 embeddedTS.com 
// Michael D. Peters
//
// PCI base address is determined at boot-time, and isn't always the same.
// The TS-7800-V2 uses the PCI bus to talk to its FPGA.  
// This software finds the current fpga base address for BAR #2 or BAR #3
// on the pci bus for the FPGA.
//
// Added peekpoke utility functions written by Mark Featherston.

#include <stdio.h>
#include <stdint.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "fpga.h"

// all the syscon access functions.

static inline uint8_t syscon_peek8(uint32_t *syscon, size_t offs) {
        return *(volatile uint8_t *)(syscon + offs);
}

static inline uint16_t syscon_peek16(uint32_t *syscon, size_t offs) {
        return *(volatile uint16_t *)(syscon + offs);
}

static inline uint32_t syscon_peek32(uint32_t *syscon, size_t offs) {
        return *(volatile uint32_t *)(syscon + offs);
}

static inline uint64_t syscon_peek64(uint32_t *syscon, size_t offs) {
        return *(volatile uint64_t *)(syscon + offs);
}

static inline void syscon_poke32(uint32_t *syscon, size_t offs, uint32_t val) {
        *(volatile uint32_t *)(syscon + offs) = val;
}

static inline void syscon_poke64(uint32_t *syscon, size_t offs, uint64_t val) {
        *(volatile uint64_t *)(syscon + offs) = val;
}

static inline void syscon_poke16(uint32_t *syscon, size_t offs, uint16_t val) {
        *(volatile uint16_t *)(syscon + offs) = val;
}

static inline void syscon_poke8(uint32_t *syscon, size_t offs, uint8_t val) {
        *(volatile uint8_t *)(syscon + offs) = val;
}

// all the ISA access functions:
static inline void isa_io_peek8(uint32_t *isa, uint8_t offs){
	return *(volatile uint8_t *)(isa + offs + 0x2000000);
}
static inline uint8_t isa_io_poke8(uint32_t *isa, uint8_t offs, uint8_t val){
	*(volatile uint8_t *)(isa + offs + 0x2000000) = val;
}
static inline uint16_t isa_io_peek16(uint32_t *isa, uint8_t offs){
	return *(volatile uint16_t *)(isa + offs + 0x3000000);
}
static inline uint16_t isa_io_poke16(uint32_t *isa, uint8_t offs, uint16_t val){
	*(volatile uint16_t *)(isa + offs + 0x3000000) = val;
}

static inline uint8_t isa_mem_peek8(uint32_t *isa, uint8_t offs){
	return *(volatile uint8_t *)(isa + offs);
}
static inline uint8_t isa_mem_poke8(uint32_t *isa, uint8_t offs, uint8_t val){
	*(volatile uint8_t *)(isa + offs);
}
static inline uint16_t isa_mem_peek16(uint32_t *isa, uint8_t offs){
	return *(volatile uint16_t *)(isa + offs + 0x1000000);
}
static inline uint16_t isa_mem_poke16(uint32_t *isa, uint8_t offs, uint16_t val){
	*(volatile uint16_t *)(isa + offs + 0x1000000);
}

uint32_t* syscon_init(void)
{
        int fd;
	uint32_t *fpga;
	fd = open("/sys/bus/pci/devices/0000:02:00.0/resource2", O_RDWR|O_SYNC);
        if (fd == -1)
                return NULL;
        fpga = (size_t)mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
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
	uint8_t* mem8_addr;
	fd = open("/sys/bus/pci/devices/0000:02:00.0/resource3", O_RDWR|O_SYNC);
	if(fd == -1)
		return NULL;
	mem8_addr = (size_t)mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (!mem8_addr)
		return NULL;
	else{
		close(fd);
		return mem8_addr;
	}
}
