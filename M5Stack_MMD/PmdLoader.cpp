#include "PmdLoader.h"

void PmdLoader::file_read(byte* buf, int size_of_buf) {
  for (int i = 0; i < size_of_buf; ++i) buf[i] = *file++;
}
void PmdLoader::LoadHeader() {
  polygonIndex = 0;
  PmdHeader pmdHeader;
  file_read((uint8_t*)(pmdHeader.magic), sizeof(pmdHeader.magic));
  file_read((uint8_t*)(&pmdHeader._version), sizeof(pmdHeader._version));
  file_read((uint8_t*)(pmdHeader.model_name), sizeof(pmdHeader.model_name));
  file_read((uint8_t*)(pmdHeader.comment), sizeof(pmdHeader.comment));
  char temp[4] = {0};
  for (int i = 0; i < sizeof(pmdHeader.magic); ++i) temp[i] = (char)(pmdHeader.magic[i]);
  Serial.print("pmdHeader.magic: ");
  Serial.println(temp);
  Serial.print("pmdHeader._version: ");
  Serial.println(pmdHeader._version);
}

void PmdLoader::GetVertexInfo() {
  file_read((uint8_t*)&numPmdVertex, sizeof(numPmdVertex));
  pmdVertex_start = file;
  file = pmdVertex_start + numPmdVertex*PmdVertexSize;
  Serial.print("numPmdVertex: ");
  Serial.println(numPmdVertex);
}

void PmdLoader::GetFaceInfo() {
  file_read((uint8_t*)&numPmdFace, sizeof(numPmdFace));
  pmdFace_start = file;  
  file = pmdFace_start + numPmdFace*sizeof(uint16_t);
  Serial.print("numPmdFace: ");
  Serial.println(numPmdFace); 
}

void PmdLoader::GetMaterialInfo() {
  file_read((uint8_t*)&numPmdMaterial, sizeof(numPmdMaterial));
  Serial.print("numPmdMaterial: ");
  Serial.println(numPmdMaterial);
  pmdMaterial_start = file;
}

PmdLoader::PmdLoader(const byte* file) {
  this->file = file;
  this->file_start = file;
  face_vert_count = 0;
  materialIndex = 0;
  LoadHeader();
  GetVertexInfo();
  GetFaceInfo();
  GetMaterialInfo();
  Serial.println();
}

bool PmdLoader::GetPolygon(Polygon* polygon, bool* materialChanged) {
  if(polygonIndex >= numPmdFace/3) {
    polygonIndex = 0;
    face_vert_count = 0;
    materialIndex = 0;
    return false;
  }
  if (face_vert_count == 0) {
    file = pmdMaterial_start + materialIndex*PmdMaterialSize;
    file_read((uint8_t*)pmdMaterial.diffuse_color, sizeof(pmdMaterial.diffuse_color));
    file_read((uint8_t*)&pmdMaterial.alpha, sizeof(pmdMaterial.alpha));
    file_read((uint8_t*)&pmdMaterial.specularity, sizeof(pmdMaterial.specularity));
    file_read((uint8_t*)pmdMaterial.specular_color, sizeof(pmdMaterial.specular_color));
    file_read((uint8_t*)pmdMaterial.mirror_color, sizeof(pmdMaterial.mirror_color));
    file_read((uint8_t*)&pmdMaterial.toon_index, sizeof(pmdMaterial.toon_index));
    file_read((uint8_t*)&pmdMaterial.edge_flag, sizeof(pmdMaterial.edge_flag));
    file_read((uint8_t*)&pmdMaterial.face_vert_count, sizeof(pmdMaterial.face_vert_count));
    file_read((uint8_t*)pmdMaterial.texture_file_name, sizeof(pmdMaterial.texture_file_name));
    face_vert_count = pmdMaterial.face_vert_count;
    ++materialIndex;
    *materialChanged = true;
  }
  else *materialChanged = false;
  uint16_t vertexIndex[3];
  file = pmdFace_start + polygonIndex*sizeof(vertexIndex);
  file_read((uint8_t*)vertexIndex, sizeof(vertexIndex));
  for (int i = 0; i < 3; ++i) {
    PmdVertex pmdVertex;
    file = pmdVertex_start + vertexIndex[i]*PmdVertexSize;
    file_read((uint8_t*)pmdVertex.pos, sizeof(pmdVertex.pos));
    file_read((uint8_t*)pmdVertex.normal_vec, sizeof(pmdVertex.normal_vec));
    file_read((uint8_t*)pmdVertex.uv, sizeof(pmdVertex.uv));
    polygon->vertices[i].x = pmdVertex.pos[0];
    polygon->vertices[i].y = pmdVertex.pos[1];
    polygon->vertices[i].z = pmdVertex.pos[2];
    polygon->normal[i].x = pmdVertex.normal_vec[0];
    polygon->normal[i].y = pmdVertex.normal_vec[1];
    polygon->normal[i].z = pmdVertex.normal_vec[2];
    polygon->texture_uv[i].x = pmdVertex.uv[0];
    polygon->texture_uv[i].y = pmdVertex.uv[1];
  }
  ++polygonIndex;
  face_vert_count -= 3;
  return true;
}

void PmdLoader::GetMaterial(Material* material) {
  float R = pmdMaterial.diffuse_color[0];
  float G = pmdMaterial.diffuse_color[1];
  float B = pmdMaterial.diffuse_color[2];
  uint16_t r = (uint16_t)((float)0x1Fu*R); 
  uint16_t g = (uint16_t)((float)0x3Fu*G); 
  uint16_t b = (uint16_t)((float)0x1Fu*B); 
  material->color = (r << 11) | (g << 5) | b;
  for (int i = 0; i < 20; ++i) {
    if (pmdMaterial.texture_file_name[i] != '*') material->texture_file_name[i] = pmdMaterial.texture_file_name[i];
    else material->texture_file_name[i] = 0;
  }
  material->texture_file_name[20] = 0;
  
//  Serial.println("Material " + String(materialIndex));
//  Serial.println("color : "+ String(material->color, BIN));
//  Serial.println("texture_file_name : "+ String((char*)material->texture_file_name));
//  Serial.println();
}

