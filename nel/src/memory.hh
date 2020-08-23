#ifndef NEL_MEMORY_HH
#define NEL_MEMORY_HH

#include <cstdint> // uint8_t
#include <cstddef> // size_t

namespace nel {

void memcpy(uint8_t *const d, uint8_t const *const s, size_t const n);
void memset(uint8_t *const d, uint8_t const s, size_t const n);
void memmove(uint8_t *const d, uint8_t *const s, size_t const n);
void memswap(uint8_t *const d, uint8_t *const s, size_t const n);

}



#endif//NEL_MEMORY_HH
