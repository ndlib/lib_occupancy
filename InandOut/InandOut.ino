/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
 
  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 
  modified 2 Sep 2016
  by Arturo Guadalupi
 
  modified 8 Sep 2016
  by Colby Newman
*/
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed but DI

#define WLAN_SSID       "ND-guest"        // cannot be longer than 32 characters!

// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

Adafruit_CC3000_Client client;
const int leftLEDPin = 9;
const int rightLEDPin = 11;
const int centerLEDPin = 10;

const int rightSensorPin = A0;
const int leftSensorPin = A2;
const int centerSensorPin = A1;

int rightValue = 0;
int centerValue = 0;
int leftValue = 0;

int rightSensorValue = 0;
int centerSensorValue = 0;
int leftSensorValue = 0;

int right = 0;
int left = 0;

int people_count = 0;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  Serial.println(F("Hello, CC3000!\n")); 
  
  Serial.println(F("\nInitialising the CC3000 ..."));
  if (!cc3000.begin()) {
    Serial.println(F("Unable to initialise the CC3000! Check your wiring?"));
    for(;;);
  }
  
  Serial.println(F("\nDeleting old connection profiles"));
  if (!cc3000.deleteProfiles()) {
    Serial.println(F("Failed!"));
    while(1);
  }

  /* Attempt to connect to an access point */
  char *ssid = WLAN_SSID;             /* Max 32 chars */
  Serial.print(F("\nAttempting to connect to ")); Serial.println(ssid);
  
  /* NOTE: Secure connections are not available in 'Tiny' mode! */
  if (!cc3000.connectOpen(WLAN_SSID)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));

}

// the loop function runs over and over again forever
void loop() {
  rightSensorValue = analogRead(rightSensorPin);
  //delay(5);
  leftSensorValue = analogRead(leftSensorPin);
  //delay(5);
  centerSensorValue = analogRead(centerSensorPin);
//  Serial.print("Raw sensor Values \t right: ");
//  Serial.print(rightSensorValue);
//  Serial.print("\t left: ");
//  Serial.print(leftSensorValue);
//  Serial.print("\t center: ");
//  Serial.println(centerSensorValue);
  delay(50);
  if (rightSensorValue + 50 < rightValue){
    if(left == 1){
      Serial.println("Left -> Right");
      people_count++;
      left = 0; 
      delay(100); 
    }else{
      Serial.println(" Right is 1");
      right = 1;  
    }
  }
  else if (leftSensorValue + 50 < leftValue) {
    if(right == 1){
      Serial.println("Right -> Left");
      if (people_count>0){
        people_count--;
      }
      right = 0;
      delay(100);
    }else{
      Serial.println(" Left is 1");
      left = 1;  
    }
  }
  else if (centerSensorValue + 50 < centerValue) Serial.println("At Center");

  rightValue = rightSensorValue;
  leftValue = leftSensorValue;
  centerValue = centerSensorValue;

//  Serial.print("Mapped sensor Values \t right: ");
//  Serial.print(rightValue);
//  Serial.print("\t left: ");
//  Serial.print(leftValue);
//  Serial.print("\t center: ");
//  Serial.println(centerValue);
  Serial.println(people_count);
}

void pullRoom(void) {
  // Hostname to IP lookup; use NTP pool (rotates through servers)
  if(cc3000.getHostByName("https://library-room.firebaseio.com/floor/1/129", &ip)) {
    static const char PROGMEM
      timeReqA[] = { 227,  0,  6, 236 },
      timeReqB[] = {  49, 78, 49,  52 };

    Serial.println(F("\r\nAttempting connection..."));
    startTime = millis();
    do {
      client = cc3000.connectUDP(ip, 123);
    } while((!client.connected()) &&
            ((millis() - startTime) < connectTimeout));

    if(client.connected()) {
      Serial.print(F("connected!\r\nIssuing request..."));
  }
}

