#include <Wire.h>

#define MPU6050_ADDR 0x68 // MPU6050 I2C address

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // Wake up MPU6050
  Wire.endTransmission(true);

  delay(100);
}

void loop() {
  int16_t accel_x, accel_y, accel_z;
  int16_t gyro_x, gyro_y, gyro_z;

  // Read accelerometer data
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 14, true); // request 14 bytes from MPU6050 and release bus
  accel_x = Wire.read() << 8 | Wire.read(); // combine two bytes into a single integer
  accel_y = Wire.read() << 8 | Wire.read();
  accel_z = Wire.read() << 8 | Wire.read();

  gyro_x = Wire.read() << 8 | Wire.read();
  gyro_y = Wire.read() << 8 | Wire.read();
  gyro_z = Wire.read() << 8 | Wire.read();

  // Convert raw values to degrees
  float accel_angle_x = atan2(accel_y, sqrt(pow(accel_x, 2) + pow(accel_z, 2))) * (180.0 / PI);
  float accel_angle_y = atan2(-accel_x, sqrt(pow(accel_y, 2) + pow(accel_z, 2))) * (180.0 / PI);

  float gyro_angle_x = gyro_x / 131.0; // MPU6050 sensitivity for gyro is 131 LSB/degrees/sec
  float gyro_angle_y = gyro_y / 131.0;

  // Combine accelerometer and gyroscope angles (complementary filter)
  float alpha = 0.95; // Weight for accelerometer angle (adjust as needed)
  static float angle_x = 0, angle_y = 0;
  angle_x = alpha * (angle_x + gyro_angle_x * 0.01) + (1 - alpha) * accel_angle_x;
  angle_y = alpha * (angle_y + gyro_angle_y * 0.01) + (1 - alpha) * accel_angle_y;

  // Print rounded angles to Serial Plotter
  Serial.print('\t');
  Serial.print(round(angle_x));
  Serial.print('\t');
  Serial.println(round(angle_y));

  delay(1);
}
