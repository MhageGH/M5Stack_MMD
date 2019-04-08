#pragma once
#include "Affine.h"

class SimpleMotion {
public : 
  void CreateMatrix(Matrix* matrix, Matrix* matrixR, float t) {
    Matrix matrixScale, matrixRy, matrixRx, matrixT, matrixTR;
    Affine::CreateRotationMatrixX(0.95*PI, &matrixRx);
    Affine::CreateRotationMatrixY((-0.3 + 0.2*t)*PI, &matrixRy);
    Affine::CreateTranslationMatrix(120, 165, 0, &matrixT);
    Affine::CreateScalingMatrix(18, &matrixScale);
    Affine::Dot(&matrixRx, &matrixRy, matrixR);
    Affine::Dot(&matrixT, matrixR, &matrixTR);
    Affine::Dot(&matrixTR, &matrixScale, matrix);
  }
};

