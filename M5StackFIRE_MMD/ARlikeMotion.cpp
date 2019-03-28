#include "ARlikeMotion.h"

ARlikeMotion::ARlikeMotion() {
  Wire.begin();
  IMU.initMPU9250();
}

void ARlikeMotion::CreateMatrix(Matrix* matrix, Matrix* matrixR) {
  Matrix matrixScale, matrixT, matrixTR, matrixTRT0, matrixT0;
  float r[3][3] = {{*getR2(), *(getR2()+3), *(getR2()+6)}, {*(getR2()+1), *(getR2()+4), *(getR2()+7)}, {*(getR2()+2), *(getR2()+5), *(getR2()+8)}};
  matrixR->m11 = +r[0][0]; matrixR->m12 = +r[0][2]; matrixR->m13 = +r[0][1]; matrixR->m14 = 0;
  matrixR->m21 = -r[1][0]; matrixR->m22 = -r[1][2]; matrixR->m23 = -r[1][1]; matrixR->m24 = 0;
  matrixR->m31 = +r[2][0]; matrixR->m32 = +r[2][2]; matrixR->m33 = +r[2][1]; matrixR->m34 = 0;
  matrixR->m41 = 0; matrixR->m42 = 0; matrixR->m43 = 0; matrixR->m44 = 1;
  Affine::CreateTranslationMatrix(120, 100, 0, &matrixT);   // centering for screen
  Affine::CreateTranslationMatrix(0, -65, 0, &matrixT0);    // centering for rotation axis
  Affine::CreateScalingMatrix(scale, &matrixScale);
  Affine::Dot(&matrixT, matrixR, &matrixTR);
  Affine::Dot(&matrixTR, &matrixT0, &matrixTRT0);
  Affine::Dot(&matrixTRT0, &matrixScale, matrix);
}

void ARlikeMotion::ReviseIMU() {
 if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
    {  
      IMU.readAccelData(IMU.accelCount);
      IMU.getAres();

      IMU.ax = (float)IMU.accelCount[0]*IMU.aRes;
      IMU.ay = (float)IMU.accelCount[1]*IMU.aRes;
      IMU.az = (float)IMU.accelCount[2]*IMU.aRes;

      IMU.readGyroData(IMU.gyroCount);
      IMU.getGres();

      IMU.gx = (float)IMU.gyroCount[0]*IMU.gRes;
      IMU.gy = (float)IMU.gyroCount[1]*IMU.gRes;
      IMU.gz = (float)IMU.gyroCount[2]*IMU.gRes;
    }
    IMU.updateTime();
    // Don't use mag since it's too noisy indoor
    MadgwickQuaternionUpdateIMU2(IMU.ax, IMU.ay, IMU.az, IMU.gx*DEG_TO_RAD, IMU.gy*DEG_TO_RAD, IMU.gz*DEG_TO_RAD, IMU.deltat); 
}
