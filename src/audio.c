#include "audio.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

SDL_AudioDeviceID
    audio_device;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int process_audio(const char* audio_path, const char* output_pcm) {
  const int command_len = 512;
  char command[command_len];
  int written =
      snprintf(command, sizeof(command),
               "ffmpeg -y -i \"%s\" -t 5 -ar 8000 -ac 1 -f s16le \"%s\"",
               audio_path, output_pcm);

  if (written < 0 || (size_t)written >= sizeof(command)) {
    error_and_exit("Error: snprintf overflow or failed");
  }

  int ret = system(command);  // NOLINT(cert-env33-c, concurrency-mt-unsafe)
  if (ret != 0) {
    error_and_exit("Error: ffmpeg command failed");
  }
  return 0;
}

void file_close_helper(FILE* file1, FILE* file2) {
  if (fclose(file1) != 0) {
    if (fclose(file2) != 0) {
      error_and_exit("Error closing OBJ file");
    }
    error_and_exit("Error closing PCM file");
  }
  if (fclose(file2) != 0) {
    error_and_exit("Error closing OBJ file");
  }
}

int pcm_to_obj(const char* output_pcm, const char* output_obj) {
  FILE* pcm_file = fopen(output_pcm, "rbe");
  if (!pcm_file) {
    error_and_exit("Error opening PCM input file");
  }

  FILE* obj_file = fopen(output_obj, "wbe");
  if (!obj_file) {
    if (fclose(pcm_file) != 0) {
      error_and_exit("Error closing PCM file");
    }
    error_and_exit("Error opening OBJ output file");
  }

  uint16_t address = AUDIO_ADDRESS;
  address = swap16(address);

  if (fwrite(&address, sizeof(address), 1, obj_file) != 1) {
    file_close_helper(pcm_file, obj_file);
    error_and_exit("Error writing start address to OBJ file");
  }

  uint16_t audio_sample = 0;
  while (1) {
    size_t ret = fread(&audio_sample, sizeof(uint16_t), 1, pcm_file);

    if (ret == 0) {
      if (feof(pcm_file)) {
        break;  // normal end
      }
      file_close_helper(pcm_file, obj_file);
      error_and_exit("Error reading PCM file");
    }

    audio_sample = swap16(audio_sample);

    if (fwrite(&audio_sample, sizeof(audio_sample), 1, obj_file) != 1) {
      file_close_helper(pcm_file, obj_file);
      error_and_exit("Error writing to OBJ file");
    }
  }

  // Close files and check fclose success
  file_close_helper(pcm_file, obj_file);

  return 1;
}

void audio_init(void) {
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    error_and_exit("SDL_Init failed\n");
    return;
  }
  SDL_AudioSpec want = {0};
  want.freq = AUDIO_FREQUENCY;
  want.format = AUDIO_U16SYS;
  want.channels = 1;
  want.samples = AUDIO_SAMPLES;
  audio_device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);

  if (audio_device == 0) {
    error_and_exit("SDL_OpenAudioDevice failed");
    return;
  }
}

int queued_samples;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void audio_output(uint16_t audio_sample) {
  SDL_QueueAudio(audio_device, &audio_sample, sizeof(audio_sample));
  queued_samples++;

  if (queued_samples == AUDIO_QUEUE_LIMIT) {
    SDL_PauseAudioDevice(audio_device, 0);
    printf("Audio started\n");
  }
}

void audio_reset_buffering(void) {
  queued_samples = 0;
  SDL_ClearQueuedAudio(audio_device);
}

void audio_close(void) {
  SDL_CloseAudioDevice(audio_device);
  SDL_Quit();
}
