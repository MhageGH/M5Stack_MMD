#ifndef _MATRIXFACTORY_H
#define _MATRIXFACTORY_H
#include "Affine.h"

class MatrixFactory {
  MatrixFactory ();
public : 
  static void CreateMatrix(Matrix* matrix, Matrix* matrixR, uint32_t t) {
    Matrix matrixScale, matrixRy, matrixRx, matrixT, matrixTR;
    Affine::CreateRotationMatrixX(0.95*PI, &matrixRx);
    Affine::CreateRotationMatrixY(-0.3 + 0.03*(t%15)*PI, &matrixRy);
    Affine::CreateTranslationMatrix(125, 180, 0, &matrixT);
    Affine::CreateScalingMatrix(25, &matrixScale);
    Affine::Dot(&matrixRx, &matrixRy, matrixR);
    Affine::Dot(&matrixT, matrixR, &matrixTR);
    Affine::Dot(&matrixTR, &matrixScale, matrix);
  }
};

#endif // _MATRIXFACTORY_H