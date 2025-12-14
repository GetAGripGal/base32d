#ifndef BASE32_H
#define BASE32_H

#include <stdint.h>
#include <stdlib.h>

/**
 * Decode a base32 string.
 *
 * @param source The base32 string to decode.
 * @safety If the source string is not null terminated unallocated memory might
 * be read, causing UB.
 * @returns The resulting decoded, null-terminated string.
 */
char *base32_decode(const char *source);

#endif // BASE32_H
