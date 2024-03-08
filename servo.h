#pragma once

#define left_servo_pin 10
#define right_servo_pin 9
#define LEDC_CHANNEL3 4
#define LEDC_CHANNEL4 5

const int frequency = 50;   // PWM frequency in Hertz
const int resolution = 14;  // PWM resolution in bits

void setupServoPins()
{
  ledcSetup(LEDC_CHANNEL3, frequency, resolution);
  ledcSetup(LEDC_CHANNEL4, frequency, resolution);
  ledcAttachPin(left_servo_pin, LEDC_CHANNEL3);    
  ledcAttachPin(right_servo_pin, LEDC_CHANNEL4);    
}

void moveLeftServo(int angle) 
{
  int dutyCycle = map(angle, 0, 180, 0, pow(2, resolution) - 1);
  ledcWrite(LEDC_CHANNEL3, dutyCycle);
  delay(500);  // Wait for the servo to reach the desired position
}

void moveRightServo(int angle) 
{
  int dutyCycle = map(180-angle, 0, 180, 0, pow(2, resolution) - 1);
  ledcWrite(LEDC_CHANNEL4, dutyCycle);
  delay(500);  // Wait for the servo to reach the desired position
}

void closeGripper() 
{
  int angle = 0;
  moveLeftServo(angle);
  moveRightServo(angle);
}

void openGripper() 
{
  int angle = 45;
  moveLeftServo(angle);
  moveRightServo(angle);
}

