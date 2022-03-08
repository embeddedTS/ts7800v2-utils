#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#include "fpga.c"

void usage(char *name)
{
	fprintf(stderr, "Usage %s <bit width 8, 16, 32, 64> <address> [value]\n", name);
	fprintf(stderr, "\tEg: %s 32 0x0\n", name);
}

int main(int argc, char **argv) {
	int sz;
	uint32_t off;
	uint64_t val;

	if(argc != 3 && argc != 4) {
		usage(argv[0]);
		return 1;
	}

	sz = strtoul(argv[1], NULL, 0);
	off = strtoul(argv[2], NULL, 0);
	if(argc == 4) val = strtoul(argv[3], NULL, 0);

	fpga_init();

 	/* fpga_peek */
	if (argc == 3) {
		if (sz == 8)
			val = fpga_peek8(off);
		else if (sz == 16)
			val = fpga_peek16(off);
		else if (sz == 32)
			val = fpga_peek32(off);
		else if (sz == 64)
			val = fpga_peek64(off);
		else {
			usage(argv[0]);
			return 1;
		}
		printf("0x%llX\n", val);
	} else { /* fpga_poke */
		if (sz == 8)
			fpga_poke8(off, val);
		else if (sz == 16)
			fpga_poke16(off, val);
		else if (sz == 32)
			fpga_poke32(off, val);
		else if (sz == 64)
			fpga_poke64(off, val);
		else {
			usage(argv[0]);
			return 1;
		}
	}

	return 0;
}
