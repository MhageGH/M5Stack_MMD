#ifndef _PIXELSHADER_H
#define _PIXELSHADER_H

#include <M5Stack.h>
#include "Polygon.h"

class PixelShader {
  const int Height, Width;    // all screen size
  const int _height, _width;  // 3D screen size
  const int txHeight, txWidth; // texture size
  const int16_t minZ;         // min value of zBuffer

  // buffer
  int16_t* zBuffer;          // z value multiplied by 10
  uint16_t* backBuffer;
  uint16_t* txBuffer;

  // material
  Material *material;
  Vector light;
  bool textureEnable;
  
  void DrawHLine(int32_t x, int32_t y, int32_t w, Polygon* polygon);
  bool Culling(Polygon* polygon);
  uint16_t DecayBrightness(uint16_t color, float rate);
  
 public: 
  PixelShader(Vector light0 = Vector(0, 0, 1, 1));
  ~PixelShader();
  void DrawPolygon(Polygon* polygon);
  void Flip();
  void Clear(uint16_t color);
  void SetMaterial(Material* material); 
  void SetTexture(Texture* texture); 
};

#endif // _PIXELSHADER_H
