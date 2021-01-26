#ifndef __VIEW_H__
#define __VIEW_H__

#include <SDL2/SDL.h>
#include <string>

class View {
public:
  View(const std::string &title, int width, int height, int scale = 1);
  ~View();

  void SetFrame(unsigned char *data, int height);
  void CopyToRGB24(unsigned char *in, unsigned char *data);
  void Update();
private:
  int _width, _height;
  SDL_Window *_window;
  SDL_Renderer *_render;
  SDL_Texture *_texture;
};

#endif
