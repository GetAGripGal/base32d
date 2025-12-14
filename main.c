#include "base32.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_CHUNK_SIZE 1024

/**
 * Read stdin to a dynamically sized buffer.
 */
static char *read_stin_to_string() {
  int chunk_count = 0;
  size_t buffer_size = 0;
  char *buffer = (char *)malloc(1);
  if (buffer == NULL) {
    return NULL;
  }

  char *chunk = (char *)malloc(INPUT_CHUNK_SIZE + 1);
  if (chunk == NULL) {
    free(buffer);
    return NULL;
  }

  while (fgets(chunk, INPUT_CHUNK_SIZE + 1, stdin) != NULL) {
    size_t chunk_length = strlen(chunk);
    size_t new_size = buffer_size + chunk_length;

    char *new_buffer = (char *)realloc(buffer, new_size);
    if (new_buffer == NULL) {
      free(buffer);
      free(chunk);
      return NULL;
    }

    buffer = new_buffer;
    memcpy(buffer + buffer_size, chunk, chunk_length);

    buffer_size = new_size;
    chunk_count++;
  }

  // ensure null termination
  buffer[buffer_size] = '\0';
  free(chunk);
  return buffer;
}

int main(int argc, char **argv) {
  char *input = read_stin_to_string();
  if (input == NULL) {
    fprintf(stderr, "Out of memory");
    return 0;
  }

  char *decoded = base32_decode(input);
  if (decoded == NULL) {
    return 0;
  }

  puts(decoded);
  free(input);
  return 0;
}