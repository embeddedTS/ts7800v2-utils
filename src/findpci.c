// findpci.c
// c. 2022 embeddedTS.com 
// Michael D. Peters
//
// PCI base address is determined at boot-time, and isn't always the same.
// The TS-7800-V2 uses the PCI bus to talk to its FPGA.  Using the 4.4
// kernel we assume an address and go with it.  Turns out that was safe
// in 4.4 but is not at all safe in 5.10.
//
// This software will check the kernel version, then find and output the
// current fpga base address for BAR #2 on the pci device that is our 
// FPGA.

#include <stdio.h>
#include <stdint.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>


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


int main(void)
{
	int fd, ver_maj;
	void *fpga;

	
	ver_maj = get_os_info();
	if(ver_maj == -1) 
		goto bad_end;
	fd = open("/sys/bus/pci/devices/0000:02:00.0/resource0", O_RDWR|O_SYNC);
	if (fd == -1) goto bad_end;
	fpga = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if ((uint32_t)fpga == -1) goto bad_end;
	else
		printf("FPGA Base address = 0x%6X\n",(uint32_t)fpga);

good_end:
	return 0;

bad_end:
	return 1;
}
