#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

class EasyImage {
public:
  EasyImage() {
    _Width = 0;
    _Height = 0;
    _Channels = 0;
    _Pixels = nullptr;
    _Loaded = false;
  }

  ~EasyImage() {
    destroy();
  }

  EasyImage(const char* path) {
    load(path);
  }

  void load(const char* path) {
    if (_Loaded) {
      throw std::logic_error("EasyImage was already loaded");
    }

    _Pixels = stbi_load(path, &_Width, &_Height, &_Channels, STBI_rgb_alpha);

    if (_Pixels == nullptr) {
      throw std::invalid_argument("Could not load image or image is corrupt");
    }

    _Loaded = true;
  }

  void destroy() {
    if (_Loaded) {
      stbi_image_free(_Pixels);
      _Loaded = false;
    }
  }

  int getHeight() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }

    return _Height;
  }

  int getWidth() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }

    return _Width;
  }

  int getChannels() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }
    return 4;
  }

  int getSizeInBytes() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }
    return getWidth() * getHeight() * getChannels();
  }

  stbi_uc* getRaw() {
    if (!_Loaded) {
      throw std::logic_error("EasyImage not yet loaded!");
    }
    return _Pixels;
  }

private:
  int _Width;
  int _Height;
  int _Channels;
  stbi_uc* _Pixels;
  bool _Loaded;

};