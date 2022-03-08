#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

static size_t fpga;

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

void fpga_init(void)
{
	int fd;

	fd = open("/sys/bus/pci/devices/0000:02:00.0/resource0", O_RDWR|O_SYNC);
	assert(fd != -1);
	fpga = (size_t)mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	assert ((void *)fpga != (void *)-1);
}
