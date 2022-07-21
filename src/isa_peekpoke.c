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
	fprintf(stderr, "Usage %s <io/mem> <bit width> <address> [value]\n", name);
	fprintf(stderr, "\tEg: %s io 8 0x140\n", name);
}

int main(int argc, char **argv) {
	int sz;
	char bus =  '\0';
	uint32_t off;
	uint64_t val;
	static uint32_t *isa;

	if(argc != 4 && argc != 5) {
		usage(argv[0]);
		return 1;
	}

	bus = argv[1][0];

	sz = strtoul(argv[2], NULL, 0);
	isa, off = strtoul(argv[3], NULL, 0);
	if(argc == 5) val = strtoul(argv[4], NULL, 0);

	isa = isa_init();

	#ifdef DEBUG
	printf("%d\n", argc);
	printf("%c\n", argv[1][0]);
	printf("%d\n", sz);
	printf("0x%X\n", isa);
	#endif

 	/* syscon_peek */
	if (argc == 4) {
		if ((bus == 'm' || bus == 'M' ) && sz == 8)
			val = isa_mem_peek8(isa, off);
		else if ((bus == 'm' || bus == 'M' ) && sz == 16)
			val = isa_mem_peek16(isa, off);
		else if ((bus == 'i' || bus == 'I' ) && sz == 8)
			val = isa_io_peek8(isa, off);
		else if ((bus == 'i' || bus == 'I' ) && sz == 16)
			val = isa_io_peek16(isa, off);
		else {
			usage(argv[0]);
			return 1;
		}
		printf("0x%llX\n", val);
	} else { /* syscon_poke */
		if ((bus == 'm' || bus == 'M' ) && sz == 8)
			isa_mem_poke8(isa, off, val);
		else if ((bus == 'm' || bus == 'M' ) && sz == 16)
			isa_mem_poke16(isa, off, val);
		else if ((bus == 'i' || bus == 'I' ) && sz == 8)
			isa_io_poke8(isa, off, val);
		else if ((bus == 'i' || bus == 'I' ) && sz == 16)
			isa_io_poke16(isa, off, val);
		else {
			usage(argv[0]);
			return 1;
		}
	}

	return 0;
}
