#ifndef _BASE32_H_
#define _BASE32_H_

#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

//typedef int size_t;

size_t base32encode_get_length(size_t size);
size_t base32encode(char* dest, const void* src, size_t size);

int    base32validate(const char* src, size_t size);
size_t base32decode_get_length(size_t size);
size_t base32decode(void* dest, const char* src, size_t size);

#ifdef __cplusplus
}
#endif

#endif

