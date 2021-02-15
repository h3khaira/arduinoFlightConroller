#pragma once
extern byte last_throttle, last_yaw, last_pitch, last_roll;
extern int receiver_input_throttle, receiver_input_yaw, receiver_input_pitch, receiver_input_roll;;
extern volatile int converted_throttle;
extern unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, current_time, zero_timer, esc_loop_timer, pulse_timer;
extern int throttle_pulse_length;
extern int start;


int convert_throttle();
void esc_pulse_output();
