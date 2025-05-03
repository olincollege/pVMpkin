#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "audio.h"
#include "instructions.h"
#include "memory.h"
#include "trapping.h"
#include "utils.h"

// NOLINTBEGIN(cppcoreguidelines-macro-to-enum, modernize-macro-to-enum)
#define PC_START 0x3000
#define WINDOW_SIZE 1024
#define MEMORY_MAP_DIM 256
#define OPCODE_SHIFT (uint16_t)12
#define FIRST_8BIT_MASK (uint16_t)0xFF
// NOLINTEND(cppcoreguidelines-macro-to-enum, modernize-macro-to-enum)


int main(int argc, const char* argv[]) {
  Uint32 last_frame_time = 0;
  const Uint32 frame_delay = 1000 / 60;  // 60 FPS

  if (argc < 2) {
    /* show instructions on how to use */
    error_and_exit("main [audio-file1] ...\n");
  }

  audio_init();

  if (!read_image("../player.obj")) {
    error_and_exit("Failed to load audio player\n");
  }

  if (!read_image(argv[1])) {
    error_and_exit("Failed to load audio\n");
  }

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    error_and_exit("Failed to inialize SDL\n");
  }

  SDL_Window* window = SDL_CreateWindow(
      "Memory Map", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE,
      WINDOW_SIZE, (Uint32)SDL_WINDOW_SHOWN | (Uint32)SDL_WINDOW_RESIZABLE);
  SDL_Renderer* renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_Texture* texture =
      SDL_CreateTexture(renderer,
                        SDL_PIXELFORMAT_RGBA8888,     // 32-bit texture
                        SDL_TEXTUREACCESS_STREAMING,  // update every frame
                        MEMORY_MAP_DIM, MEMORY_MAP_DIM);

  disable_input_buffering();

  /* since one condition flag should be set at all times, set the Z flag*/
  reg[R_COND] = FL_ZRO;

  /* set the PC to starting position (0x3000 is default)*/
  reg[R_PC] = PC_START;

  int running = 1;

  while (running) {
    uint32_t instr = mem_read(reg[R_PC]++);
    uint16_t opcode = (uint16_t)instr >> OPCODE_SHIFT;

    /* TODO: Implement Memory Map */
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        restore_input_buffering();
        audio_close();
        printf("Exited Gracefully\n");
        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        exit(0);
      }
    }
    Uint32 current_time = SDL_GetTicks();

    if (current_time - last_frame_time >= frame_delay) {  // 60 FPS
      /* update the frame */
      update_texture(texture, memory);
      SDL_RenderClear(renderer);

      SDL_Rect dest_rect = {0, 0, WINDOW_SIZE, WINDOW_SIZE};
      SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
      SDL_RenderPresent(renderer);
      last_frame_time = current_time;
    }

    switch (opcode) {
      case OP_ADD:
        add_instr(instr);
        break;
      case OP_AND:
        and_instr(instr);
        break;
      case OP_NOT:
        not_instr(instr);
        break;
      case OP_BR:
        branch_instr(instr);
        break;
      case OP_JMP:
        jump_instr(instr);
        break;
      case OP_JSR:
        jump_register_instr(instr);
        break;
      case OP_LD:
        load_instr(instr);
        break;
      case OP_LDI:
        ldi_instr(instr);
        break;
      case OP_LDR:
        load_reg_instr(instr);
        break;
      case OP_LEA:
        load_eff_addr_instr(instr);
        break;
      case OP_ST:
        store_instr(instr);
        break;
      case OP_STI:
        store_indirect_instr(instr);
        break;
      case OP_STR:
        store_reg_instr(instr);
        break;
      case OP_TRAP:
        reg[R_R7] = reg[R_PC];

        switch (instr & FIRST_8BIT_MASK) {
          case TRAP_GETC:
            trap_getc();
            break;
          case TRAP_OUT:
            trap_out();
            break;
          case TRAP_PUTS:
            trap_puts();
            break;
          case TRAP_IN:
            trap_in();
            break;
          case TRAP_PUTSP:
            trap_putsp();
            break;
          case TRAP_HALT:
            trap_halt(&running);
            break;
          default:
            printf("Unknown trapcode\n");
            running = 0;
            break;
        }
        break;

      default:
        printf("Unknown opcode: 0x%X\n", opcode);
        running = 0;
        break;
    }
  }
  // SDL_DestroyTexture(texture);
  // SDL_DestroyRenderer(renderer);
  // SDL_DestroyWindow(window);
  // SDL_Quit();
  // restore_input_buffering();  // restores the terminal back to normal
  // audio_close();
}
