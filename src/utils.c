#include <stdint.h>
#include "utils.h"

// Register Storage
uint16_t reg[R_COUNT];

void update_flags(uint16_t r) {
  if (reg[r] == 0) {
    reg[R_COND] = FL_ZRO;
  } else if (reg[r] >> 15) {
    reg[R_COND] = FL_NEG;
  } else {
    reg[R_COND] = FL_POS;
  }
}

uint16_t swap16(uint16_t x) {
  return (x << 8) | (x >> 8);
}

void read_image_file(FILE* file) {
  /* the origin tells us where in memory to place the image */
  uint16_t origin;
  fread(&origin, sizeof(origin), 1, file);
  origin = swap16(origin);

  /* we know the maximum file size so we only need one fread */
  uint16_t max_read = MEMORY_MAX - origin;
  uint16_t* p = memory + origin;
  size_t read = fread(p, sizeof(uint16_t), max_read, file);

  /* swap to little endian */
  while (read-- > 0)
  {
      *p = swap16(*p);
      ++p;
  }
}

int read_image(const char* image_path) {
  FILE* file = fopen(image_path, "rb");
  if (!file) { return 0; };
  read_image_file(file);
  fclose(file);
  return 1;
}
