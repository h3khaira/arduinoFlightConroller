#include <Arduino.h>
#include <Wire.h>
#include <gyro.h>

//receiver and esc variables
byte last_throttle, last_yaw, last_pitch, last_roll;
int user_input_throttle, user_input_yaw, user_input_pitch, user_input_roll;
volatile int converted_throttle;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, current_time, zero_timer, esc_loop_timer;
unsigned long pulse_timer_1, pulse_timer_2, pulse_timer_3, pulse_timer_4;
int throttle_pulse_length;
int start = 0;
float pid_roll_input, pid_yaw_input, pid_pitch_input;
float p_gain = 0.2, d_gain = 0.01, i_gain = 0.1;

int convert_throttle()
{
  int low, center, high, actual, difference;
  low = 1188;
  high = 1840;
  center = 1512;
  actual = user_input_throttle;
  if (actual < center)
  {
    if (actual < low)
      actual = low;
    difference = ((long)(center - actual) * (long)500) / (center - low);
    return 1500 - difference;
  }
  else if (actual > center)
  {
    if (actual > high)
      actual = high;
    difference = ((long)(actual - center) * (long)500) / (high - center);
    return 1500 + difference;
  }
  else
    return 1500;
}

void esc_pulse_output()
{
  zero_timer = micros();
  PORTD |= B11110000; // Set arduino pins 4,5,6,7 high sending signal to ESCs
  //B00010000 BOTTOM LEFT
  //B00100000 TOP LEFT
  //B01000000 TOP RIGHT
  //B10000000 BOTTOM RIGHT

  //taking into account PID controller outputs
  pulse_timer_1 = throttle_pulse_length + zero_timer + pid_pitch_input + pid_roll_input + pid_yaw_input;
  pulse_timer_2 = throttle_pulse_length + zero_timer + pid_pitch_input - pid_roll_input - pid_yaw_input;
  pulse_timer_3 = throttle_pulse_length + zero_timer - pid_pitch_input + pid_roll_input - pid_yaw_input;
  pulse_timer_4 = throttle_pulse_length + zero_timer - pid_pitch_input - pid_roll_input + pid_yaw_input;
/*
  pulse_timer_1 = throttle_pulse_length + zero_timer;
  pulse_timer_2 = throttle_pulse_length + zero_timer;
  pulse_timer_3 = throttle_pulse_length+ zero_timer;
  pulse_timer_4 = throttle_pulse_length+ zero_timer;
*/
  read_gyro_signals();
  while (PORTD >= 16)
  { //THIS LOOP RUNS WHILE ANY OF 4 ESCs ARE HIGH
    esc_loop_timer = micros();
    if (pulse_timer_1 <= esc_loop_timer)
      PORTD &= B00100000; //sending low signal to ESC 1
    if (pulse_timer_2 <= esc_loop_timer)
      PORTD &= B01000000; //sending low signals to ESC 2
    if (pulse_timer_3 <= esc_loop_timer)
      PORTD &= B00010000; //sending low signals to ESC 2
    if (pulse_timer_4 <= esc_loop_timer)
      PORTD &= B10000000; //sending low signals to ESC 2
  }
  //reset the outputs to the motors
  pid_pitch_input = 0;
  pid_roll_input = 0;
}

void get_pid_inputs(){
  pid_pitch_input = p_gain*angle_pitch;
  pid_roll_input = p_gain*angle_roll;
}