#include <M5Stack.h>
#include "PixelShader.h"
#include "MatrixFactory.h"
#include "BitmapFactory.h"
#include "FpsMeter.h"
#include "Mesh.h"

PixelShader* pixelShader;
FpsMeter* fpsMeter;
Mesh* mesh;

void setup(){
  M5.begin();
  delay(100);
  // Serial.begin(115200); // Don't call it! M5.begin() includes it. If it is called again, some problems occcurs as freeze.
  pixelShader = new PixelShader();
  fpsMeter = new FpsMeter();
  mesh = new Mesh("/Shanghai.pmd");// File name should be ASCII
}

void loop() {
  static float t = 0.0f;
  Matrix matrix, matrixR;
  MatrixFactory::CreateMatrix(&matrix, &matrixR, t);
  for (int i = 0; i < mesh->numVertex; ++i) {
    Affine::Dot(&matrix, &mesh->pos0[i], &mesh->pos[i]);
    Affine::Dot(&matrixR, &mesh->normal0[i], &mesh->normal[i]);
  }
  int materialIndex = 0;
  int face_vert_count = 0;
  int face_vert_count_max = mesh->materials[materialIndex].face_vert_count;
  pixelShader->Clear(0x39E7); // gray  
  Polygon polygon;
  for (int i = 0; i < mesh->numPolygon; ++i) {
    for (int j = 0; j < 3; ++j){
      uint16_t vertexIndex = mesh->vertexIndices[3*i + j];
      polygon.vertices[j] = mesh->pos[vertexIndex];
      polygon.normal[j] = mesh->normal[vertexIndex];
      polygon.texture_uv[j] = mesh->texture_uv[vertexIndex];
    }
    if (face_vert_count == 0) { // Material setting
      pixelShader->SetMaterial(&mesh->materials[materialIndex]);
      if (mesh->materials[materialIndex].texture_file_name[0] != 0) { // Texture setting
        for (int i = 0; i < mesh->numTextures; ++i) {                 // search by texture file name
          if (strcmp((char*)mesh->materials[materialIndex].texture_file_name, (char*)mesh->textures[i].texture_file_name) == 0) {
            pixelShader->SetTexture(&mesh->textures[i]);
            break;
          }
        }
      }
    }
    face_vert_count += 3;
    if (face_vert_count == face_vert_count_max) {
      ++materialIndex;
      face_vert_count_max = mesh->materials[materialIndex].face_vert_count;
      face_vert_count = 0;
    }
    pixelShader->DrawPolygon(&polygon);
  }
  pixelShader->Flip();
  fpsMeter->Run();
  Serial.println("FPS = " + String(fpsMeter->GetFps()));
  t += 1.0f/fpsMeter->GetFps();

///// if you want to create bitmap, comment out below /////
//  BitmapFactory::CreateBitmap(pixelShader->backBuffer, "/SS.bmp", pixelShader->_height, pixelShader->_width);
//  Serial.println("Bitmap Output finished");
//  while(1);

}
