/* Project: Final
 * Name: final.ino
 * Author: <Group 9> <Sai Chand Gubbala, Rahul Birewar, Aditya Rangamani>
 * University of Pennsylvania
 */

// WiFi Parameters //
#include <WiFi.h>
WiFiServer server(80);

// UDP Parameters //
#include <WiFiUdp.h>
WiFiUDP botUDPServer;
// WiFiUDP myUDPServer;
#define BOTUDPPORT 2510 // port for game obj transmission
// #define MYUDPPORT 2808
IPAddress myIP(192, 168, 1, 107); // our IP
IPAddress targetIP(192, 168, 1, 255); // broadcast IP

// Website Parameters //
#include "html510.h"
HTML510Server h(80);

#include "motor.h"                // Motor controls
#include "beacon.h"               // IR LED sensors
#include "ultrasonic.h"           // Ultrasonic sensors
#include "webpage_controller.h"   // Website
#include "servo.h"                // Gripper controls
#include "vive_follow.h"          // VIVE sensors

// Uncomment the router SSID and Password that is being used //
// ********************************************************* //
// const char* ssid     = "TP-Link_05AF";
// const char* password = "47543454";

const char* ssid     = "TP-Link_E0C8";
const char* password = "52665134";

// const char* ssid     = "TP-Link_FD24"; 
// const char* password = "65512111";
// ********************************************************* //

#define WallFollowing 1
#define PoliceCarPushing 2
#define TrophyTracking 3
#define FakeTrophyTracking 4

int modeValue = 0;
int policeX=4000;
int policeY=4000;
char udpBuffer[100];

long lastUpdate = 0;

void handleRoot() {
  h.sendhtml(body);
}

// void fncUdpSend()
// {
//   // send what ever you want upto buffer size       
//   char s[13]; 
//   char udpBuffer = *s;      
//   char UDPbuffer[20];        
//   botUDPServer.beginPacket(targetIP, 2808);  // send to UDPport 2808
//     botUDPServer.printf("%s",udpBuffer);
//   botUDPServer.endPacket();
//   Serial.println(udpBuffer);
//   delay(100);
// }

void UDPsend(int robotX=xCenter, int robotY=yCenter)
{
  char s[13];
  char UDPbuffer[20];

  sprintf(s, "%2d:%4d,%4d", 9, robotX, robotY);
  // Serial.println(s);
  char udpBuffer = *s;

  botUDPServer.beginPacket(targetIP, BOTUDPPORT);
  // botUDPServer.write((uint8_t *)udpBuffer, 13);
  botUDPServer.println(udpBuffer);
  Serial.println(udpBuffer);
  botUDPServer.endPacket();

  //  send what ever you want upto buffer size                      
  // UDPServer.beginPacket(targetIP, 2808);  // send to UDPport 2808
  //   UDPServer.printf("%s",udpBuffer);
  // UDPServer.endPacket();
  // Serial.println(udpBuffer);
  Serial.println("Sending");
  // Serial.println(udpBuffer);
  // Serial.println("Sending data: %s", *s);
  delay(100);
}

void UDPreceive() {
   const int UDP_PACKET_SIZE = 14; // can be up to 65535          
   uint8_t packetBuffer[UDP_PACKET_SIZE];

   int cb = botUDPServer.parsePacket(); // if there is no message cb=0
   if (cb) {
      int x,y, policeCar;
      packetBuffer[13]=0; // null terminate string

    botUDPServer.read(packetBuffer, UDP_PACKET_SIZE);
      policeCar = atoi((char *)packetBuffer);
      x = atoi((char *)packetBuffer+3); // ##,####,#### 2nd indexed char
      y = atoi((char *)packetBuffer+8); // ##,####,#### 7th indexed char
      if (policeCar == 0)
      {
        policeX = x;
        policeY = y;
        Serial.println(x);
        Serial.println(y);
        Serial.println(policeCar);
      }
  }
}

void handleMode() {
  modeValue=0;
  delay(500);
  modeValue = h.getVal();   // get the mode of the robot

  h.sendhtml(body);
}

void setup() {
  int i=0;
  Serial.begin(115200);

  // STATION MODE //
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("MECHAwhy??");
  WiFi.softAPConfig(myIP, IPAddress(192, 168, 1, 1), IPAddress(255, 255,255, 0));
  // WiFi.begin(ssid, password);
  WiFi.begin(ssid);


  Serial.print("Sending messages from "); Serial.print(myIP); 
  Serial.print(" to "); Serial.println(targetIP); 
  while(WiFi.status()!=WL_CONNECTED && i++ <20) {
    delay(500);
    Serial.print(".");
  }

  // if (i <19)
  // {
  //   Serial.println("WiFi connected as ");
  //   Serial.print(WiFi.localIP());
  // }
  // Serial.print("WiFi connected");
  // myUDPServer.begin(MYUDPPORT);
  botUDPServer.begin(BOTUDPPORT);

  // HTML Setup //
  h.begin();
  h.attachHandler("/mode?value=", handleMode);     // handler to monitor mode from website
  h.attachHandler("/", handleRoot);

  // setup the pins on ESP32 S2
  setupMotorPins();
  setupBeaconPins();
  setupUltrasonicPins();
  setupVive();
  setupServoPins();

  // ROBOT START //
  directionNoMotion();
  openGripper();
  delay(2000);
  closeGripper();
}

void loop() {  
  h.serve();
  // UDPsend(xCenter, yCenter);
  // strcpy(udpBuffer, "hello testing message");
  // fncUdpSend();

  if (modeValue == WallFollowing)
  {
    wallFollow();
  }
  else if (modeValue == PoliceCarPushing)
  {
    UDPreceive();
    updateVive();
    delay(500);
    moveBot(policeX, policeY);
  }
  else if (modeValue == TrophyTracking)
  {
    // moveBot(5500, 4500);
    // moveBot(2500, 3500);
    // openGripper();
    beacon_track(550);
    if (distanceFront < 10) {
      // closeGripper();
      // moveBot(2500, 3500);
      // moveBot(5500, 4500);
      // openGripper();
    }
  }
  else if (modeValue == FakeTrophyTracking)
  {
    // moveBot(2500, 3500);
    // moveBot(5500, 4500);
    // openGripper();
    beacon_track(23);
    if (distanceFront < 10) {
      // closeGripper();
      // moveBot(5500, 4500);
      // moveBot(2500, 3500);
      // openGripper();
    }
  }
  else {
    directionNoMotion();
    driveMotors(0, 0);
  }
  // closeGripper();
}
