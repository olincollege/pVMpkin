#include "audio.h"

#include <SDL2/SDL.h>
#include <stdint.h>

#include "utils.h"

SDL_AudioDeviceID audio_device;

int process_audio(const char* audio_path, const char* output_pcm) {
  char command[512];
  snprintf(command, sizeof(command),
           "ffmpeg -y -i \"%s\" -t 5 -ar 8000 -ac 1 -f s16le \"%s\"",
           audio_path, output_pcm);

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
    fprintf(stderr, "Error: pcm_file failed\n");
    return 0;
  }

  FILE* obj_file = fopen(output_obj, "wb");
  if (!obj_file) {
    fprintf(stderr, "Error: obj_file failed\n");
    return 0;
  }

  uint16_t address = 0x5000;
  address = swap16(address);

  fwrite(&address, sizeof(address), 1, obj_file);

  while (1) {
    uint16_t audio_sample;
    size_t ret = fread(&audio_sample, sizeof(uint16_t), 1, pcm_file);

    if (ret == 0) {
      break;
    }

    audio_sample = swap16(audio_sample);

    fwrite(&audio_sample, sizeof(audio_sample), 1, obj_file);
  }

  fclose(pcm_file);
  fclose(obj_file);

  return 1;
}

void audio_init(void) {
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return;
  }
  SDL_AudioSpec want = {0};
  want.freq = 8000;
  want.format = AUDIO_U16SYS;
  want.channels = 1;
  want.samples = 5096;
  audio_device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);

  if (audio_device == 0) {
    fprintf(stderr, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
    return;
  }
}

static int queued_samples = 0;

void audio_output(uint16_t audio_sample) {
  SDL_QueueAudio(audio_device, &audio_sample, sizeof(audio_sample));
    queued_samples++;

    if (queued_samples == 500) {
        SDL_PauseAudioDevice(audio_device, 0);
        fprintf(stdout, "Audio started\n");
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
