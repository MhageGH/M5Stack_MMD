#include <M5Stack.h>
#include "PixelShader.h"
#include "MatrixFactory.h"
#include "BitmapFactory.h"
#include "FpsMeter.h"
#include "Mesh.h"
#include "utility/MPU9250.h"
#include "quaternionFilters2.h"

PixelShader* pixelShader;
FpsMeter* fpsMeter;
Mesh* mesh;
MPU9250 IMU;

void InitAR() {
  Wire.begin();
  byte c = IMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  IMU.initMPU9250();
}

void CreateARMatrix(Matrix* matrix, Matrix* matrixR) {
  Matrix matrixAR, matrixRx, matrixRy, matrixRz, matrixScale, matrixT, matrixTR, temp;
  float r[3][3] = {{*getR2(), *(getR2()+3), *(getR2()+6)}, {*(getR2()+1), *(getR2()+4), *(getR2()+7)}, {*(getR2()+2), *(getR2()+5), *(getR2()+8)}};
  // matrixR->m11 = +r[0][0]; matrixR->m12 = r[0][2]; matrixR->m13 = -r[0][1]; matrixR->m14 = 0;
  // matrixR->m21 = -r[1][0]; matrixR->m22 = -r[1][2]; matrixR->m23 = -r[1][1]; matrixR->m24 = 0;
  // matrixR->m31 = +r[2][0]; matrixR->m32 = r[2][2]; matrixR->m33 = +r[2][1]; matrixR->m34 = 0;
  // matrixR->m41 = 0; matrixR->m42 = 0; matrixR->m43 = 0; matrixR->m44 = 1;

  matrixAR.m11 = -r[0][0]; matrixAR.m12 = r[0][1]; matrixAR.m13 = r[0][2]; matrixAR.m14 = 0;
  matrixAR.m21 = +r[1][0]; matrixAR.m22 = -r[1][1]; matrixAR.m23 = -r[1][2]; matrixAR.m24 = 0;
  matrixAR.m31 = -r[2][0]; matrixAR.m32 = r[2][1]; matrixAR.m33 = r[2][2]; matrixAR.m34 = 0;
  matrixAR.m41 = 0; matrixAR.m42 = 0; matrixAR.m43 = 0; matrixAR.m44 = 1;
  Affine::CreateRotationMatrixX(PI/2, &matrixRx);
  Affine::CreateRotationMatrixZ(PI, &matrixRz);
  Affine::Dot(&matrixRz, &matrixRx, &temp);
  Affine::Dot(&matrixAR, &temp, matrixR);
  Affine::CreateTranslationMatrix(120, 100, 0, &matrixT);
  Affine::CreateScalingMatrix(18, &matrixScale);
  Affine::Dot(&matrixT, matrixR, &matrixTR);
  Affine::Dot(&matrixTR, &matrixScale, matrix);
}

void ReviseIMU() {
 if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
    {  
      IMU.readAccelData(IMU.accelCount);
      IMU.getAres();

      IMU.ax = (float)IMU.accelCount[0]*IMU.aRes;
      IMU.ay = (float)IMU.accelCount[1]*IMU.aRes;
      IMU.az = (float)IMU.accelCount[2]*IMU.aRes;

      IMU.readGyroData(IMU.gyroCount);
      IMU.getGres();

      IMU.gx = (float)IMU.gyroCount[0]*IMU.gRes;
      IMU.gy = (float)IMU.gyroCount[1]*IMU.gRes;
      IMU.gz = (float)IMU.gyroCount[2]*IMU.gRes;
    }
    IMU.updateTime();
    // Don't use mag since it's too noisy indoor
    MadgwickQuaternionUpdateIMU2(IMU.ax, IMU.ay, IMU.az, IMU.gx*DEG_TO_RAD, IMU.gy*DEG_TO_RAD, IMU.gz*DEG_TO_RAD, IMU.deltat); 
}

void setup(){
  M5.begin();
  delay(100);
  // Serial.begin(115200); // Don't call it! M5.begin() includes it. If it is called again, some problems occcurs as freeze.
  pixelShader = new PixelShader();
  fpsMeter = new FpsMeter();
  mesh = new Mesh("/Shanghai.pmd");// File name should be ASCII
  InitAR();
}

void loop() {
  static float t = 0.0f;
  Matrix matrix, matrixR;
  //MatrixFactory::CreateMatrix(&matrix, &matrixR, t);
  CreateARMatrix(&matrix, &matrixR);
  ReviseIMU();
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
