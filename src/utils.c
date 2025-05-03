#include "utils.h"

#include <stdint.h>
#include <stdlib.h>

// Register Storage
uint16_t reg[R_COUNT];
struct termios original_tio;

void disable_input_buffering(void) {
  tcgetattr(STDIN_FILENO, &original_tio);
  struct termios new_tio = original_tio;
  new_tio.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

uint16_t check_key(void) {
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

void handle_interrupt(int signal) {
  restore_input_buffering();
  printf("\n");
  exit(-2);
}

void update_flags(uint16_t r) {
  if (reg[r] == 0) {
    reg[R_COND] = FL_ZRO;
  } else if (reg[r] >> 15) {
    reg[R_COND] = FL_NEG;
  } else {
    reg[R_COND] = FL_POS;
  }
}

uint16_t swap16(uint16_t x) { return (x << 8) | (x >> 8); }

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
  while (read-- > 0) {
    *p = swap16(*p);
    ++p;
  }
}

int read_image(const char* image_path) {
  FILE* file = fopen(image_path, "rb");
  if (!file) {
    return 0;
  };
  read_image_file(file);
  fclose(file);
  return 1;
}

void update_texture(SDL_Texture* texture, uint16_t* memory) {
  void* pixels;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixels, &pitch);

  uint32_t* pixel_ptr = (uint32_t*)pixels;

  for (uint16_t addr = 0; addr < UINT16_MAX; ++addr) {
    uint16_t val = memory[addr];
    uint16_t intensity = (val >> 8) & 0xFF;
    uint32_t color = (0xFF << 24) | // A
                (intensity << 16) | // R
                (intensity << 8)  | // G
                (intensity);        // B

    pixel_ptr[addr] = color;
  }

  SDL_UnlockTexture(texture);
}
