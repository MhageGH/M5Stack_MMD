# M5Stack_MMD
3D rendering by M5Stack<br>
 ![MMD](doc/MMD.jpg)
 
## Prepare
- [M5Stack](https://www.switch-science.com/catalog/3647/)  : Switch Science

## Development Environment
- [Arduino IDE](https://www.arduino.cc/en/main/software)
- [arduino-esp32](https://github.com/espressif/arduino-esp32)

## About MMD
see [VPVP wiki](https://www6.atwiki.jp/vpvpwiki)

## Customize
- You can move, rotate, or scale the 3D model by rewriting MatrixFactory.h.
- You can output bitmap file to SD card by enabling bitmap_out in M5Stack_MMD.ino.
- In the current version, the code includes a MMD model data (PMD file) as constant arrays.
  You can change a MMD model by rewriting  arrays EmbemddedFile.h.
  The arrays and simply made from PMD file and BITMAP files for texture by Binary To Text Converter.
  BITMAP file type has to be 16bit ARGB(1-5-5-5) and the sizes of the files must be same.
  And you should take care ROM size of hardware.

## Future
This code includes MMD model data (PMD file) as constant arrays. Normal M5Stack can't deal PMD file in SD card at a sufficient speed because of insufficient RAM. 
If M5Stack FIRE with 2M PSRAM can be used, I will realize to draw any 3D models in SD card at higher speed.
