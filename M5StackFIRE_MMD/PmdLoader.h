#ifndef _PMDLOADER_H
#define _PMDLOADER_H

#include <M5Stack.h>
#include "PmdData.h"
#include "Polygon.h"

class PmdLoader {
  PmdMaterial pmdMaterial;
  unsigned int pmdVertex_start;
  unsigned int pmdFace_start;
  unsigned int pmdMaterial_start;
  File file;
  
  void LoadHeader();
  void GetVertexInfo();
  void GetFaceInfo();
  void GetMaterialInfo();
  void GetMaterial(Material* material);

public :
  uint32_t numPmdVertex;
  uint32_t numPmdFace;
  uint32_t numPmdMaterial;

  PmdLoader(String filename);
  ~PmdLoader();
  void GetVertices(Vector* pos, Vector* normal, Vector* texture_uv);
  void GetVertexIndices(uint16_t* vertexIndices);
  void GetMaterials(Material* materials);
  int GetNumTextures(Material* materials);
  void GetTextures(Material* materials, Texture* textures);
};

#endif // _PMDLOADER_H
