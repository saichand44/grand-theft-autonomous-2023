#pragma once

// for PWM
#define LEDC_CHANNEL1 0
#define LEDC_CHANNEL2 2
#define LEDC_RESOLUTION_BITS 14
#define LEDC_RESOLUTION ((1<<LEDC_RESOLUTION_BITS)-1)

#define enaMotor1Pin 36         // pwm for motor1 (left)
#define in1Motor1Pin 38         // for direction
#define in2Motor1Pin 37         // for direction
#define ot1Motor1Pin 14         // for output 
#define ot2Motor1Pin 13         // for output

#define enaMotor2Pin 41         // pwm for motor2 (right)
#define in1Motor2Pin 40         // for direction
#define in2Motor2Pin 39         // for direction
#define ot1Motor2Pin 7          // for output 
#define ot2Motor2Pin 6          // for output

#define nomotion 0
#define forward 1
#define backward 2
#define left 3
#define right 4

uint32_t freqValue=1500;                  // frequency for PWM
uint32_t speedValue1, directionValue1;    // speed, direction of motor1
uint32_t speedValue2, directionValue2;    // speed, direction of motor2

volatile int posiMotor1 = 0;
volatile int posiMotor2 = 0;
long prevT = 0;
float eprevMotor1 = 0;
float eprevMotor2 = 0;
float eintegralMotor1 = 0;
float eintegralMotor2 = 0;

int botRotated = 0;

void ledcAnalogWrite(uint8_t channel, uint32_t currSpeed, uint32_t maxSpeed = 100)
{ 
  // compute the duty cycle as a proportion to the resolution bits
  uint32_t speed = LEDC_RESOLUTION * min(currSpeed, maxSpeed) / maxSpeed;
  ledcWrite(channel, speed);
}

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2)
{
  analogWrite(pwm,pwmVal);
  if(dir == 1)
  {
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
  }
  else if(dir == -1)
  {
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
  }
  else
  {
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
  }  
}

void readEncoderMotor1()
{
  int b = digitalRead(ot2Motor1Pin);
  if(b > 0)
  {
    posiMotor1++;
  }
  else
  {
    posiMotor1--;
  }
}

void readEncoderMotor2()
{
  int b = digitalRead(ot2Motor2Pin);
  if(b > 0)
  {
    posiMotor2++;
  }
  else
  {
    posiMotor2--;
  }
}

void setupMotorPins()
{
  // motor 1
  pinMode(in1Motor1Pin, OUTPUT);
  pinMode(in2Motor1Pin, OUTPUT);
  pinMode(enaMotor1Pin, OUTPUT);
  pinMode(ot1Motor1Pin, INPUT);
  pinMode(ot2Motor1Pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(ot1Motor1Pin),readEncoderMotor1,RISING);

  // motor 2
  pinMode(in1Motor2Pin, OUTPUT);
  pinMode(in2Motor2Pin, OUTPUT);
  pinMode(enaMotor2Pin, OUTPUT);
  pinMode(ot1Motor2Pin, INPUT);
  pinMode(ot2Motor2Pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(ot1Motor2Pin),readEncoderMotor2,RISING);

  // setup PWM
  ledcSetup(LEDC_CHANNEL1, freqValue, LEDC_RESOLUTION_BITS);  // intialize channel, freq, resolution
  ledcAttachPin(enaMotor1Pin, LEDC_CHANNEL1);                 // attach a pin to output the PWM
  ledcSetup(LEDC_CHANNEL2, freqValue, LEDC_RESOLUTION_BITS);  // intialize channel, freq, resolution
  ledcAttachPin(enaMotor2Pin, LEDC_CHANNEL2);                 // attach a pin to output the PWM
}

void directionForward()
{
  // set all the driver pins to LOW initially
  digitalWrite(in1Motor1Pin, LOW);    
  digitalWrite(in2Motor1Pin, LOW);
  digitalWrite(in1Motor2Pin, LOW);    
  digitalWrite(in2Motor2Pin, LOW);
  
  // set the driver pins
  // motor 1
  digitalWrite(in1Motor1Pin, HIGH);    
  digitalWrite(in2Motor1Pin, LOW);
  // motor 2
  digitalWrite(in1Motor2Pin, HIGH);    
  digitalWrite(in2Motor2Pin, LOW);
}

void directionBackward()
{
  // set all the driver pins to LOW initially
  digitalWrite(in1Motor1Pin, LOW);    
  digitalWrite(in2Motor1Pin, LOW);
  digitalWrite(in1Motor2Pin, LOW);    
  digitalWrite(in2Motor2Pin, LOW);
  
  // set the driver pins
  // motor 1
  digitalWrite(in1Motor1Pin, LOW);    
  digitalWrite(in2Motor1Pin, HIGH);
  // motor 2+
  digitalWrite(in1Motor2Pin, LOW);    
  digitalWrite(in2Motor2Pin, HIGH);
}

