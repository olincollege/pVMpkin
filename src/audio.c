#include "audio.h"

#include <stdint.h>

#include "utils.h"

int process_audio(const char* audio_path, const char* output_pcm) {
  char command[512];
  snprintf(command, sizeof(command),
           "ffmpeg -y -i \"%s\" -t 5 -ar 8000 -ac 1 -f u8 \"%s\"", audio_path,
           output_pcm);

  int ret = system(command);

  if (ret != 0) {
    fprintf(stderr, "Error: ffmpeg failed\n");
    return -1;
  }
  return 0;
}

int pcm_to_obj(const char* output_pcm, const char* output_obj) {
  FILE* pcm_file = fopen(output_pcm, "rb");
  if (!pcm_file) {
    return 0;
  }

  FILE* obj_file = fopen(output_obj, "wb");
  if (!obj_file) {
    return 0;
  }

  uint16_t address = 0xF000;
  address = swap16(address);

  fwrite(&address, sizeof(address), 1, obj_file);

  while (1) {
    uint8_t first_half, second_half;
    uint16_t final_word;
    size_t ret1 = fread(&first_half, sizeof(uint8_t), 1, pcm_file);
    size_t ret2 = fread(&second_half, sizeof(uint8_t), 1, pcm_file);

    if (ret1 == 0) {
      break;
    }

    if (ret2 == 0) {
      final_word = swap16((0 << 8 | first_half));
    } else {
      final_word = swap16((second_half << 8 | first_half));
    }

    fwrite(&final_word, sizeof(final_word), 1, obj_file);
  }

  fclose(pcm_file);
  fclose(obj_file);
}
