#include "view.hpp"

#include <cstring>
#include <iostream>

View::View(const std::string &title, int width, int height, int scale)
    : _width(width), _height(height) {
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
}

View::~View() {
  SDL_DestroyTexture(_texture);
  SDL_DestroyRenderer(_render);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void View::SetFrame(unsigned char *data, int height) {
  void *pixeldata;
  int width;

  SDL_LockTexture(_texture, nullptr, &pixeldata, &width);
  std::memcpy(pixeldata, data, width * height);
  SDL_UnlockTexture(_texture);
}

void View::CopyToRGB24(unsigned char *in, unsigned char *data) {
  for (int i = 0; i < (_width * _height); i++) {
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

void View::Update() {
  SDL_RenderCopy(_render, _texture, NULL, NULL);
  SDL_RenderPresent(_render);
}
