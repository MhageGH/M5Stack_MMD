#include <M5Stack.h>
#include "PixelShader.h"
#include "SimpleMotion.h"
#include "BitmapFactory.h"
#include "FpsMeter.h"
#include "Mesh.h"
#include "ARlikeMotion.h"
#define ENABLE_ARLIKE // uncomment and disenable ARlikeMotion

PixelShader* pixelShader;
FpsMeter* fpsMeter;
Mesh* mesh;
SimpleMotion* simpleMotion;
ARlikeMotion* arlikeMotion;

void setup(){
  M5.begin(); // This inludes Serial.begin(). Don't call it.
  delay(100);
  pixelShader = new PixelShader();
  fpsMeter = new FpsMeter();
  mesh = new Mesh("/dog.pmd");// File name should be ASCII
  //mesh = new Mesh("/kirby.pmd");
  //mesh = new Mesh("/ghoul.pmd");
#ifdef ENABLE_ARLIKE
  arlikeMotion = new ARlikeMotion();
  arlikeMotion->ReviseIMU();
#else
  simpleMotion = new SimpleMotion();
#endif
}

void loop() {
  const float IMU_update_period = 0.01f;  // [second]
  static float t = 0.0f;
  static float screen_update_period = 0.1f;  // updated every loop
  int divisor = mesh->numPolygon * IMU_update_period / screen_update_period + 1;
  Matrix matrix, matrixR;
#ifdef ENABLE_ARLIKE
  arlikeMotion->CreateMatrix(&matrix, &matrixR);
#else
  simpleMotion->CreateMatrix(&matrix, &matrixR, t);
#endif
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
#ifdef ENABLE_ARLIKE
    if (i % divisor == 0) arlikeMotion->ReviseIMU();  // call per IMU_update_period
#endif
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
  screen_update_period = 1.0f/fpsMeter->GetFps();
  t += screen_update_period;

///// if you want to create bitmap, comment out below /////
//  BitmapFactory::CreateBitmap(pixelShader->backBuffer, "/SS.bmp", pixelShader->_height, pixelShader->_width);
//  Serial.println("Bitmap Output finished");
//  while(1);

}
