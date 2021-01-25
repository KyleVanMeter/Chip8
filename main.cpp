#include "chip8.h"

#include <chrono>
#include <cstring>
#include <iostream>

#include <SDL2/SDL.h>

#define HEIGHT 32
#define WIDTH 64
#define NUM_SQUARES 2048

class View {
public:
  View(const std::string &title, int width, int height, int scale = 1) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      throw std::runtime_error(SDL_GetError());
    }

    _window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, width * scale,
                               height * scale, 0);
    _render = SDL_CreateRenderer(_window, -1, 0);
    _texture = SDL_CreateTexture(_render, SDL_PIXELFORMAT_RGB24,
                                 SDL_TEXTUREACCESS_STREAMING, width, height);

    if (!_window) {
      throw std::runtime_error(SDL_GetError());
    }
    if (!_render) {
      throw std::runtime_error(SDL_GetError());
    }
    if (!_texture) {
      throw std::runtime_error(SDL_GetError());
    }
  };

  ~View() {
    SDL_DestroyTexture(_texture);
    SDL_DestroyRenderer(_render);
    SDL_DestroyWindow(_window);
    SDL_Quit();
  };

  void SetFrame(unsigned char *data, int height) {
    void *pixeldata;
    int width;

    SDL_LockTexture(_texture, nullptr, &pixeldata, &width);
    std::memcpy(pixeldata, data, width * height);
    SDL_UnlockTexture(_texture);
  };

  void CopyToRGB24(unsigned char *in, unsigned char *data) {
    for (int i = 0; i < NUM_SQUARES; i++) {
      if (in[i] != 0) {
        data[i * 3] = 255;
        data[i * 3 + 1] = 0;
        data[i * 3 + 2] = 0;
      } else {
        data[i * 3] = 0;
        data[i * 3 + 1] = 0;
        data[i * 3 + 2] = 0;
      }
    }
  }

  void Update() {
    SDL_RenderCopy(_render, _texture, NULL, NULL);
    SDL_RenderPresent(_render);
  };

private:
  SDL_Window *_window;
  SDL_Renderer *_render;
  SDL_Texture *_texture;
};

chip8 Chip;

int main(int argc, char **argv) {
  std::chrono::time_point<std::chrono::steady_clock> begin, end;

  // Clear chip8 memory, and copy program into memory
  Chip.init();
  Chip.load("PONG");

  View viewer("Chip8", WIDTH, HEIGHT, 10);
  unsigned char *rgb = static_cast<unsigned char *>(
      std::calloc(NUM_SQUARES * 3, sizeof(unsigned char)));

  viewer.SetFrame(rgb, HEIGHT);

  try {
    for (;;) {
      begin = std::chrono::steady_clock::now();
      Chip.emuCycle();

      if (Chip.drawFlag) {
        viewer.CopyToRGB24(Chip.gfx, rgb);
        viewer.SetFrame(rgb, HEIGHT);
        viewer.Update();

        Chip.drawFlag = false;
      }

      // TODO: Clean this up for obvious reasons!
      // 16 is approximately 1/60, or 60 Hz in ms.
      // this is a do/while loop because even if the condition isn't true we
      // should still execute the loop at least once.  Basically we are trying
      // to wait until at least 1/60th of a second has passed for each emulation
      // cycle for usability / accuracy.
      end = std::chrono::steady_clock::now();
      do {
        Chip.setKeys();
        end = std::chrono::steady_clock::now();
      } while (
          std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
              .count() <= 2);
    }
  } catch (std::exception &e) {
    std::cerr << "ERR: " << e.what();
    std::exit(-1);
  }

  return 0;
}
