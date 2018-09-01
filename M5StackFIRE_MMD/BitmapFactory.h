#ifndef _BITMAPFACTORY_H
#define _BITMAPFACTORY_H

#include <M5Stack.h>

class BitmapFactory {
  BitmapFactory();
  
public :
  // Note: _width must be even for no padding
  static void CreateBitmap(uint16_t** backBuffer, String filename, int _height, int _width) {
    // Note: file writing length should be power of 2
    File file = SD.open(filename, FILE_WRITE);
    file.write('B');
    file.write('M');
    uint32_t bfSize = _height*_width*2 + 14 + 40 + 12;  // file header 14, information header 40, bit field 12
    file.write((byte*)&bfSize, sizeof(bfSize));
    for (int i = 0; i < 4; ++i) file.write(0);
    uint32_t bfOffBits = 14 + 40+ 12;
    file.write((byte*)&bfOffBits, sizeof(bfOffBits));
    uint32_t biSize = 40;
    file.write((byte*)&biSize, sizeof(biSize));
    int32_t biWidth = _width, biHeight = _height;
    file.write((byte*)&biWidth, sizeof(biWidth));
    file.write((byte*)&biHeight, sizeof(biHeight));
    uint16_t biPlanes = 1;
    file.write((byte*)&biPlanes, sizeof(biPlanes));
    uint16_t biBitCount = 16;
    file.write((byte*)&biBitCount, sizeof(biBitCount));
    uint32_t biCompression = 3;
    file.write((byte*)&biCompression, sizeof(biCompression));
    uint32_t biSizeImage = _height*_width*2;
    file.write((byte*)&biSizeImage, sizeof(biSizeImage));
    int32_t biXPixPerMeter = 0, biYPixPerMeter = 0;
    file.write((byte*)&biXPixPerMeter, sizeof(biXPixPerMeter));
    file.write((byte*)&biYPixPerMeter, sizeof(biYPixPerMeter));
    uint32_t biClrUsed = 0;
    file.write((byte*)&biClrUsed, sizeof(biClrUsed));
    uint32_t biClrImportant = 0;
    file.write((byte*)&biClrImportant, sizeof(biClrImportant));
    uint32_t bitFields[3] = {0x0000F800, 0x000007E0, 0x0000001F};
    for (int i = 0; i < 3; ++i) file.write((byte*)&bitFields[i], sizeof(bitFields[i]));
    for (int i = 0; i < _height; ++i) for (int j = 0; j < _width; ++j) file.write((byte*)&backBuffer[_height - 1 - i][j], sizeof(backBuffer[_height - 1 - i][j]));
    file.close();
  }
};

#endif // _BITMAPFACTORY_H
