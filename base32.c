#include "base32.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BASE32_ALPHABET_SIZE 32
#define QUANTA_SIZE 8 // 8 values of 5 bits totalling 40 bits.

/**
 * The alphabet for base32.
 */
const char BASE32_ALPHABET[BASE32_ALPHABET_SIZE + 1] =
    "ABCDEFGHJKLMNPQRSTUVWXYZ23456789"; // (+1 to the size for the null
                                        // terminator).

/**
 * Decode a base32 char.
 *
 * @param symbol The input symbol
 * @return The decoded char on success. -1 on error.
 */
static char base32_decode_char(char c);

/**
 * Calculate the amount of padding in a base32 string.
 *
 * @param source The source string.
 * @safety If the source string is not null terminate unallocated memory
 * might be read, causing UB.
 * @returns The amount of padding in the string.
 */
static size_t base32_calc_padding_amount(const char *source);

/**
 * Calculates the amount of quanta in a base32 string.
 *
 * @param source The source string.
 * @param padding_amount The amount of padding in the string.
 * @safety If the source string is not null terminate unallocated memory might
 * be read, causing UB.
 * @param last_quanta_len The lenght of the last quanta.
 * @returns The amount of quanta in a base32 string.
 */
static size_t base32_calc_quanta_amount(const char *source,
                                        size_t padding_amount);

/**
 * Caclulate the length of the last quanta in a base32 string.
 *
 * @param source The source string.
 * @param padding_amount The amount of padding in the string.
 * @safety If the source string is not null terminate unallocated memory might
 * be read, causing UB.
 * @returns The size of the last quanta in the base32 string.
 */
static size_t base32_calc_last_quanta_len(const char *source,
                                          size_t padding_amount);

/**
 * Calculate the size of the decoded base32 string.
 *
 * @param source A base32 encoded string.
 * @param padding_amount The amount of padding in the string.
 * @safety If the source string is not null terminate unallocated memory might
 * be read, causing UB.
 * @param last_quantum_len The lenght of the last quantum.
 * @returns The size in bytes of the encoded string, including null-terminator.
 * Returns 0 if the base32 string is invalid.
 */
static size_t base32_calc_decoded_size(const char *source,
                                       size_t padding_amount,
                                       size_t last_quantum_len);

char *base32_decode(const char *source) {
  const size_t source_len = strlen(source);
  const size_t padding_amount = base32_calc_padding_amount(source);

  size_t quanta_amount = base32_calc_quanta_amount(source, padding_amount);
  size_t last_quantum_len = base32_calc_last_quanta_len(source, padding_amount);

  // If the last quantum is a partial quantum we need to add one to the quanta
  // amount.
  if (last_quantum_len > 0) {
    quanta_amount += 1;
  } else {
    last_quantum_len = QUANTA_SIZE;
  }

  const size_t decoded_len =
      base32_calc_decoded_size(source, padding_amount, last_quantum_len);
  if (decoded_len <= 0) {
    return NULL;
  }

  char *buffer = (char *)malloc(decoded_len);
  char *partial_destination = buffer;
  const char *partial_source = source;

  size_t quantum_len;
  for (int32_t i = 0; i < quanta_amount; i++) {
    // Check if we are on the last quantum.
    if (i == quanta_amount - 1) {
      quantum_len = last_quantum_len;
    } else {
      quantum_len = 8;
    }

    switch (quantum_len) {
    case 8:
      partial_destination[4] = base32_decode_char(partial_source[7]);
      partial_destination[4] |= base32_decode_char(partial_source[6]) << 5;
    case 7:
      partial_destination[3] = base32_decode_char(partial_source[6]) >> 3;
      partial_destination[3] |= (base32_decode_char(partial_source[5]) & 0x1F)
                                << 2;
      partial_destination[3] |= base32_decode_char(partial_source[4]) << 7;
    case 5:
      partial_destination[2] = base32_decode_char(partial_source[4]) >> 1;
      partial_destination[2] |= base32_decode_char(partial_source[3]) << 4;
    case 4:
      partial_destination[1] = base32_decode_char(partial_source[3]) >> 4;
      partial_destination[1] |= (base32_decode_char(partial_source[2]) & 0x1F)
                                << 1;
      partial_destination[1] |= base32_decode_char(partial_source[1]) << 6;
    case 2:
      partial_destination[0] = base32_decode_char(partial_source[1]) >> 2;
      partial_destination[0] |= base32_decode_char(partial_source[0]) << 3;
      break;
    default:
      fprintf(stderr, "Found quantum with invalid lenght: %u\n",
              (uint32_t)quantum_len);
      return NULL;
      break;
    }

    partial_source += 8;
    partial_destination += 5;
  }

  buffer[decoded_len] = '\0';
  return buffer;
}

static char base32_decode_char(char c) {
  if (c >= 'A' && c <= 'Z') {
    return c - 'A';
  } else if (c >= '2' && c <= '7') {
    return c - '2' + 26;
  } else if (c >= 'a' && c <= 'z') {
    return c - 'a';
  } else {
    return 0xFF;
  }
}

static size_t base32_calc_padding_amount(const char *source) {
  const char padding_char = '=';
  size_t source_len = strlen(source);
  size_t padding_amount = 0;

  for (size_t i = 1; i < source_len; i++) {
    const char c = source[source_len - i];
    if (c == padding_char) {
      padding_amount++;
    } else if (c == '\n') {
      continue;
    } else {
      break;
    }
  }

  return padding_amount;
}

static size_t base32_calc_quanta_amount(const char *source,
                                        size_t padding_amount) {
  return ((strlen(source) - 1) - padding_amount) / QUANTA_SIZE;
}

static size_t base32_calc_last_quanta_len(const char *source,
                                          size_t padding_amount) {
  return ((strlen(source) - 1) - padding_amount) % QUANTA_SIZE;
}

static size_t base32_calc_decoded_size(const char *source,
                                       size_t padding_amount,
                                       size_t last_quantum_len) {
  size_t len = base32_calc_quanta_amount(source, padding_amount) * 5;
  switch (last_quantum_len) {
  case 8:
    return len;
  case 7:
    return len + 4;
  case 5:
    return len + 2;
  case 4:
    return len + 2;
  case 2:
    return len + 1;
  default:
    fprintf(stderr, "The last quantum has an invalid length of: %u\n",
            (uint32_t)last_quantum_len);
    break;
  }
  return 0;
}