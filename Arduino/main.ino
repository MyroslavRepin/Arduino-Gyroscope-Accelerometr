// BEST WORK.  !!!!!!!!!!!!!!!!!!!!!!


#include <Wire.h>
#include <Servo.h>

#define MPU6050_ADDR 0x68 // MPU6050 I2C address
#define ROTATION_ANGLE 0 // Specify rotation angle in degrees

Servo servo;
Servo servo_2;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // Wake up MPU6050
  Wire.endTransmission(true);

  servo.attach(8);
  servo_2.attach(9);

  delay(5);
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
  float accel_angle_z = atan2(sqrt(pow(accel_x, 2) + pow(accel_y, 2)), accel_z) * (180.0 / PI);

  float gyro_angle_x = gyro_x / 131.0; // MPU6050 sensitivity for gyro is 131 LSB/degrees/sec
  float gyro_angle_y = gyro_y / 131.0;
  float gyro_angle_z = gyro_z / 131.0;

  // Combine accelerometer and gyroscope angles (complementary filter)
  float alpha = 0.95; // Weight for accelerometer angle (adjust as needed)
  static float angle_x = 0, angle_y = 0, angle_z = 0;

  // Adjust angle calculation based on orientation
  if (accel_angle_z > 90 || accel_angle_z < -90) {
    angle_x = alpha * (angle_x + gyro_angle_x * 0.01) + (1 - alpha) * accel_angle_x;
    angle_y = alpha * (angle_y + gyro_angle_y * 0.01) + (1 - alpha) * accel_angle_y;
  } else {
    angle_x = alpha * (angle_x + gyro_angle_x * 0.01) + (1 - alpha) * -accel_angle_x;
    angle_y = alpha * (angle_y + gyro_angle_y * 0.01) + (1 - alpha) * accel_angle_y;
  }

  // Print rounded angles to Serial Monitor
  Serial.print("X: ");
  Serial.print(round(angle_x));
  Serial.print("\tY: ");
  Serial.println(round(angle_y));

  int round_x = round(angle_x);
  int round_y = round(angle_y);

  int servo_angle_x = angle_x * 0.3 + angle_x;
  int servo_angle_y = angle_y * 0.3 + angle_y;

  servo.write(85 - servo_angle_x);
  servo_2.write(115 - servo_angle_y);
  
  delay(10);
}
