#include <Arduino.h>
#include <Wire.h>
#include <math.h>

int gyro_address = 0x68;  //address for MPU-6050 in hexadecimal
float raw_gyro_roll, raw_gyro_pitch, raw_gyro_yaw; //raw values from the gyro
float x_acc, y_acc, z_acc;
float roll_cal=-1270, pitch_cal=210, yaw_cal=52, x_acc_cal=-2140, y_acc_cal=125, z_acc_cal=210; //offsets
float angle_roll, angle_pitch, angle_yaw; //adjusted values in deg per sec
int temperature;
float acc_magnitude, acc_pitch, acc_roll;

void euler_angles()
{
  acc_magnitude = sqrt(pow(x_acc, 2) + pow(y_acc, 2) + pow(z_acc, 2));
  acc_roll = 57.2958 * atan2(y_acc, z_acc);
  acc_pitch = 57.2958 * atan2(-1 * x_acc, sqrt(pow(y_acc, 2) + pow(z_acc, 2)));
}

void read_gyro_signals()
{
  //Gyro data
  Wire.beginTransmission(gyro_address);
  Wire.write(0x3B); //First register of the accelerometer
  Wire.endTransmission();
  Wire.requestFrom(gyro_address, 14); //request 14 byes of information. the gyro will send 14 registers containing measurements from register 0x3Bonwards
  while (Wire.available() < 14); //stays on this line until 14 bytes are available
    x_acc = Wire.read() << 8 | Wire.read();
    x_acc -= x_acc_cal;
    y_acc = Wire.read() << 8 | Wire.read();
    y_acc -= y_acc_cal;
    z_acc = Wire.read() << 8 | Wire.read();
    z_acc -= z_acc_cal;
    euler_angles();
    temperature = Wire.read() << 8 | Wire.read();
    raw_gyro_roll = Wire.read() << 8 | Wire.read(); //the first two registered contain the roll measurements in the form of 8 bits each
    raw_gyro_roll -= roll_cal;
    raw_gyro_pitch = Wire.read() << 8 | Wire.read();
    raw_gyro_pitch -= pitch_cal;
    raw_gyro_yaw = Wire.read() << 8 | Wire.read();
    raw_gyro_yaw -= yaw_cal;
}

void start_gyro()
{
  Wire.beginTransmission(gyro_address);
  //MPU-6050 starts in Sleep mode by default. Changing this register brings it out of sleep mode
  Wire.write(0x6B); //Access PWR_MGMT_1 register
  Wire.write(0x00); //Write register to 0 to bring out of sleep mode
  Wire.endTransmission();

  Wire.beginTransmission(gyro_address);
  Wire.write(0x1B); //Access the GYRO_CONFIG register
  Wire.write(0x08); //Write the FS_SEL bits to change gyro resolution to 500 degrees per second, 1 DEG/SEC  = 65.5 OUTPUT FROM GYRO LSB
  Wire.endTransmission();

  Wire.beginTransmission(gyro_address);
  Wire.write(0x1C); //Access accelerometer config register
  Wire.write(0x10); // +-8g = 4096 LSB/g
  Wire.endTransmission();

  Wire.beginTransmission(gyro_address);
  Wire.write(0x1A); //Access CONFIG register
  Wire.write(0x03); // Digial Low Pass Filter for gyro/accelorometer set to 43 Hz
  Wire.endTransmission();
}
