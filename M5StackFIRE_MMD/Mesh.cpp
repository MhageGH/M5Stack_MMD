#include "Mesh.h"

Mesh::Mesh(String filename) {
  PmdLoader* pmdLoader = new PmdLoader(filename);
  numVertex = pmdLoader->numPmdVertex;
  if (!(pPos0 = ps_malloc(sizeof(Vector)*numVertex))) Serial.println("pos0 allocation failed!");
  pos0 = new(pPos0) Vector[numVertex];            // use placement new to call constructor of Vector
  if(!(pNormal0 = ps_malloc(sizeof(Vector)*numVertex))) Serial.println("normal0 allocation failed!");
  normal0 = new(pNormal0) Vector[numVertex];
  if (!(pPos = ps_malloc(sizeof(Vector)*numVertex))) Serial.println("pos allocation failed!");
  pos = new(pPos) Vector[numVertex];
  if(!(pNormal = ps_malloc(sizeof(Vector)*numVertex))) Serial.println("normal allocation failed!");
  normal = new(pNormal) Vector[numVertex];
  if(!(pTexture_uv = ps_malloc(sizeof(Vector)*numVertex))) Serial.println("texture_uv allocation failed!");
  texture_uv = new(pTexture_uv) Vector[numVertex];
  pmdLoader->GetVertices(pos0, normal0, texture_uv);
  uint32_t numVertexIndices = pmdLoader->numPmdFace;
  if(!(vertexIndices = (uint16_t*)ps_malloc(sizeof(uint16_t)*numVertexIndices))) Serial.println("vertexIndices allocation failed!");
  pmdLoader->GetVertexIndices(vertexIndices);
  uint32_t numMaterial = pmdLoader->numPmdMaterial;
  if(!(materials = (Material*)ps_malloc(sizeof(Material)*numMaterial))) Serial.println("materials allocation failed!");
  pmdLoader->GetMaterials(materials);
  numPolygon = pmdLoader->numPmdFace/3;
  numTextures = pmdLoader->GetNumTextures(materials);
  if(!(textures = (Texture*)ps_malloc(sizeof(Texture)*numTextures))) Serial.println("textures allocation failed!");
  pmdLoader->GetTextures(materials, textures);
  delete pmdLoader;
}

Mesh::~Mesh() {
  free(pPos0);  // You need to call destructor explicitly when using placement new. But Vector class does not have destructor.
  free(pNormal0);
  free(pPos);
  free(pNormal);
  free(pTexture_uv);
  free(vertexIndices);
  free(materials);
  free(textures);
}

