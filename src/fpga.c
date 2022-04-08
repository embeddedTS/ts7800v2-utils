// findpci_lib.c
// c. 2022 embeddedTS.com 
// Michael D. Peters
//
// PCI base address is determined at boot-time, and isn't always the same.
// The TS-7800-V2 uses the PCI bus to talk to its FPGA.  
// This software finds the current fpga base address for BAR #2 on the pci
// bus for the FPGA.
//
// Added peekpoke utility functions written by Mark Featherston.

#include <stdio.h>
#include <stdint.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

static inline uint8_t fpga_peek8(size_t offs) {
        return *(volatile uint8_t *)(fpga + offs);
}

static inline uint16_t fpga_peek16(size_t offs) {
        return *(volatile uint16_t *)(fpga + offs);
}

static inline uint32_t fpga_peek32(size_t offs) {
        return *(volatile uint32_t *)(fpga + offs);
}

static inline uint64_t fpga_peek64(size_t offs) {
        return *(volatile uint64_t *)(fpga + offs);
}

static inline void fpga_poke32(size_t offs, uint32_t val) {
        *(volatile uint32_t *)(fpga + offs) = val;
}

static inline void fpga_poke64(size_t offs, uint64_t val) {
        *(volatile uint64_t *)(fpga + offs) = val;
}

static inline void fpga_poke16(size_t offs, uint16_t val) {
        *(volatile uint16_t *)(fpga + offs) = val;
}

static inline void fpga_poke8(size_t offs, uint8_t val) {
        *(volatile uint8_t *)(fpga + offs) = val;
}

int get_os_info(void)
{
	int ver_maj, err;
	char ver_maj_char;
	struct utsname *os_info;

	os_info = malloc(sizeof(*os_info));
	err = uname(os_info);
	if(err){
		free(os_info);
		return -1;
	}
	ver_maj_char = os_info->release[0];
	ver_maj = atoi(&ver_maj_char);
	free(os_info);
	return ver_maj;
}

uint32_t* fpga_init(void)
{
        uint32_t *fpga = NULL;
        int fd;

        fd = open("/sys/bus/pci/devices/0000:02:00.0/resource2", O_RDWR|O_SYNC);
        if (fd == -1)
                return fd;
        fpga = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	close(fd);
        return fpga;
}
