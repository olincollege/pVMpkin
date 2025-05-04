#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdint.h>

#include "../src/audio.h"

// NOLINTBEGIN

extern int queued_samples;

// --- Test audio_output increments queued_samples correctly ---

Test(audio_output, increments_queued_samples) {
  queued_samples = 0;
  uint16_t sample = 0xAAAA;

  audio_output(sample);

  cr_assert(eq(int, queued_samples, 1),
            "Queued samples should increment by 1 after audio_output");
}

// NOLINTEND
