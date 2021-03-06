#pragma once

#include <Arduino.h>

void MadgwickQuaternionUpdate2(float ax, float ay, float az, float gx, float gy,
                              float gz, float mx, float my, float mz,
                              float deltat);
void MadgwickQuaternionUpdateIMU2(float ax, float ay, float az, float gx, float gy,
                                 float gz, float deltat);
void MahonyQuaternionUpdate2(float ax, float ay, float az, float gx, float gy,
                            float gz, float mx, float my, float mz,
                            float deltat);
const float * getQ2();
const float * getR2();

