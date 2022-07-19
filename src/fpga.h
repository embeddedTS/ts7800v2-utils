#pragma once
uint32_t* isa_init(void);
uint32_t* syscon_init(void);
extern inline uint8_t syscon_peek8(uint32_t *syscon, size_t offs);
extern inline uint16_t syscon_peek16(uint32_t *syscon, size_t offs);
extern inline uint32_t syscon_peek32(uint32_t *syscon, size_t offs);
extern inline uint64_t syscon_peek64(uint32_t *syscon, size_t offs);
extern inline void syscon_poke32(uint32_t *syscon, size_t offs, uint32_t val);
extern inline void syscon_poke64(uint32_t *syscon, size_t offs, uint64_t val);
extern inline void syscon_poke16(uint32_t *syscon, size_t offs, uint16_t val);
extern inline void syscon_poke8(uint32_t *syscon, size_t offs, uint8_t val);

// all the ISA access functions:
extern inline void isa_io_peek8(uint32_t *isa, uint8_t offs);
extern inline uint8_t isa_io_poke8(uint32_t *isa, uint8_t offs, uint8_t val);
extern inline uint16_t isa_io_peek16(uint32_t *isa, uint8_t offs);
extern inline uint16_t isa_io_poke16(uint32_t *isa, uint8_t offs, uint16_t val);
extern inline uint8_t isa_mem_peek8(uint32_t *isa, uint8_t offs);
extern inline uint8_t isa_mem_poke8(uint32_t *isa, uint8_t offs, uint8_t val);
extern inline uint16_t isa_mem_peek16(uint32_t *isa, uint8_t offs);
extern inline uint16_t isa_mem_poke16(uint32_t *isa, uint8_t offs, uint16_t val);