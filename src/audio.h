#pragma once

#include <stdlib.h>
#include <stdio.h>

int process_audio(const char* image_path, const char* output_pcm); 

int pcm_to_obj(const char* output_pcm, const char* output_obj);
