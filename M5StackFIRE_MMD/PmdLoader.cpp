#include "PmdLoader.h"

void PmdLoader::LoadHeader() {
  PmdHeader pmdHeader;
  file.read((byte*)pmdHeader.magic, sizeof(pmdHeader.magic));
  file.read((byte*)(&pmdHeader._version), sizeof(pmdHeader._version));
  file.read((byte*)pmdHeader.model_name, sizeof(pmdHeader.model_name));
  file.read((byte*)pmdHeader.comment, sizeof(pmdHeader.comment));
  char temp[4] = {0};
  for (int i = 0; i < sizeof(pmdHeader.magic); ++i) temp[i] = (char)(pmdHeader.magic[i]);
  Serial.print("pmdHeader.magic: ");
  Serial.println(temp);
  Serial.print("pmdHeader._version: ");
  Serial.println(pmdHeader._version);
}

void PmdLoader::GetVertexInfo() {
  file.read((byte*)(&numPmdVertex), sizeof(numPmdVertex));
  pmdVertex_start = file.position();
  file.seek(pmdVertex_start + numPmdVertex*PmdVertexSize);
  Serial.print("numPmdVertex: ");
  Serial.println(numPmdVertex);
}

void PmdLoader::GetFaceInfo() {
  file.read((byte*)(&numPmdFace), sizeof(numPmdFace));
  pmdFace_start = file.position();
  file.seek(pmdFace_start + numPmdFace*sizeof(uint16_t));
  Serial.print("numPmdFace: ");
  Serial.println(numPmdFace); 
}

void PmdLoader::GetMaterialInfo() {
  file.read((byte*)(&numPmdMaterial), sizeof(numPmdMaterial));
  pmdMaterial_start = file.position();
  Serial.print("numPmdMaterial: ");
  Serial.println(numPmdMaterial);
}

PmdLoader::PmdLoader(String filename) {
  file = SD.open(filename);
  Serial.println("filename: " + filename);
  LoadHeader();
  GetVertexInfo();
  GetFaceInfo();
  GetMaterialInfo();
  Serial.println();
}

PmdLoader::~PmdLoader() {
  file.close();
}

void PmdLoader::GetVertices(Vector* pos, Vector* normal, Vector* texture_uv) {
  for (int i = 0; i < numPmdVertex; ++i) {
    PmdVertex pmdVertex;
    file.seek(pmdVertex_start + i*PmdVertexSize);
    file.read((byte*)pmdVertex.pos, sizeof(pmdVertex.pos));
    file.read((byte*)pmdVertex.normal_vec, sizeof(pmdVertex.normal_vec));
    file.read((byte*)pmdVertex.uv, sizeof(pmdVertex.uv));
    pos[i].x = pmdVertex.pos[0];
    pos[i].y = pmdVertex.pos[1];
    pos[i].z = pmdVertex.pos[2];
    normal[i].x = pmdVertex.normal_vec[0];
    normal[i].y = pmdVertex.normal_vec[1];
    normal[i].z = pmdVertex.normal_vec[2];
    texture_uv[i].x = pmdVertex.uv[0];
    texture_uv[i].y = pmdVertex.uv[1];
    texture_uv[i].z = 0;
  }
}

void PmdLoader::GetVertexIndices(uint16_t* vertexIndices) {
  file.seek(pmdFace_start);
  file.read((byte*)vertexIndices, numPmdFace*sizeof(uint16_t));
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
    if (pmdMaterial.texture_file_name[i] == '*') {
      material->texture_file_name[i] = 0;
      break;
    }
    material->texture_file_name[i] = pmdMaterial.texture_file_name[i];
    const char sph[] = ".sph";
    bool sph_match = false;
    if ((i < 20 - strlen(sph)) && (strncmp((char*)(pmdMaterial.texture_file_name + i), sph, strlen(sph)) == 0)) sph_match = true;
    const char spa[] = ".spa";
    bool spa_match = false;
    if ((i < 20 - strlen(spa)) && (strncmp((char*)(pmdMaterial.texture_file_name + i), spa, strlen(spa)) == 0)) spa_match = true;
    if (sph_match || spa_match) {
      material->texture_file_name[0] = 0;
      break;
    }
  }
  material->texture_file_name[20] = 0;
  material->face_vert_count = pmdMaterial.face_vert_count;
  material->useCulling = pmdMaterial.alpha > 0.999 ? true : false;
}

void PmdLoader::GetMaterials(Material* materials) {
  for (int i = 0; i < numPmdMaterial; ++i){
    file.seek(pmdMaterial_start + i*PmdMaterialSize);
    file.read((byte*)pmdMaterial.diffuse_color, sizeof(pmdMaterial.diffuse_color));
    file.read((byte*)(&pmdMaterial.alpha), sizeof(pmdMaterial.alpha));
    file.seek(file.position() + 30);  // sizeof(pmdMaterial.specularity) + sizeof(pmdMaterial.specular_color) + sizeof(pmdMaterial.mirror_color) + sizeof(pmdMaterial.toon_index) + sizeof(pmdMaterial.edge_flag) = 4 + 4*3 + 4*3 + 1 + 1 = 30
    file.read((byte*)(&pmdMaterial.face_vert_count), sizeof(pmdMaterial.face_vert_count));
    file.read((byte*)pmdMaterial.texture_file_name, sizeof(pmdMaterial.texture_file_name));
    GetMaterial(&materials[i]);
  }
}

int PmdLoader::GetNumTextures(Material* materials) {
  int numTextures = 0;
  for (int i = 0; i < numPmdMaterial; ++i){
    if (!materials[i].texture_file_name[0]) continue;
    bool existing = false;
    for (int j = 0; j < i; ++j) if (strcmp((char*)materials[i].texture_file_name, (char*)materials[j].texture_file_name) == 0) existing = true;
    if (!existing) ++numTextures;
  }
  return numTextures;
}

void PmdLoader::GetTextures(Material* materials, Texture* textures) {
  int k = 0;
  for (int i = 0; i < numPmdMaterial; ++i){
    if (!materials[i].texture_file_name[0]) continue;
    bool existing = false;
    for (int j = 0; j < i; ++j) if (strcmp((char*)materials[i].texture_file_name, (char*)materials[j].texture_file_name) == 0) existing = true;
    if (existing) continue;
    memcpy(textures[k].texture_file_name, materials[i].texture_file_name, sizeof(materials[i].texture_file_name));
    uint32_t header_offset, biWidth, biHeight;
    if (SD.exists("/" + String((char*)textures[k].texture_file_name)) == false) {
      Serial.println("Texture file " + String((char*)textures[k].texture_file_name) + " not found!");
      while(1);
    }
    file = SD.open("/" + String((char*)textures[k].texture_file_name));
    file.seek(10);
    file.read((uint8_t*)&header_offset, sizeof(header_offset));
    file.seek(18);
    file.read((uint8_t*)&biWidth, sizeof(biWidth));
    file.read((uint8_t*)&biHeight, sizeof(biHeight));
    if (biWidth != 128 || biHeight != 128) {
      Serial.println("wrong texture size!");
      while(1);
    }
    file.seek(header_offset);
    file.read((uint8_t*)textures[k].imageData, sizeof(uint16_t)*biHeight*biWidth);
    ++k;
  }
}

