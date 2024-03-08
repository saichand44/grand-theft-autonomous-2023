#include "motor.h"

#define trigPin1 19  // left
#define echoPin1 20

#define trigPin2  1  // right
#define echoPin2  2

#define trigPin3 8  // front
#define echoPin3 18

int distanceFront;
int distanceLeft;
int distanceRight;

int frontDistHistory[40];
int rightDistHistory[40];

float errorPrevRight= 0;
float eintegralRight = 0;

void setupUltrasonicPins()
{
  // ultrsonic sensor 1
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);

  // ultrsonic sensor 2
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // ultrsonic sensor 3
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
}

float calcDistance(int trigPin, int echoPin, int sensorID, bool printInfo=false)
{
  float duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
  
  if (printInfo == true)
  {
    Serial.print("[INFO]");
    Serial.print("Ultrasonic Sensor:");
    Serial.print(sensorID);
    Serial.print("-->");
    Serial.print("Distance: ");
    Serial.println(distance);
  }
  return distance;
}

int right_range(int trigPinRight, int echoPinRight)
{
  digitalWrite(trigPinRight, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPinRight, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinRight, LOW);

  int duration, range_cm, range_cm_filtered;
  duration = pulseIn(echoPinRight, HIGH);
  range_cm = duration * 0.017;
  // range_cm_filtered = movingavgRight(range_cm, 10);
  
  return range_cm;
}

int front_range(int trigPinFront, int echoPinFront)
{
  digitalWrite(trigPinFront, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPinFront, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinFront, LOW);

  int duration, range_cm, range_cm_filtered;
  duration = pulseIn(echoPinFront, HIGH);
  range_cm = duration * 0.017;
  // range_cm_filtered = movingavgFront(range_cm, 1);

  return range_cm;
}

// Function to calculate the sum of array elements
int average(int values[], int size)
{
  int result = 0;
  for (int i = 0; i < size; i++) 
  {
    result += values[i];
  }
  result = result / size;
  
  return result;
}

int averageFilterFront(int history[], int newValue, int size)
{
  // Shift values in the history array to make room for the new value
  for (int i = 0; i < size; i++)
  {
    history[i] = history[i + 1];
  }

  // Add the new value to the history array
  history[size-1] = newValue;

  // Calculate and return the median of the history array
  return average(history, size);
}

int averageFilterRight(int history[], int newValue, int size)
{
  // Shift values in the history array to make room for the new value
  for (int i = 0; i < size; i++)
  {
    history[i] = history[i + 1];
  }

  // Add the new value to the history array
  history[size-1] = newValue;

  // Calculate and return the median of the history array
  return average(history, size);
}

void wallFollow()
{
  int measuredRight = right_range(trigPin2, echoPin2);
  int measuredFront = front_range(trigPin3, echoPin3);

  distanceFront = averageFilterFront(frontDistHistory, measuredFront, 40);
  distanceRight = averageFilterRight(rightDistHistory, measuredRight, 40);
  Serial.printf("right: %d, front: %d\n", distanceRight, distanceFront);

  // PID constants (front)
  float kpF = 1000;
  float kdF = 1500;
  float kiF = 0;

  // PID constants (right)
  float kpR = 0.6;
  float kdR = 0.0;
  float kiR = 0;

   // error
  int errorRight = distanceRight - 20;

  // derivative
  float dedtRight= errorRight-errorPrevRight;

  // integral
  eintegralRight +=  errorRight;

  // control signal
  float uRight = kpR*errorRight + kdR*dedtRight + kiR*eintegralRight;

  // motor speed
  int speed = uRight;

  if (fabs(speed) > 100)                                                                                                                                                                                                                                                                                                                                                            
  {                                             
    speed = 100;
  }
    
  if (distanceFront > 20 && distanceRight > 4 && distanceRight < 25)
  {
    Serial.println("State0");
    directionForward();                                     
    driveMotors(75 + speed, 75 + speed);
  }

  if (distanceFront > 20 && distanceRight > 25)
  {
    Serial.println("State1");
    directionForward();                                     
    driveMotors(75 + speed, 75 - speed);
  }

  if (distanceFront > 20 && distanceRight < 4)
  { 
    Serial.println("State2");
    driveMotors(0, 0);
    directionLeft();
    driveMotors(50, 50);
  }

  if (distanceFront < 20)
  {
    Serial.println("State3");
    driveMotors(0, 0);
    directionLeft();
    driveMotors(50, 50);
  }
  
  errorPrevRight = errorRight;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
}