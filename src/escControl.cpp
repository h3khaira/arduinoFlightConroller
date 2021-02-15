#include <Arduino.h>
#include <Wire.h>

//receiver and esc variables
byte last_throttle, last_yaw, last_pitch, last_roll;
int receiver_input_throttle, receiver_input_yaw, receiver_input_pitch, receiver_input_roll;;
volatile int converted_throttle;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, current_time, zero_timer, esc_loop_timer, pulse_timer;
int throttle_pulse_length;
int start = 0;

int convert_throttle() {
  int low, center, high, actual, difference;
  low = 1188;
  high = 1840;
  center = 1512;
  actual = receiver_input_throttle;
  if (actual < center) {
    if (actual < low) actual = low;
    difference = ((long)(center - actual) * (long)500) / (center - low);
    return 1500 - difference;
  }
  else if (actual > center) {
    if (actual > high)actual = high;
    difference = ((long)(actual - center) * (long)500) / (high - center);
    return 1500 + difference;
  }
  else return 1500;
}

void esc_pulse_output() {
  zero_timer = micros();
  PORTD |= B11110000; // Set arduino pins 4,5,6,7 high sending signal to ESCs
  pulse_timer = throttle_pulse_length + zero_timer;
  //B00001000
  //B00000100
  //B00000010
  //B00000001
  while (PORTD >= 16) { //THIS LOOP RUNS WHILE ALL 4 OF THE LEDs ARE LOW
    esc_loop_timer = micros();
    if (pulse_timer <= esc_loop_timer)PORTD &= B00001111; //sending low signals to all 4 ESCs
  }
}
