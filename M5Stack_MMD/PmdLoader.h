#ifndef _PMDLOADER_H
#define _PMDLOADER_H

#include <M5Stack.h>
#include "PmdData.h"
#include "Polygon.h"

class PmdLoader {
  const byte* file_start;
  const byte* file;
  uint32_t numPmdVertex;
  const byte* pmdVertex_start;
  uint32_t numPmdFace;
  const byte* pmdFace_start;
  uint32_t numPmdMaterial;
  const byte* pmdMaterial_start;
  PmdMaterial pmdMaterial;
  uint16_t polygonIndex;
  uint32_t face_vert_count;
  uint16_t materialIndex;
  
  void LoadHeader();
  void GetVertexInfo();
  void GetFaceInfo();
  void GetMaterialInfo();
  void file_read(byte* buf, int size_of_buf);
public :
  PmdLoader(const byte* file);
  bool GetPolygon(Polygon* polygon, bool* materialChanged);  // return : existance of remain
  void GetMaterial(Material* material);
};

#endif // _PMDLOADER_H
