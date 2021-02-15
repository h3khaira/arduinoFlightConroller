#pragma once
extern int gyro_address; //address for MPU-6050 in hexadecimal
extern float raw_gyro_roll, raw_gyro_pitch, raw_gyro_yaw; //raw values from the gyro
extern float x_acc, y_acc, z_acc;
extern float roll_cal, pitch_cal, yaw_cal, x_acc_cal, y_acc_cal, z_acc_cal;
extern float angle_roll, angle_pitch, angle_yaw; //adjusted values in deg per sec
extern int temperature;
extern float acc_magnitude, acc_pitch, acc_roll;
extern float filter_weight;

void read_gyro_signals();
void start_gyro();
