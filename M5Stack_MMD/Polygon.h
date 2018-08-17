#ifndef _POLYGON_H
#define _POLYGON_H

#include "Affine.h"

class Polygon {
public :
//  uint16_t color;
  Vector vertices[3];
  Vector normal[3];
  Vector texture_uv[3];
  Polygon(){}
};

class Material {
public : 
  uint16_t color;
  byte texture_file_name[21];
};

#endif // _POLYGON_H
