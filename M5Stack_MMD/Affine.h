#ifndef _AFFINE_H
#define _AFFINE_H

#include <M5Stack.h>

class Point {
public: 
  int16_t X, Y;
  Point(int16_t X0 = 0, int16_t Y0 =  0) : X(X0), Y(Y0) {}
};

class Vector {  // column vector
public : 
  float x, y, z, w;
  Vector(float x0 = 0, float y0 = 0, float z0 = 0, float w0 = 1) : x(x0), y(y0), z(z0), w(w0) {}
  Point ToPoint() { return Point((int16_t)x, (int16_t)y); }
};

class Matrix {
public : 
  float m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44;
  Matrix(float m11_0 = 1, float m12_0 = 0, float m13_0 = 0, float m14_0 = 0, float m21_0 = 0, float m22_0 = 1, float m23_0 = 0, float m24_0 = 0,
         float m31_0 = 0, float m32_0 = 0, float m33_0 = 1, float m34_0 = 0, float m41_0 = 0, float m42_0 = 0, float m43_0 = 0, float m44_0 = 1) : 
         m11(m11_0), m12(m12_0), m13(m13_0), m14(m14_0), m21(m21_0), m22(m22_0), m23(m23_0), m24(m24_0), 
         m31(m31_0), m32(m32_0), m33(m33_0), m34(m34_0), m41(m41_0), m42(m42_0), m43(m43_0), m44(m44_0) {}
};

class Affine {
  Affine();
public : 
  static void Dot(Matrix* matrix1, Matrix* matrix2, Matrix* matrixOut);
  static void Dot(Matrix* matrix, Vector* vector, Vector* vectorOut);
  static float Dot(Vector* vector1, Vector* vector2);  // ignore w
  static void CreateTranslationMatrix(float x, float y, float z, Matrix* matrixOut);
  static void CreateRotationMatrixX(float theta, Matrix* matrixOut);
  static void CreateRotationMatrixY(float theta, Matrix* matrixOut);
  static void CreateRotationMatrixZ(float theta, Matrix* matrixOut);
  static void CreateScalingMatrix(float scale, Matrix* matrixOut);
  static void Normalize(Vector* vector);  // ignore w
};


#endif // _AFFINE_H
