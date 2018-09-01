#ifndef _ADDRESSCHECKER_H
#define _ADDRESSCHECKER_H

// for test
class AddressChecker {
public :
  static void Check(void* p, char* valName) {
    uintptr_t addr = (uintptr_t)p;
    String memoryArea;
    if (0x3F400000 <= addr && addr <= 0x3F7FFFFF) memoryArea = String("External Flash");
    else if (0x3F800000 <= addr && addr <= 0x3FBFFFFF) memoryArea = String("External SRAM");
    else if (0x3FF80000 <= addr && addr <= 0x3FFFFFFF) memoryArea = String("Embedded SRAM");
    Serial.println(String(valName) + ": addr 0x"+ String(addr, HEX) + " - " + memoryArea);
  }
};

#endif // _ADDRESSCHECKER_H
