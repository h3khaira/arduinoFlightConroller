#include <Wire.h>
#include <Arduino.h>
#include <gyro.h>

float filter_weight = 0.98;

void setup()
{
  pinMode(12, OUTPUT);
  Wire.begin();
  Serial.begin(57600);
  //digitalWrite(12,HIGH);
  delay(250);
  start_gyro();
}

void loop()
{
  TWBR = 12;
  read_gyro_signals();
  angle_pitch -= angle_roll * sin(raw_gyro_yaw * 0.000001066); //transferring roll to pitch when quad rotates on yaw axis
  angle_roll += angle_pitch * sin(raw_gyro_yaw * 0.000001066);
  
  //complementary filter to account for drift in sensors
  angle_pitch += (filter_weight * (raw_gyro_pitch * 0.0000611) + (1 - filter_weight) * acc_pitch);
  angle_roll += (filter_weight * (raw_gyro_roll * 0.0000611) + (1 - filter_weight) * acc_roll);
 
  //Uncomment to read offsets
  /*
  Serial.print("Pitch is ");
  Serial.print(raw_gyro_pitch);
  Serial.print(" Roll is ");
  Serial.print(raw_gyro_roll);
  Serial.print(" Yaw is ");
  Serial.print(raw_gyro_yaw);
  Serial.print(" Acc x is ");
  Serial.print(x_acc);
  Serial.print(" Acc y is ");
  Serial.print(y_acc);
  Serial.print(" Acc z is ");
  Serial.println(z_acc);
  */

  Serial.print("Roll angle ");
  Serial.print(acc_roll);
  Serial.print(" Pitch angle ");
  Serial.println(acc_pitch);
}
