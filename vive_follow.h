/*
   broadcasts data packets to port 2510
   Demonstrates high precision vive localization and displays on RGB LED

*/
#include "vive510.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "motor.h"

#define RGBLED 18 // for ESP32S2 Devkit pin 18, for M5 stamp=2
#define SIGNALPIN1 4 // pin receiving signal from Vive circuit
#define SIGNALPIN2 5
#define FREQ 1 // in Hz

int rotateBot=0;

uint16_t xCenter,yCenter;
uint16_t xRear,yRear;

int X1[40];
int Y1[40];
int X2[40];
int Y2[40];

Vive510 vive1(SIGNALPIN1);
Vive510 vive2(SIGNALPIN2);

int calcRobotAngle(int V1X, int V1Y, int V2X, int V2Y)
{
  int refV01X = 6150;
  int refV01Y = 2800;
  int refV02X = 6150;
  int refV02Y = 5200;

  int refAxis[2];
  int currAxis[2];

  refAxis[0] = refV02X - refV01X;
  refAxis[1] = refV02Y - refV01Y;
  float refAxisMag = sqrt(pow(refAxis[0], 2) + pow(refAxis[1], 2));

  currAxis[0] = V2X - V1X;
  currAxis[1] = V2Y - V1Y;
  float currAxisMag = sqrt(pow(currAxis[0], 2) + pow(currAxis[1], 2));

  float dotProduct = refAxis[0] * currAxis[0] + refAxis[1] * currAxis[1];

  float cosAngle = dotProduct / (refAxisMag * currAxisMag);
  // Serial.println(cosAngle);

  float angle = acos(cosAngle);
  angle = angle * 180 / 3.14;
  // Serial.println(angle);

  return int(angle);
}

// Function to calculate the sum of array elements
int averageVive(int values[], int size)
{
  int result = 0;
  for (int i = 0; i < size; i++) 
  {
    result += values[i];
  }
  result = result / size;
  
  return result;
}

int averageFilterX1(int history[], int newValue, int size)
{
  // Shift values in the history array to make room for the new value
  for (int i = 0; i < size; i++)
  {
    history[i] = history[i + 1];
  }

  // Add the new value to the history array
  history[size-1] = newValue;

  // Calculate and return the median of the history array
  return averageVive(history, size);
}

int averageFilterY1(int history[], int newValue, int size)
{
  // Shift values in the history array to make room for the new value
  for (int i = 0; i < size; i++)
  {
    history[i] = history[i + 1];
  }

  // Add the new value to the history array
  history[size-1] = newValue;

  // Calculate and return the median of the history array
  return averageVive(history, size);
}

int averageFilterX2(int history[], int newValue, int size)
{
  // Shift values in the history array to make room for the new value
  for (int i = 0; i < size; i++)
  {
    history[i] = history[i + 1];
  }

  // Add the new value to the history array
  history[size-1] = newValue;

  // Calculate and return the median of the history array
  return averageVive(history, size);
}

int averageFilterY2(int history[], int newValue, int size)
{
  // Shift values in the history array to make room for the new value
  for (int i = 0; i < size; i++)
  {
    history[i] = history[i + 1];
  }

  // Add the new value to the history array
  history[size-1] = newValue;

  // Calculate and return the median of the history array
  return averageVive(history, size);
}

void reachTargetY(int currentY, int targetY)
{
  // PID constants
  float kp = 10;
  float kd = 1.0;
  float ki = 0;

  static int errorPrevY=0;
  static int eintegralY=0;

  //error
  int errorY = currentY - targetY;

  // derivative
  int dedtY= errorY-errorPrevY;

  // integral
  eintegralY +=  errorY;

  // control signal
  int uY = kp*errorY + kd*dedtY + ki*eintegralY;

  // motor speed
  int speed = uY;

  if (fabs(speed) > 100)                                                                                                                                                                                                                                                                                                                                                            
  {                                             
    speed = 100;
  }

  if (errorY > 0)
  {
    directionForward();                                     
    driveMotors(75 + speed, 75 + speed);
  }
  else if (errorY < 0)
  {
    directionBackward();                                     
    driveMotors(75 + speed, 75 + speed);
  }
  else
  {
  directionNoMotion();                                     
  driveMotors(0, 0);
  }

  errorPrevY = errorY;
}

void setupVive() {
  vive1.begin();
  vive2.begin();
  Serial.println("  Vive trackers started");
}

void updateVive()
{
  if (vive1.status() == VIVE_RECEIVING && vive2.status() == VIVE_RECEIVING) {
    xCenter = averageFilterX1(X1, vive1.xCoord(), 40);
    yCenter = averageFilterY1(Y1, vive1.yCoord(), 40);
    xRear = averageFilterX2(X2, vive2.xCoord(), 40);
    yRear = averageFilterY2(Y2, vive2.yCoord(), 40);
    neopixelWrite(RGBLED, 0,(xCenter+xRear)/400,(yCenter+yRear)/400); // blue to greenish
  }

  else {
    xCenter = 0;
    yCenter = 0;
    xRear = 0;
    yRear = 0;
    switch (vive1.sync(5) && vive2.sync(5)) {
        break;
      case VIVE_SYNC_ONLY: // missing sweep pulses (signal weak)
        neopixelWrite(RGBLED, 64, 32, 0); // yellowish
        break;
      default:
      case VIVE_NO_SIGNAL: // nothing detected
        neopixelWrite(RGBLED, 128, 0, 0); // red
    }
  }
  delay(20);
}

void moveBot(int targetX=4000, int targetY=4000) {

  int robotY =  (yCenter+yRear)/2;
  int robotX =  (xCenter+xRear)/2;

  static int stopBot = 0;
  static int rotateBot = 0;

  // reachTargetY(robotY, 4000);
  int errorY = robotY - targetY;
  int errorX = robotX - targetX;

  if (errorY > 0 && stopBot == 0)
  {
    directionBackward();
    driveMotors(50, 50);
  }

  else if (errorY < 0 && stopBot ==0)
  {
    directionForward();
    driveMotors(50, 50);
  }

  else 
  {
    directionForward();
    driveMotors(0, 0);
    stopBot = 1;
  }
  // Serial.println("Stopbot:");
  // Serial.println(stopBot);
  // Serial.println("RotateBot:");
  // Serial.println(rotateBot);

  if (stopBot == 1 && rotateBot == 0)
  {
    if (errorX > 0)
    {
      Serial.println(errorX);
      directionLeft();
      driveMotors(50, 50);
      rotateBot = 1; 
      delay(1600);
    }
    else if (errorX < 0)
    { 
      Serial.println(errorX);
      directionRight();
      driveMotors(50, 50);
      rotateBot = 1;
      delay(1600);
    }
  }

  if (stopBot == 1 && rotateBot == 1)
  {
    directionForward();
    driveMotors(100, 100);
  }
}