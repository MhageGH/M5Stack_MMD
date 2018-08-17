#include <M5Stack.h>
#include "PixelShader.h"
#include "PmdLoader.h"
#include "MatrixFactory.h"
#include "BitmapFactory.h"
#include "EmbemddedFile.h"
PixelShader* pixelShader;
PmdLoader* pmdLoader = 0;

bool bitmap_out = false;  // if you need to output bitmap file to SD card, enable it.

void setup(){
  M5.begin();
  delay(100);
  Serial.begin(115200);
  pixelShader = new PixelShader();
  pmdLoader = new PmdLoader(Shanghai_pmd);
}

void loop() {
  static uint32_t t = 0;
  Matrix matrix, matrixR;
  MatrixFactory::CreateMatrix(&matrix, &matrixR, t);
  pixelShader->Clear(0x39E7); // gray
  Polygon polygon;
  bool materialChanged;
  while(pmdLoader->GetPolygon(&polygon, &materialChanged)){
    if (materialChanged) {
      Material material;
      pmdLoader->GetMaterial(&material);
      pixelShader->SetMaterial(&material);
    }
    Vector v[3], n[3];
    for (int i = 0; i < 3; ++i) Affine::Dot(&matrix, &polygon.vertices[i], &v[i]);
    for (int i = 0; i < 3; ++i) polygon.vertices[i] = v[i];
    for (int i = 0; i < 3; ++i) Affine::Dot(&matrixR, &polygon.normal[i], &n[i]);
    for (int i = 0; i < 3; ++i) polygon.normal[i] = n[i];
    pixelShader->DrawPolygon(&polygon, true, true);
  }
  pixelShader->Flip();
  ++t;
  delay(10);
  if (bitmap_out){
    BitmapFactory::CreateBitmap(pixelShader->backBuffer, "/SS.bmp", pixelShader->_height, pixelShader->_width);
    Serial.println("Bitmap Output finished");
    while(1);
  }
}
