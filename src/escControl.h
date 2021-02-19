#pragma once
extern byte last_throttle, last_yaw, last_pitch, last_roll;
extern int user_input_throttle, user_input_yaw, user_input_pitch, user_input_roll;;
extern volatile int converted_throttle;
extern unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, current_time, zero_timer, esc_loop_timer;
extern unsigned long pulse_timer_1, pulse_timer_2, pulse_timer_3, pulse_timer_4; 
extern int throttle_pulse_length;
extern int start;
extern float pid_roll_input, pid_yaw_input, pid_pitch_input;
extern float last_angle_roll, last_angle_pitch, add_angle_roll, add_angle_pitch;



int convert_throttle();
void esc_pulse_output();
void get_pid_inputs();