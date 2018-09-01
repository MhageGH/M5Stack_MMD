#include "Affine.h"

void Affine::Dot(Matrix* matrix1, Matrix* matrix2, Matrix* matrixOut) {
  matrixOut->m11 = matrix1->m11*matrix2->m11 + matrix1->m12*matrix2->m21 + matrix1->m13*matrix2->m31 + matrix1->m14*matrix2->m41;  
  matrixOut->m12 = matrix1->m11*matrix2->m12 + matrix1->m12*matrix2->m22 + matrix1->m13*matrix2->m32 + matrix1->m14*matrix2->m42;
  matrixOut->m13 = matrix1->m11*matrix2->m13 + matrix1->m12*matrix2->m23 + matrix1->m13*matrix2->m33 + matrix1->m14*matrix2->m43;
  matrixOut->m14 = matrix1->m11*matrix2->m14 + matrix1->m12*matrix2->m24 + matrix1->m13*matrix2->m34 + matrix1->m14*matrix2->m44;
  matrixOut->m21 = matrix1->m21*matrix2->m11 + matrix1->m22*matrix2->m21 + matrix1->m23*matrix2->m31 + matrix1->m24*matrix2->m41;
  matrixOut->m22 = matrix1->m21*matrix2->m12 + matrix1->m22*matrix2->m22 + matrix1->m23*matrix2->m32 + matrix1->m24*matrix2->m42;
  matrixOut->m23 = matrix1->m21*matrix2->m13 + matrix1->m22*matrix2->m23 + matrix1->m23*matrix2->m33 + matrix1->m24*matrix2->m43;
  matrixOut->m24 = matrix1->m21*matrix2->m14 + matrix1->m22*matrix2->m24 + matrix1->m23*matrix2->m34 + matrix1->m24*matrix2->m44;
  matrixOut->m31 = matrix1->m31*matrix2->m11 + matrix1->m32*matrix2->m21 + matrix1->m33*matrix2->m31 + matrix1->m34*matrix2->m41;
  matrixOut->m32 = matrix1->m31*matrix2->m12 + matrix1->m32*matrix2->m22 + matrix1->m33*matrix2->m32 + matrix1->m34*matrix2->m42;
  matrixOut->m33 = matrix1->m31*matrix2->m13 + matrix1->m32*matrix2->m23 + matrix1->m33*matrix2->m33 + matrix1->m34*matrix2->m43;
  matrixOut->m34 = matrix1->m31*matrix2->m14 + matrix1->m32*matrix2->m24 + matrix1->m33*matrix2->m34 + matrix1->m34*matrix2->m44;
  matrixOut->m41 = matrix1->m41*matrix2->m11 + matrix1->m42*matrix2->m21 + matrix1->m43*matrix2->m31 + matrix1->m44*matrix2->m41;
  matrixOut->m42 = matrix1->m41*matrix2->m12 + matrix1->m42*matrix2->m22 + matrix1->m43*matrix2->m32 + matrix1->m44*matrix2->m42;
  matrixOut->m43 = matrix1->m41*matrix2->m13 + matrix1->m42*matrix2->m23 + matrix1->m43*matrix2->m33 + matrix1->m44*matrix2->m43;
  matrixOut->m44 = matrix1->m41*matrix2->m14 + matrix1->m42*matrix2->m24 + matrix1->m43*matrix2->m34 + matrix1->m44*matrix2->m44;
}

void Affine::Dot(Matrix* matrix, Vector* vector, Vector* vectorOut) {
  vectorOut->x = matrix->m11*vector->x + matrix->m12*vector->y + matrix->m13*vector->z + matrix->m14*vector->w;
  vectorOut->y = matrix->m21*vector->x + matrix->m22*vector->y + matrix->m23*vector->z + matrix->m24*vector->w;
  vectorOut->z = matrix->m31*vector->x + matrix->m32*vector->y + matrix->m33*vector->z + matrix->m34*vector->w;
  vectorOut->w = matrix->m41*vector->x + matrix->m42*vector->y + matrix->m43*vector->z + matrix->m44*vector->w;
}

float Affine::Dot(Vector* vector1, Vector* vector2) {
  return vector1->x*vector2->x + vector1->y*vector2->y + vector1->z*vector2->z;
}

