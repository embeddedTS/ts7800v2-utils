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
	fprintf(stderr, "Usage %s <bit width 8, 16, 32, 64> <address> [value]\n", name);
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
		if (sz == 8)
			val = syscon_peek8(syscon, off);
		else if (sz == 16)
			val = syscon_peek16(syscon, off);
		else if (sz == 32)
			val = syscon_peek32(syscon, off);
		else if (sz == 64)
			val = syscon_peek64(syscon, off);
		else {
			usage(argv[0]);
			return 1;
		}
		printf("0x%llX\n", val);
	} else { /* syscon_poke */
		if (sz == 8)
			syscon_poke8(syscon, off, val);
		else if (sz == 16)
			syscon_poke16(syscon, off, val);
		else if (sz == 32)
			syscon_poke32(syscon, off, val);
		else if (sz == 64)
			syscon_poke64(syscon, off, val);
		else {
			usage(argv[0]);
			return 1;
		}
	}

	return 0;
}
