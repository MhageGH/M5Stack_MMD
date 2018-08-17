#ifndef _PIXELSHADER_H
#define _PIXELSHADER_H

#include <M5Stack.h>
#include "Polygon.h"

class PixelShader {
  const int Height, Width;    // all screen size
  const int16_t minZ;         // min value of zBuffer
  int16_t** zBuffer;          // z value multiplied by 10
  Vector light;
  void DrawHLine(int32_t x, int32_t y, int32_t w, Polygon* polygon, bool gouraud, bool texture);
  void CreateNormal(Polygon* polygon);
  Material material;
  uint16_t plainColor;
  uint32_t header_offset, biWidth, biHeight;  // for texture file
  bool textureFileAvailable;
  
  const byte* file_start;
  const byte* file;
  void file_read(byte* buf, int size_of_buf);

 public: 
  const int _height, _width;  // 3D screen size
  uint16_t** backBuffer;
  PixelShader(Vector light0 = Vector(0, 0, 1, 1));
  ~PixelShader();
  void DrawPolygon(Polygon* polygon, bool gouraud = false, bool texture = false);
  void Flip();
  void Clear(uint16_t color);

  // Texture file name must have only ASCII. The file type is ARGB(1-5-5-5) bitmap. 
  void SetMaterial(Material* material);  
  
};

#endif // _PIXELSHADER_H
