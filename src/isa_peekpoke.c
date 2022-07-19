#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#include "fpga.h"

void usage(char *name)
{
	fprintf(stderr, "Usage %s <bus> <address> [value]\n", name);
	fprintf(stderr, "bus:  1 = MEM - 8 bit\n");
	fprintf(stderr, "      2 = MEM - 16 bit\n");
	fprintf(stderr, "      3 = IO  - 8 bit\n");
	fprintf(stderr, "      4 = IO  - 16 bitn");
	fprintf(stderr, "\tEg: %s 32 0x0\n", name);
}

int main(int argc, char **argv) {
	int sz;
	uint32_t off;
	uint64_t val;
	static uint32_t *syscon;

	if(argc != 3 && argc != 4) {
		usage(argv[0]);
		return 1;
	}

	sz = strtoul(argv[1], NULL, 0);
	syscon, off = strtoul(argv[2], NULL, 0);
	if(argc == 4) val = strtoul(argv[3], NULL, 0);

	syscon = syscon_init();

 	/* syscon_peek */
	if (argc == 3) {
		if (sz == 1)
			val = isa_mem_peek8(syscon, off);
		else if (sz == 2)
			val = isa_mem_peek16(syscon, off);
		else if (sz == 3)
			val = isa_io_peek8(syscon, off);
		else if (sz == 4)
			val = isa_io_peek16(syscon, off);
		else {
			usage(argv[0]);
			return 1;
		}
		printf("0x%llX\n", val);
	} else { /* syscon_poke */
		if (sz == 1)
			isa_mem_poke8(syscon, off, val);
		else if (sz == 2)
			isa_mem_poke16(syscon, off, val);
		else if (sz == 3)
			isa_io_poke8(syscon, off, val);
		else if (sz == 4)
			isa_io_poke16(syscon, off, val);
		else {
			usage(argv[0]);
			return 1;
		}
	}

	return 0;
}
