#include "motor.h"
#define IR_PIN_1 45

volatile long LastIR1 = 0;
volatile long NewestIR1 = 0;

int beaconTrackFreq = 10;
int servoAngle = 0;
float beaconFreq = 0;

void IRAM_ATTR IR_ISR_1()
{
  LastIR1 = NewestIR1;
  NewestIR1 = micros();
}

void attach_interrupts()
{
  attachInterrupt(digitalPinToInterrupt(IR_PIN_1), IR_ISR_1, RISING);
}

void setupBeaconPins()
{
  pinMode(IR_PIN_1, INPUT);
  attach_interrupts();
}

void beacon_track(int desiredFreq)
{
  static long BeaconLastUpdate = 0;

  if (millis()> BeaconLastUpdate + 1000/ beaconTrackFreq)
  {
    long currtime = micros();
    float freq_1 = 0;

    if (NewestIR1)
    {
        freq_1 = 1.0/((NewestIR1 - LastIR1)/1000000.0);
    }

    if (NewestIR1< micros() - 100000)
    {
      freq_1 = -1;
    }

    bool detected = false;

    if (desiredFreq == 23)
    { 
      if (freq_1> 10 && freq_1 < 30)
      {
      Serial.print("Frequency");
      Serial.println(freq_1);
      detected = true;
      }
    }

    if (desiredFreq == 550)
    {
      if (freq_1> 500 && freq_1 < 600)
      {
      Serial.print("Frequency");
      Serial.println(freq_1);
      detected = true;
      }
    }

    if (detected)
    {
      driveMotors(0, 0);
      directionForward();
      driveMotors(100, 100);
    }
    else
    {
      directionLeft();
      driveMotors(75, 75);
    }
    
  }
}
