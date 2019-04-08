#pragma once
#include "utility/MPU9250.h"
#include "quaternionFilters2.h"
#include "Affine.h"

class ARlikeMotion {
    MPU9250 IMU;
public :
    ARlikeMotion();
    void CreateMatrix(Matrix* matrix, Matrix* matrixR);
    void ReviseIMU();
    float scale = 18;
};
