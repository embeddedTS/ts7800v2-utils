#pragma once
uint32_t* isa_init(void);
uint32_t* syscon_init(void);
uint8_t syscon_peek8(uint32_t *syscon, size_t offs);
uint16_t syscon_peek16(uint32_t *syscon, size_t offs);
uint32_t syscon_peek32(uint32_t *syscon, size_t offs);
uint64_t syscon_peek64(uint32_t *syscon, size_t offs);
void syscon_poke32(uint32_t *syscon, size_t offs, uint32_t val);
void syscon_poke64(uint32_t *syscon, size_t offs, uint64_t val);
void syscon_poke16(uint32_t *syscon, size_t offs, uint16_t val);
void syscon_poke8(uint32_t *syscon, size_t offs, uint8_t val);

// all the ISA access functions:
uint8_t isa_io_peek8(uint32_t *isa, uint8_t offs);
uint8_t isa_io_poke8(uint32_t *isa, uint8_t offs, uint8_t val);
uint16_t isa_io_peek16(uint32_t *isa, uint8_t offs);
uint16_t isa_io_poke16(uint32_t *isa, uint8_t offs, uint16_t val);
uint8_t isa_mem_peek8(uint32_t *isa, uint8_t offs);
uint8_t isa_mem_poke8(uint32_t *isa, uint8_t offs, uint8_t val);
uint16_t isa_mem_peek16(uint32_t *isa, uint8_t offs);
uint16_t isa_mem_poke16(uint32_t *isa, uint8_t offs, uint16_t val);