void directionLeft()
{
  // set all the driver pins to LOW initially
  digitalWrite(in1Motor1Pin, LOW);    
  digitalWrite(in2Motor1Pin, LOW);
  digitalWrite(in1Motor2Pin, LOW);    
  digitalWrite(in2Motor2Pin, LOW);
  
  // set the driver pins
  // motor 1
  digitalWrite(in1Motor1Pin, LOW);    
  digitalWrite(in2Motor1Pin, HIGH);
  // motor 2
  digitalWrite(in1Motor2Pin, HIGH);    
  digitalWrite(in2Motor2Pin, LOW);
}

void directionRight()
{
  // set all the driver pins to LOW initially
  digitalWrite(in1Motor1Pin, LOW);    
  digitalWrite(in2Motor1Pin, LOW);
  digitalWrite(in1Motor2Pin, LOW);    
  digitalWrite(in2Motor2Pin, LOW);
  
  // set the driver pins
  // motor 1
  digitalWrite(in1Motor1Pin, HIGH);    
  digitalWrite(in2Motor1Pin, LOW);
  // motor 2
  digitalWrite(in1Motor2Pin, LOW);    
  digitalWrite(in2Motor2Pin, HIGH);
}

void directionNoMotion()
{
  // set all the driver pins to LOW
  digitalWrite(in1Motor1Pin, LOW);    
  digitalWrite(in2Motor1Pin, LOW);
  digitalWrite(in1Motor2Pin, LOW);    
  digitalWrite(in2Motor2Pin, LOW);
}

void driveMotors(float speedValue1, float speedValue2)
{
  ledcAnalogWrite(LEDC_CHANNEL1, speedValue1);
  ledcAnalogWrite(LEDC_CHANNEL2, speedValue2);
}

void positionControl(int botAngle, volatile int &posiMotor1, volatile int &posiMotor2)
{ 
  // calculate the encoder value corresponding to bot angle
  float rRatio = 3.6;      // ratio of bot radius to tire radius
  int target = (rRatio*botAngle*600)/360;
  
  // set target position
  int targetMotor1 = target;
  int targetMotor2 = -target;

  // PID constants
  float kp = 1000;
  float kd = 1500;
  float ki = 0;

  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  // read the position
  int posMotor1 = 0; 
  int posMotor2 = 0; 
  noInterrupts(); // disable interrupts temporarily while reading
  posMotor1 = posiMotor1;
  posMotor2 = posiMotor2;
  interrupts(); // turn interrupts back on

  // error
  int eMotor1 = posMotor1 - targetMotor1;
  int eMotor2 = posMotor2 - targetMotor2;

  if (eMotor1 !=0 && eMotor2 != 0) botRotated=0;

  // derivative
  float dedtMotor1 = (eMotor1-eprevMotor1)/(deltaT);
  float dedtMotor2 = (eMotor2-eprevMotor2)/(deltaT);

  // integral
  eintegralMotor1 +=  eMotor1*deltaT;
  eintegralMotor2 +=  eMotor2*deltaT;

  // control signal
  float uMotor1 = kp*eMotor1 + kd*dedtMotor1 + ki*eintegralMotor1;
  float uMotor2 = kp*eMotor2 + kd*dedtMotor2 + ki*eintegralMotor2;

  // motor power
  float pwrMotor1 = fabs(uMotor1);
  float pwrMotor2 = fabs(uMotor2);
  if (pwrMotor1 > 255)
  {
    pwrMotor1 = 255;
  }
  if (pwrMotor2 > 255)
  {
    pwrMotor2 = 255;
  }

  // motor direction
  int dirMotor1 = 1;
  if (uMotor1<0)
  {
    dirMotor1 = -1;
  }

  int dirMotor2 = 1;
  if (uMotor2<0)
  {
    dirMotor2 = -1;
  }

  // signal the motor
  setMotor(dirMotor1, pwrMotor1, enaMotor1Pin, in1Motor1Pin, in2Motor1Pin);
  setMotor(dirMotor2, pwrMotor2, enaMotor2Pin, in1Motor2Pin, in2Motor2Pin);

  // store previous error
  eprevMotor1 = eMotor1;
  eprevMotor2 = eMotor2;

  int rotationError = abs(posMotor1) + abs(posMotor2) - 2*target;

  if (rotationError == 0) botRotated = 1;

  Serial.print(target);
  Serial.print(" ");
  Serial.print(posMotor1);
  Serial.print(" ");
  Serial.print(posMotor2);
  Serial.print(" ");
  Serial.print(botRotated);
  Serial.println();
}
