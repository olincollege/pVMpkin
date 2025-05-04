#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum {
  AUDIO_ADDRESS = 0x1500,
  AUDIO_FREQUENCY = 12000,
  AUDIO_SAMPLES = 5096,
  AUDIO_QUEUE_LIMIT = 5000,
};

/**
 * @brief Converts an audio file to a raw PCM file.
 *
 * This function uses `ffmpeg` to extract a 5-second, 8000Hz, mono, signed
 * 16-bit PCM version of the input audio file and writes it to a specified
 * output file.
 *
 * @param audio_path Path to the input audio file (e.g., .mp3, .wav).
 * @param output_pcm Path where the generated PCM file will be written.
 * @return 0 on success, exits the program on failure.
 */
int process_audio(const char* audio_path, const char* output_pcm);

/**
 * @brief Converts a raw PCM file into a binary object (.obj) file for memory
 * loading.
 *
 * This function reads PCM samples, endian-swaps each 16-bit value, and writes
 * them into an object file starting from a fixed memory address.
 *
 * @param output_pcm Path to the PCM file to read.
 * @param output_obj Path where the generated object file will be written.
 * @return 1 on success, exits the program on failure.
 */
int pcm_to_obj(const char* output_pcm, const char* output_obj);

/**
 * @brief Initializes SDL audio playback.
 *
 * This function initializes the SDL audio subsystem and opens an audio device
 * with a sample rate of 8000Hz, mono channel, and 16-bit signed samples.
 * It prepares the audio system for later output.
 *
 * Exits the program if initialization fails.
 */
void audio_init(void);

/**
 * @brief Queues a single 16-bit audio sample for playback.
 *
 * This function queues the provided audio sample to the SDL audio device.
 * Once a certain number of samples are queued (AUDIO_QUEUE_LIMIT),
 * playback automatically starts.
 *
 * @param audio_sample The 16-bit audio sample to queue.
 */
void audio_output(uint16_t audio_sample);

/**
 * @brief Shuts down the SDL audio subsystem.
 *
 * This function stops the audio device, closes it, and quits the SDL
 * audio subsystem cleanly. Should be called before program exit.
 */
void audio_close(void);
