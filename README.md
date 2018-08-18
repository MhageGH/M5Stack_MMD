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
 
## Future
This code includes MMD model data (PMD file) as constant arrays. It can't deal PMD file in SD card at a sufficient speed because of insufficient RAM. 
If M5Stack FIRE with 2M PSRAM can be used, I will realize to draw any 3D models in SD card at higher speed.
