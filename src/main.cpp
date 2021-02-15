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
  //digitalWrite(12,HIGH);
  delay(250);
  start_gyro();
  while (convert_throttle() > 1000) {
    start++;
    // send a 1000 microsecond pulse to the motors to initialize
    PORTD |= B11110000;
    delayMicroseconds(1000);
    PORTD &= B00001111;
    delay(3);
    if (start == 125) {
      //flash LED on and off
      digitalWrite(12, !digitalRead(12));
      start = 0;
    }
  }
}

void loop()
{
  TWBR = 12;
  read_gyro_signals();
  //motor control
  throttle_pulse_length = convert_throttle();
  esc_pulse_output();

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
  Serial.println(throttle_pulse_length);
}

//Receiver interrupt
ISR(PCINT0_vect) {
  current_time = micros();
  if (PINB & B00000010) {
    if (last_throttle == 0) {
      last_throttle = 1;
      timer_throttle = current_time;
    }
  }
  else if (last_throttle == 1) {
    last_throttle = 0;
    receiver_input_throttle = current_time - timer_throttle;
  }
}
