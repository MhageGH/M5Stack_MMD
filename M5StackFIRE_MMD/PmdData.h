#pragma once

const int PmdHeaderSize = 1*3 + 4*1 + 1*20 + 1*256;
const int PmdVertexSize = 4*3 + 4*3 + 4*2 + 2*2 + 1*1 + 1*1;
const int PmdMaterialSize = 4*3 + 4*1 + 4*1 + 4*3 + 4*3 + 1*1 + 1*1 + 4*1 + 1*20;

class PmdHeader {
public:
  byte magic[3];
  float _version;
  byte model_name[20];
  byte comment[256];
};

class PmdVertex {
public: 
  float pos[3];
  float normal_vec[3];
  float uv[2];
  uint16_t bone_num[2];
  byte bone_weight;
  byte edge_flag;
};

class PmdMaterial {
public:
  float diffuse_color[3];
  float alpha;
  float specularity;
  float specular_color[3];
  float mirror_color[3];
  byte toon_index;
  byte edge_flag;
  uint32_t face_vert_count; // the number of vertex with this material. The PMD vertex is arranged in material number order.
  byte texture_file_name[20];
  PmdMaterial() { for (int i = 0; i < 20; ++i) texture_file_name[i] = 0; }
};

