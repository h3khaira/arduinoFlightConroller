#include <Wire.h>
#include <Arduino.h>
#include <gyro.h>
#include <escControl.h>



void setup()
{
  //ESC Stuff
  DDRD |= B11110000; //Setting ESC signal wires as output
  DDRB |= B00010000; //Setting pin 12 as output
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT0); //Pin 8 as interrupt
  PCMSK0 |= (1 << PCINT1); //Pin 9 as interrupt
  PCMSK0 |= (1 << PCINT2); //Pin 10 as interrupt
  PCMSK0 |= (1 << PCINT3); //Pin 11 as interrupt
  Wire.begin();
  Serial.begin(57600);
  delay(250);
  start_gyro();
  delay(250);
  while (convert_throttle() > 1000) {
    // send a 1000 microsecond pulse to the motors to initialize
    PORTD |= B11110000;
    delayMicroseconds(1000);
    PORTD &= B00001111;
    delay(3);
  }
  digitalWrite(12,HIGH); //turn LED to indicate setup is done
}

void loop()
{
  TWBR = 12;
  //motor control
  throttle_pulse_length = convert_throttle();
  esc_pulse_output();
  euler_angles();
  angle_pitch -= angle_roll * sin(raw_gyro_yaw * 0.000001066); //transferring roll to pitch when quad rotates on yaw axis
  angle_roll += angle_pitch * sin(raw_gyro_yaw * 0.000001066);

  //complementary filter to account for drift in sensors
  angle_pitch += (filter_weight * (raw_gyro_pitch * 0.0000611) + (1 - filter_weight) * acc_pitch);
  angle_roll += (filter_weight * (raw_gyro_roll * 0.0000611) + (1 - filter_weight) * acc_roll);
  get_pid_inputs();
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

  /*
  Serial.print("Roll angle ");
  Serial.print(acc_roll);
  Serial.print(" Pitch angle ");
  Serial.println(acc_pitch);
  */
 /*
  Serial.print ("Yaw: ");
  Serial.print(user_input_yaw);
  Serial.print (" Pitch: ");
  Serial.print(user_input_pitch);
  Serial.print (" Roll: ");
  Serial.println(user_input_roll);
  */
  Serial.println(pid_pitch_input);
}

//Receiver interrupt
ISR(PCINT0_vect){
  current_time=micros();
  //Yaw
  if (PINB & B00000001){  //check if the input was changed from the last time it was checked
    if (last_yaw == 0){
        last_yaw=1;  // note that the yaw input was activated 
        timer_yaw=current_time; //start timer to calculate length
      }
  }
  else if (last_yaw == 1){
    last_yaw=0;
    user_input_yaw = current_time - timer_yaw; //receiver input is the length of pulse in microseconds
  }
  //Throttle
  if (PINB & B00000010){
    if (last_throttle == 0){
        last_throttle=1;
        timer_throttle=current_time;
      }
  }
  else if (last_throttle == 1){
    last_throttle=0;
    user_input_throttle = current_time - timer_throttle;
  }
  //Pitch
   if (PINB & B00000100){
    if (last_pitch == 0){
        last_pitch=1;
        timer_pitch=current_time;
      }
  }
  else if (last_pitch == 1){
    last_pitch=0;
    user_input_pitch = current_time - timer_pitch;
  }
  //Roll
    if (PINB & B00001000){
    if (last_roll == 0){
        last_roll=1;
        timer_roll=current_time;
      }
  }
  else if (last_roll == 1){
    last_roll=0;
    user_input_roll = current_time - timer_roll;
  }
}