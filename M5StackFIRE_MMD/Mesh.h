#ifndef _MESH_H
#define _MESH_H
#include "Polygon.h"
#include "PmdLoader.h"

class Mesh {
  void *pPos0, *pNormal0, *pPos, *pNormal, *pTexture_uv;
public:
  uint32_t numVertex;
  int numPolygon;
  int numTextures;

  // buffer
  Vector *pos0, *normal0; // initial 
  Vector *pos, *normal;
  Vector *texture_uv;
  uint16_t *vertexIndices;
  Material *materials;
  Texture* textures;  

  Mesh(String filename);
  ~Mesh();
};

#endif // _MESH_H
