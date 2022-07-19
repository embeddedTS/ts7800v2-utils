#pragma once
uint32_t* isa_init(void);
uint32_t* syscon_init(void);
static inline uint8_t syscon_peek8(uint32_t *syscon, size_t offs);
static inline uint16_t syscon_peek16(uint32_t *syscon, size_t offs);
static inline uint32_t syscon_peek32(uint32_t *syscon, size_t offs);
static inline uint64_t syscon_peek64(uint32_t *syscon, size_t offs);
static inline void syscon_poke32(uint32_t *syscon, size_t offs, uint32_t val);
static inline void syscon_poke64(uint32_t *syscon, size_t offs, uint64_t val);
static inline void syscon_poke16(uint32_t *syscon, size_t offs, uint16_t val);
static inline void syscon_poke8(uint32_t *syscon, size_t offs, uint8_t val);

// all the ISA access functions:
static inline void isa_io_peek8(uint32_t *isa, uint8_t offs);
static inline uint8_t isa_io_poke8(uint32_t *isa, uint8_t offs, uint8_t val);
static inline uint16_t isa_io_peek16(uint32_t *isa, uint8_t offs);
static inline uint16_t isa_io_poke16(uint32_t *isa, uint8_t offs, uint16_t val);
static inline uint8_t isa_mem_peek8(uint32_t *isa, uint8_t offs);
static inline uint8_t isa_mem_poke8(uint32_t *isa, uint8_t offs, uint8_t val);
static inline uint16_t isa_mem_peek16(uint32_t *isa, uint8_t offs);
static inline uint16_t isa_mem_poke16(uint32_t *isa, uint8_t offs, uint16_t val);