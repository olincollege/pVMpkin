#include "utils.h"

#include <SDL2/SDL_render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include "audio.h"
#include "memory.h"

struct timeval;

// Register Storage

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
uint16_t reg[R_COUNT];
struct termios original_tio;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void error_and_exit(const char* error_msg) {
  perror(error_msg);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  exit(EXIT_FAILURE);
}

void disable_input_buffering(void) {
  tcgetattr(STDIN_FILENO, &original_tio);
  struct termios new_tio = original_tio;
  new_tio.c_lflag &= (tcflag_t) ~(ICANON | ECHO);
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

void update_flags(uint16_t R_Rx) {
  if (reg[R_Rx] == 0) {
    reg[R_COND] = FL_ZRO;
  } else if (reg[R_Rx] >> (2 * BYTE_LEN - 1)) {
    reg[R_COND] = FL_NEG;
  } else {
    reg[R_COND] = FL_POS;
  }
}

uint16_t swap16(uint16_t bit) {
  return (uint16_t)(bit << BYTE_LEN) | (uint16_t)(bit >> BYTE_LEN);
}

void read_image_file(FILE* file) {
  /* the origin tells us where in memory to place the image */
  uint16_t origin = 0;
  if (!fread(&origin, sizeof(origin), 1, file)) {
    error_and_exit("Unable to read origin from image");
  }
  origin = swap16(origin);

  /* we know the maximum file size so we only need one fread */
  uint16_t max_read = MEMORY_MAX - origin;
  uint16_t* pointer = memory + origin;
  size_t read = fread(pointer, sizeof(uint16_t), max_read, file);

  // /* swap to little endian */
  while (read-- > 0) {
    *pointer = swap16(*pointer);
    ++pointer;
  }
}

int read_image(const char* image_path) {
  const char* output_obj = "audio.obj";
  char* suffix_wav = ".wav";
  char* suffix_mp3 = ".mp3";

  if (!strcmp(image_path + strlen(image_path) - strlen(suffix_wav),
              suffix_wav) ||
      !strcmp(image_path + strlen(image_path) - strlen(suffix_mp3),
              suffix_mp3)) {
    const char* output_pcm = "audio.pcm";
    if (process_audio(image_path, output_pcm) == 0) {
      pcm_to_obj(output_pcm, output_obj);
    } else {
      return 0;
    }
  } else {
    output_obj = image_path;
  }

  FILE* file = fopen(output_obj, "rbe");
  if (!file) {
    return 0;
  };
  read_image_file(file);

  if (fclose(file) != 0) {
    error_and_exit("Failed to close image file");
  }
  return 1;
}

void update_texture(SDL_Texture* texture) {
  void* pixels = NULL;
  int pitch = 0;
  SDL_LockTexture(texture, NULL, &pixels, &pitch);

  uint32_t* pixel_ptr = (uint32_t*)pixels;

  for (uint16_t addr = 0; addr < MEMORY_MAX; ++addr) {
    uint16_t val = memory[addr];
    uint16_t intensity = (uint16_t)(val >> BIT_SHIFT_8) & BYTE_MASK;
    uint32_t color = (BYTE_MASK << BIT_SHIFT_24) |            // A
                     (uint32_t)(intensity << BIT_SHIFT_16) |  // R
                     (uint32_t)(intensity << BIT_SHIFT_8) |   // G
                     (intensity);                             // B

    pixel_ptr[addr] = color;
  }

  SDL_UnlockTexture(texture);
}