void Affine::CreateTranslationMatrix(float x, float y, float z, Matrix* matrixOut) {
  matrixOut->m11 = 1; matrixOut->m12 = 0; matrixOut->m13 = 0, matrixOut->m14 = x;
  matrixOut->m21 = 0; matrixOut->m22 = 1; matrixOut->m23 = 0, matrixOut->m24 = y;
  matrixOut->m31 = 0; matrixOut->m32 = 0; matrixOut->m33 = 1, matrixOut->m34 = z;
  matrixOut->m41 = 0; matrixOut->m42 = 0; matrixOut->m43 = 0, matrixOut->m44 = 1;
}

void Affine::CreateRotationMatrixX(float theta, Matrix* matrixOut) {
  matrixOut->m11 = 1; matrixOut->m12 = 0; matrixOut->m13 = 0; matrixOut->m14 = 0;
  matrixOut->m21 = 0; matrixOut->m22 = cos(theta); matrixOut->m23 = -sin(theta); matrixOut->m24 = 0;
  matrixOut->m31 = 0; matrixOut->m32 = sin(theta); matrixOut->m33 = cos(theta); matrixOut->m34 = 0;
  matrixOut->m41 = 0; matrixOut->m42 = 0; matrixOut->m43 = 0; matrixOut->m44 = 1;
}

void Affine::CreateRotationMatrixY(float theta, Matrix* matrixOut) {
  matrixOut->m11 = cos(theta); matrixOut->m12 = 0; matrixOut->m13 = sin(theta); matrixOut->m14 = 0;
  matrixOut->m21 = 0; matrixOut->m22 = 1; matrixOut->m23 = 0; matrixOut->m24 = 0;
  matrixOut->m31 = -sin(theta); matrixOut->m32 = 0; matrixOut->m33 = cos(theta); matrixOut->m34 = 0;
  matrixOut->m41 = 0; matrixOut->m42 = 0; matrixOut->m43 = 0; matrixOut->m44 = 1;
}

void Affine::CreateRotationMatrixZ(float theta, Matrix* matrixOut) {
  matrixOut->m11 = cos(theta); matrixOut->m12 = -sin(theta); matrixOut->m13 = 0; matrixOut->m14 = 0;
  matrixOut->m21 = sin(theta); matrixOut->m22 = cos(theta); matrixOut->m23 = 0; matrixOut->m24 = 0;
  matrixOut->m31 = 0; matrixOut->m32 = 0; matrixOut->m33 = 1; matrixOut->m34 = 0;
  matrixOut->m41 = 0; matrixOut->m42 = 0; matrixOut->m43 = 0; matrixOut->m44 = 1;
}

void Affine::CreateScalingMatrix(float scale, Matrix* matrixOut) {
  matrixOut->m11 = scale; matrixOut->m12 = 0; matrixOut->m13 = 0; matrixOut->m14 = 0;
  matrixOut->m21 = 0; matrixOut->m22 = scale; matrixOut->m23 = 0; matrixOut->m24 = 0;
  matrixOut->m31 = 0; matrixOut->m32 = 0; matrixOut->m33 = scale; matrixOut->m34 = 0;
  matrixOut->m41 = 0; matrixOut->m42 = 0; matrixOut->m43 = 0; matrixOut->m44 = 1;
}

void Affine::Normalize(Vector* vector) {
  // Approximate calculation : ref http://d-kami.hatenablog.com/entry/20140512/1399894402
  float x = abs(vector->x), y = abs(vector->y), z = abs(vector->z);
  float _max, _min, _center;
  if (x < y && x < z) { 
    _min = x;
    if (y < z) { _center = y; _max = z; }
    else { _center = z; _max = y; }
  }
  else if (x > y && x > z) {
    _max = x;
    if (y < z) { _min = y; _center = z; }
    else { _center = y; _min = z; }
  }
  else {
    _center = x;
    if (y < z) { _min = y; _max = z; }
    else {_min = z; _max = y; }
  }
  float inv_d = 1.0f/(_max*0.84f + _center*0.5175f + _min*0.2675f);
  vector->x *= inv_d;
  vector->y *= inv_d;
  vector->z *= inv_d;  
}

