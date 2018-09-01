#ifndef _POLYGON_H
#define _POLYGON_H

#include "Affine.h"

class Polygon {
public :
  Vector vertices[3];
  Vector normal[3];
  Vector texture_uv[3];
};

class Material {
public : 
  uint16_t color;
  byte texture_file_name[21];
  uint32_t face_vert_count;
  bool useCulling;
};

class Texture {
public :
  byte texture_file_name[21]; // File name should be ASCII. File type should be ARGB(1-5-5-5) 128x128pixel bitmap.
  byte imageData[2*128*2*128];
};

#endif // _POLYGON_H
