#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int process_audio(const char* image_path, const char* output_pcm);

int pcm_to_obj(const char* output_pcm, const char* output_obj);

void audio_init(void);

void audio_output(uint16_t audio_sample);

void audio_close(void);
