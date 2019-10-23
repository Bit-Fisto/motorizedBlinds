//Automatic Blind Controller v2.02
//Copyright CABUU.COM
//Arduino Sketch Version 2.02
//29 May 2019

//v2.02 Include safety cut off feature and LED (requires udate to Blynk app)

//This is a beta, and works for CCW (UP) and CW (Down) motion only
//Replicates a WemoSwitch in Alexa
//Replace the relevant variables below

//------- Substitute your own variables below ------
char ssid[] = "WIFI SSD HERE";         // your network SSID (name)
char password[] = "WIFI PASS HERE";  // your network key
char DeviceName[] = "Blind Controller";       //Name of the device as seen by Alexa etc.

boolean useBlynk = true;                    //Use Blynk
char auth[] = "BLYNK AUTH TOKEN HERE"; //Blynk authentication token
//End of user defined variables

#include "WemoSwitch.h"
#include "WemoManager.h"
#include "CallbackFunction.h"

#include <Adafruit_NeoPixel.h>
#define PIN            D3
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

// prototypes
boolean connectWifi();

WemoManager wemoManager;
WemoSwitch *light = NULL;

const int ledPin = BUILTIN_LED;

unsigned long previousMillis = 0;   

#include <Encoder.h>

int upButtonPin = 15;
int downButtonPin = 13;

Encoder myEnc(14,12);

#define BLYNK_PRINT Serial
 
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

long buttonTimer = 0;
long longPressTime = 500;

boolean DownButtonActive = false;
boolean DownLongPressActive = false;

boolean UpButtonActive = false;
boolean UpLongPressActive = false;

int moveToggle = 0; //o not moving, 1 moving

//Was relates to mounting on oppsosite side, may be useful
//Arduino PWM Speed Controlï¼š
//int E1 = 5; // was 5 
//int M1 = 4; // was 4 


//Mounted on RHS
int E1 = 4; // was 5 
int M1 = 5; // was 4 


#include <EEPROM.h>

int blindPosition;

long closedPosition = 1000;
long openPosition = -1000;
long currPosition = 0;
long currentPosition = myEnc.read();
long targetPosition = 0;
long travelLength = 2000;
double travelPercent;
int blindState; //1 is open, 0 is closed

//Safety cut off variables
long movementStartTimer;
long movementTimer;
long movementStartPosition;
boolean errorFlag = false;

//EEPROM Address
int addrclosedPosition = 30; 
int addropenPosition = 40; 
//int addrcurrentPosition = 50; 
int addrblindState = 50; 
int addrcurrPosition = 60; 

BlynkTimer timer;

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (pinValue == 1){
    //Moving From 1 to the next
    Serial.print("Closed Position = ");
    Serial.println(closedPosition);
    Serial.print("Open Position = ");
    Serial.println(openPosition);
    currentPosition = myEnc.read();
    Serial.print("CurrentPosition = ");
    Serial.print(currentPosition);


    if (blindState == 1) { //Blinds are open, close
      Serial.println("Closing1...");
      int potValue=700;
       //Going backward this is th backwarrds code should be as below
      int mappedVal = map(potValue-512,0,512,0,255);

      //digitalWrite(E1,HIGH);     
      analogWrite(E1, mappedVal);   //PWM Speed Control  

      digitalWrite(E1,HIGH);     
      analogWrite(M1, mappedVal);   //PWM Speed Control  
      
      pixels.setPixelColor(0,255,165,0);
      pixels.show();

      
      while(currentPosition < closedPosition){
        
        currentPosition = myEnc.read();

        //Write as we go
        travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);
        Blynk.virtualWrite(V8, travelPercent);
        Blynk.virtualWrite(V9, travelPercent);
        delay(50);

                      
        delay(0);
      }

      pixels.setPixelColor(0,0,0,0);
      pixels.show();

      blindState = 0; //Closed
      digitalWrite(M1,LOW);  
      digitalWrite(E1,LOW);
      //Blynk.virtualWrite(V3, currentPosition);

    //Update to memory
    EEPROM.begin(512);
    EEPROM.put(addrblindState,blindState);
    EEPROM.put(addrcurrPosition,currentPosition);
    Serial.print("Blinds Closed");
    EEPROM.commit(); 
    flash_LED(1,String("Blue"));
    
    } else {

    int potValue=300;
    Serial.println("Opening V2...");
    int mappedVal = map(potValue-512,0,512,0,255);

      //digitalWrite(M1,HIGH);
      analogWrite(M1, mappedVal);   //PWM Speed Control

      digitalWrite(M1,HIGH);
      analogWrite(E1, mappedVal);   //PWM Speed Control

      pixels.setPixelColor(0,255,165,0);
      pixels.show();
      
      while(currentPosition > openPosition){
        Serial.print(currentPosition);
        Serial.print(">");
        Serial.println(openPosition);
        currentPosition = myEnc.read();

        //Write as we go
        travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);
        Blynk.virtualWrite(V8, travelPercent);
        Blynk.virtualWrite(V9, travelPercent);
        delay(50);
        
        delay(0);
      }
      
      pixels.setPixelColor(0,0,0,0);
      pixels.show(); 

      digitalWrite(E1,LOW);
      digitalWrite(M1,LOW);
      //Blynk.virtualWrite(V3, currentPosition);
    blindState = 1; //Open

    //Update to memory
    EEPROM.begin(512);
    EEPROM.put(addrblindState,blindState);
    EEPROM.put(addrcurrPosition,currentPosition);
    Serial.print("Blinds Open");
    EEPROM.commit(); 
    flash_LED(1,String("Blue"));

      
    }
    
  }
}


BLYNK_WRITE(V4)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  
  if (pinValue == 1){
//    digitalWrite(E1,HIGH);
//    Serial.println("E1 High"); 
//
//    int potValue=700;
//     //Going backward this is th backwarrds code should be as below
//    int mappedVal = map(potValue-512,0,512,0,255);
//    analogWrite(M1, mappedVal);   //PWM Speed Control
//    Serial.println("Analog to M1"); 

            int potValue=700;
            Serial.println("Opening Via WWW...");
            int mappedVal = map(potValue-512,0,512,0,255);
        
              //digitalWrite(M1,HIGH);
              analogWrite(E1, mappedVal);   //PWM Speed Control
        
              digitalWrite(E1,HIGH);
              analogWrite(M1, mappedVal);   //PWM Speed Control
        
              pixels.setPixelColor(0,255,165,0);
              pixels.show();


              delay(100);
              moveToggle = 1;
    
  } else {
                  moveToggle = 0;
                  
                  digitalWrite(E1,LOW);
                  digitalWrite(M1,LOW);
                  pixels.setPixelColor(0,0,0,0);
                  pixels.show();
                  Blynk.virtualWrite(V3, myEnc.read());

                  //Update Blynk display and slider
                  currentPosition = myEnc.read();
                  Blynk.virtualWrite(V3, currentPosition);
                  travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);
                  Blynk.virtualWrite(V8, travelPercent);
                  Blynk.virtualWrite(V9, travelPercent);
                  
                  //Update EEPROM with position and set blind status to neither open or closed
                  blindState = 2;
                  currentPosition = myEnc.read();
                  EEPROM.begin(512);
                  EEPROM.put(addrblindState,blindState);
                  EEPROM.put(addrcurrPosition,currentPosition);
                  EEPROM.commit(); 
    
  }
}

BLYNK_WRITE(V5)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  
  if (pinValue == 1){
    
          moveToggle = 1;
          
//    digitalWrite(M1,HIGH);
//    Serial.println("M1 High"); 
//    int potValue=100;
//     //Going backward this is th backwarrds code should be as below
//    int mappedVal = map(potValue-512,0,512,0,255);
//    analogWrite(E1, mappedVal);   //PWM Speed Control
//    Serial.println(mappedVal); 

           int potValue=300;
            Serial.println("Closing Via WWW...");
            int mappedVal = map(potValue-512,0,512,0,255);
        
              //digitalWrite(M1,HIGH);
              analogWrite(M1, mappedVal);   //PWM Speed Control
        
              digitalWrite(M1,HIGH);
              analogWrite(E1, mappedVal);   //PWM Speed Control
        
              pixels.setPixelColor(0,255,165,0);
              pixels.show();

              delay(100);

  } else {
                moveToggle = 0;
    
//    digitalWrite(M1,LOW);
//    Serial.println("M1 Low"); 

                   digitalWrite(E1,LOW);
                  digitalWrite(M1,LOW);
                  pixels.setPixelColor(0,0,0,0);
                  pixels.show();


                  //Write as we go
                  currentPosition = myEnc.read();
                  Blynk.virtualWrite(V3, currentPosition);
                  travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);
                  Blynk.virtualWrite(V8, travelPercent);
                  Blynk.virtualWrite(V9, travelPercent);

                  //Update EEPROM with position and set blind status to neither open or closed
                  blindState = 2;
                  currentPosition = myEnc.read();
                  EEPROM.begin(512);
                  EEPROM.put(addrblindState,blindState);
                  EEPROM.put(addrcurrPosition,currentPosition);
                  EEPROM.commit(); 


 
  }
}

BLYNK_WRITE(V6) //Set Open Position
{
  int pinValue = param.asInt(); // assigning incoming value from pin V6 to a variable

  if (pinValue == 1){
    openPosition = myEnc.read();
    Blynk.virtualWrite(V3, openPosition);
    blindState = 1; //Blinds must be open
  
    //Update to memory
    EEPROM.begin(512);
    EEPROM.put(addrblindState,blindState);
    EEPROM.put(addropenPosition,openPosition);
    EEPROM.put(addrcurrPosition,currentPosition);
    Serial.print("Blinds Open Position Updated!");
    EEPROM.commit();   
    flash_LED(1,String("Blue"));

    //Update Total Travel Length
    travelLength = abs(openPosition-closedPosition);
    travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);
    
    //update Blynk app
    Blynk.virtualWrite(V8, travelPercent);
    Blynk.virtualWrite(V9, travelPercent);
  }


}

BLYNK_WRITE(V7) //Set Closed Position
{
  int pinValue = param.asInt(); // assigning incoming value from pin V7 to a variable
  
  if (pinValue == 1){
    closedPosition = myEnc.read();
    Blynk.virtualWrite(V3, closedPosition);
    blindState = 0; //Blinds must be closed
  
    //Update to memory
    EEPROM.begin(512);
    EEPROM.put(addrblindState,blindState);
    EEPROM.put(addrclosedPosition,closedPosition);
    EEPROM.put(addrcurrPosition,currentPosition);
    Serial.print("Blinds Closed Position Updated!");
    EEPROM.commit(); 
    flash_LED(1,String("Blue"));   

    //Update Total Travel Length
    travelLength = abs(openPosition-closedPosition);
    travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);

    //update Blynk app
    Blynk.virtualWrite(V8, travelPercent);
    Blynk.virtualWrite(V9, travelPercent);
    
  }

}


BLYNK_WRITE(V10) //Calling for blinds to be opened
{
  int pinValue = param.asInt(); // assigning incoming value from pin V10 to a variable
  
  if (pinValue == 1){

    //Open blinds
    openBlinds();

  
  }

}

BLYNK_WRITE(V11) //Calling for blinds to be closed
{
  int pinValue = param.asInt(); // assigning incoming value from pin V11 to a variable
  
  if (pinValue == 1){

    //Get the current blind position
    currentPosition = myEnc.read();

    //Close blinds
    closeBlinds();  
  }

}


BLYNK_WRITE(V12) //Changing LED Brightness
{
  int pinValue = param.asInt(); // assigning incoming value from pin V10 to a variable
  pixels.setBrightness(pinValue);

}

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  
  int val = digitalRead(blindPosition);
  
  if ((pinValue == 1) & (blindState != 1)){
    //Open BLinds
    
    ///Get Current Position (Closed)
    closedPosition = myEnc.read();
    currentPosition = myEnc.read();
    
    Serial.println("Opening...");
    int potValue=700;
     //Going backward this is th backwarrds code should be as below
    int mappedVal = map(potValue-512,0,512,0,255);
    digitalWrite(E1,HIGH);         
    analogWrite(M1, mappedVal);   //PWM Speed Control
delay(1000);

    digitalWrite(E1,LOW); 
    currentPosition = myEnc.read();   
    Blynk.virtualWrite(V3, currentPosition);
    blindState = 1;

    //Update to memory
    EEPROM.begin(512);
    EEPROM.put(addrblindState,blindState);
    EEPROM.put(addrclosedPosition,closedPosition);
    EEPROM.put(addrcurrPosition,currentPosition);
    Serial.print("Blinds Open");
    EEPROM.commit(); 
    flash_LED(1,String("Blue"));
  }
  else if ((pinValue == 0) & (blindState == 1))
  {
    //Close Blinds

    ///Get Current Position OPEN
    openPosition = myEnc.read();
    
    int potValue=700;
    Serial.println("Closing2...");
    int mappedVal = map(potValue-512,0,512,0,255);
    digitalWrite(M1,HIGH);     
    analogWrite(E1, mappedVal);   //PWM Speed Control

    delay(1000);
    digitalWrite(M1,LOW); 
    currentPosition = myEnc.read();   
    Blynk.virtualWrite(V3, currentPosition);
    blindState = 0;
    
    //Update to memory
    EEPROM.begin(512);
    EEPROM.put(addrblindState,blindState);
    EEPROM.put(addropenPosition,openPosition);
    EEPROM.put(addrcurrPosition,currentPosition);
    Serial.print("Blinds Closed");
    EEPROM.commit(); 
    flash_LED(1,String("Blue"));
  }
  
}



BLYNK_WRITE(V9) //Set blind position based on slider
{
  Serial.println("V9 activated");
  int pinValue = param.asInt(); // assigning incoming value from pin V10 to a variable
  travelPercent = double(pinValue);
  double travelFraction = (travelPercent/100); 
  
  currentPosition = myEnc.read();
  targetPosition = openPosition-(abs(travelLength*travelFraction));

Serial.println("Target:");
Serial.println(targetPosition);


Serial.println("Current:");

Serial.println(currentPosition);


Serial.println("Closed Position:");
Serial.println(closedPosition);

Serial.println("Open Position:");
Serial.println(openPosition);

  //Need to check if we are opening or closing some more
  if ((currentPosition > targetPosition) && (targetPosition > closedPosition)) { //Closing

    Serial.println("Close some more");
    int potValue=300;
    int mappedVal = map(potValue-512,0,512,0,255);
        
    //digitalWrite(M1,HIGH);
    analogWrite(M1, mappedVal);   //PWM Speed Control
        
    digitalWrite(M1,HIGH);
    analogWrite(E1, mappedVal);   //PWM Speed Control
        
    pixels.setPixelColor(0,255,165,0);
    pixels.show();
              
    while(currentPosition > targetPosition) {
      //Write as we go
      Serial.println("Closing some more");
      travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);
      Blynk.virtualWrite(V8, travelPercent);
      Blynk.virtualWrite(V9, travelPercent);
      currentPosition = myEnc.read();
      delay(50);
    }

    
  } else if ((currentPosition < targetPosition) && (targetPosition < openPosition)) { //Opening
    
    Serial.println("Open some more");
    int potValue=700;
    int mappedVal = map(potValue-512,0,512,0,255);
  
    //digitalWrite(M1,HIGH);
    analogWrite(E1, mappedVal);   //PWM Speed Control
  
    digitalWrite(E1,HIGH);
    analogWrite(M1, mappedVal);   //PWM Speed Control
  
    pixels.setPixelColor(0,255,165,0);
    pixels.show();
          
    while(currentPosition < targetPosition){
      //Write as we go
      travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);
      Blynk.virtualWrite(V8, travelPercent);
      Blynk.virtualWrite(V9, travelPercent);
      currentPosition = myEnc.read();
      delay(50);
     }


  }
  
                  
    delay(0);
    digitalWrite(E1,LOW);
    digitalWrite(M1,LOW);
    pixels.setPixelColor(0,0,0,0); 
    pixels.show();

    //Update EEPROM to reflect new position and blind status

    currentPosition = myEnc.read();
    blindState = 2; //Blind status unknown
    
    //Update to memory
    EEPROM.begin(512);
    EEPROM.put(addrblindState,blindState);
    EEPROM.put(addrcurrPosition,currentPosition);
    EEPROM.commit();  
    flash_LED(1,String("Blue")); 
}



void setup()
{
  // Debug console
  Serial.begin(9600);

  //NeoPixel library
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(200);
  
  if (digitalRead(downButtonPin) == HIGH) {
    Serial.println("Resetting...");
    flash_LED(8,String("Green")); 
    delay(3000);
    resetAll();
  }

  Serial.println("Initiallising...");
  //Turn on led as boot status
  pixels.setPixelColor(0,255,165,0); 
  pixels.show();
  delay(2000);

  Serial.println("Blind Controller v2.02");
//  Serial.println("currentPosition =");
//  Serial.println(currentPosition);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    flash_LED(1,String("Red"));  //Flash orange LED while trying to connect
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  wemoManager.begin();
  // Format: Alexa invocation name, local port no, on callback, off callback
  light = new WemoSwitch(DeviceName, 80, openBlinds, closeBlinds);
  wemoManager.addDevice(*light);

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH); // Wemos BUILTIN_LED is active Low, so high is off




  








    // Flash green 3 times to inidciate sucesful startup
        pixels.setPixelColor(0,0,255,0);
        pixels.show();
        delay(100);
        pixels.setPixelColor(0,0,0,0); 
        pixels.show();
        delay(100);
        pixels.setPixelColor(0,0,255,0);
        pixels.show();
        delay(100);
        pixels.setPixelColor(0,0,0,0); 
        pixels.show();
        delay(100);
        pixels.setPixelColor(0,0,255,0);
        pixels.show();
        delay(100);
        pixels.setPixelColor(0,0,0,0); 
        pixels.show();
    //End flash


uint addr = 0;
//
//  struct { 
//    uint val = 0;
//    char str[20] = "";
//  } data;

EEPROM.begin(512);

//Read in settings from memory, if any missing set as default

EEPROM.get(addrclosedPosition,closedPosition);
EEPROM.get(addropenPosition,openPosition);
EEPROM.get(addrblindState,blindState);
EEPROM.get(addrcurrPosition,currPosition);

      Serial.println("POSTIONS PRE");
      Serial.print("Open Position =");
      Serial.println(openPosition);
      Serial.print("Closed Position =");
      Serial.println(closedPosition);
      Serial.print("Current Position =");
      Serial.println(currPosition);

if (blindState == 0) {
  Serial.print("Blinds Closed");
} else {
  Serial.print("Blinds Open");
}

if ((blindState == 0)) {
  //If blinds closed
      openPosition = (abs(openPosition-closedPosition));
      closedPosition = 0;
      currentPosition = 0;
            Serial.println("POSTIONS UPDATED!1");
      Serial.print("Open Position =");
      Serial.println(openPosition);
      Serial.print("Closed Position =");
      Serial.println(closedPosition);
      Serial.print("Current Position =");
      Serial.println(currentPosition);
} else if ((blindState == 1)) {
  //If blinds open
      closedPosition = closedPosition-openPosition;
      openPosition = 0;
      currentPosition = 0;
      Serial.println("POSTIONS UPDATED!2");
      Serial.print("Open Position =");
      Serial.println(openPosition);
      Serial.print("Closed Position =");
      Serial.println(closedPosition);
      Serial.print("Current Position =");
      Serial.println(currentPosition);
} else {
      //flash_LED(6,String("Red"));
      openPosition = openPosition-currPosition;
      closedPosition = closedPosition-currPosition;
      currentPosition = 0;
      Serial.println("POSTIONS UPDATED3!");
      Serial.print("Open Position =");
      Serial.println(openPosition);
      Serial.print("Closed Position =");
      Serial.println(closedPosition);
      Serial.print("Current Position =");
      Serial.println(currentPosition);
}

  if(useBlynk == true){
    Blynk.begin(auth, ssid, password);
  }

  travelLength = abs(openPosition-closedPosition);
  travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);

  if(useBlynk == true){
    Blynk.virtualWrite(V8, travelPercent);
    Blynk.virtualWrite(V9, travelPercent);
    timer.setInterval(1000L, myTimerEvent);

    //Sync with values saved since last sleep
    delay(500);
    Serial.println("Syncing with Blynk");
    Blynk.syncAll();
    delay(500);
    Serial.println("Done!");

    //Flash LED 4 times
    flash_LED(4,String("Green"));
  }

  //Blynk LED
  WidgetLED StatusLED(V13);
  StatusLED.on();
  
  //Set App status LED to Green to show we are connected
  Blynk.setProperty(V13, "color","#4ca64c");      // Set staus led in app to green
  
  
}

long oldPosition  = -999;

void myTimerEvent()
{
}


 
void loop()
{

  wemoManager.serverLoop();


  unsigned long currentMillis = millis();

  //Clear EEPROM
//    EEPROM.begin(512);
//    EEPROM.put(addrclosedPosition,closedPosition);
//    EEPROM.put(addropenPosition,openPosition);
//    EEPROM.commit(); 
  //Clear EPROM


    
  long newPosition = myEnc.read();
//  if (newPosition != oldPosition) {
//    oldPosition = newPosition;
//    Serial.println(newPosition);
//    Blynk.virtualWrite(V3, newPosition);
//    delay(100);
//  }

//fauxmo.handle();


  if(useBlynk == true){
    Blynk.run();
    timer.run(); // Initiates BlynkTimer
  }
  
    
    if (digitalRead(downButtonPin) == HIGH) {
  
    if (DownButtonActive == false) { //Button presed
      DownButtonActive = true;
      
      //Record current time
      buttonTimer = millis();
    }

    if ((millis() - buttonTimer > longPressTime) && (DownLongPressActive == false)) { // Button Held
      DownLongPressActive = true;

      while(digitalRead(downButtonPin) == HIGH) { 
            int potValue=700;
            Serial.println("Closing V2...");
            int mappedVal = map(potValue-512,0,512,0,255);
        
              //digitalWrite(M1,HIGH);
              analogWrite(M1, mappedVal);   //PWM Speed Control
        
              digitalWrite(M1,HIGH);
              analogWrite(E1, mappedVal);   //PWM Speed Control
        
              pixels.setPixelColor(0,255,165,0);
              pixels.show();

              Serial.println("Down");
              currentPosition = myEnc.read();
              //currentPosition = float((currentPosition/100000)*1023);
              //Write as we go
              travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);

              if(useBlynk == true){
                Blynk.virtualWrite(V8, travelPercent);
                Blynk.virtualWrite(V9, travelPercent);
                Blynk.virtualWrite(V3, currentPosition);
              }
              delay(50); 
      }
    } 


                  
    
  } else {

    if (DownButtonActive == true) {

      if (DownLongPressActive == true) { // Button Released following long hold
        DownLongPressActive = false;
        //Button released, stop motors
        Serial.println("Released Button");
        digitalWrite(E1,LOW);
        digitalWrite(M1,LOW);

        //Update new position
        currentPosition = myEnc.read();
        currentPosition = float((currentPosition/100000)*1023);

        if(useBlynk == true){
          Blynk.virtualWrite(V3, currentPosition);
        }

        
        // Wait for 3 seconds, to check if we were just moving or recalibrating
        delay(1000);
        
        // If button held again blink led twice to confrim recalibration
        if(digitalRead(downButtonPin) == HIGH) {

          // Recalibration Instructed, write new up position to memory 
          closedPosition = myEnc.read();

          if(useBlynk == true){
            Blynk.virtualWrite(V3, closedPosition);
          }
          blindState = 0; //Blinds must be closed
  
          //Update to memory
          EEPROM.begin(512);
          EEPROM.put(addrblindState,blindState);
          EEPROM.put(addrclosedPosition,closedPosition);
          EEPROM.put(addrcurrPosition,currentPosition);
          Serial.print("Blinds Closed Position Updated!");
          EEPROM.commit();  
          flash_LED(1,String("Blue")); 

          //Update Total Travel Length
          travelLength = abs(openPosition-closedPosition);

          //Update Blynk app
          travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);

          if(useBlynk == true){
            Blynk.virtualWrite(V8, travelPercent);
            Blynk.virtualWrite(V9, travelPercent);
          }

          //EEPROM.writeInt(0,CurrentState);   
          //delay(3000); //Wait 3 seconds to give user a chance to let go of button
          //Flash LED to confirm
          flash_LED(4,String("Yellow"));
        }

        //else just turn off led
        else {
          pixels.setPixelColor(0,0,0,0);
          pixels.show();
        }

      } else { // Button released following single press
        closeBlinds();
      }

      DownButtonActive = false;
      //digitalWrite(LED2, LOW);

    }

  }
            
    
  //end down button
    
  
    

  if (digitalRead(upButtonPin) == HIGH) {
  
    if (UpButtonActive == false) { //Button presed
      UpButtonActive = true;
      
      //Record current time
      buttonTimer = millis();
    }

    if ((millis() - buttonTimer > longPressTime) && (UpLongPressActive == false)) { // Button Held
      UpLongPressActive = true;

      while(digitalRead(upButtonPin) == HIGH) { 
            int potValue=700;
            Serial.println("Opening V2...");
            int mappedVal = map(potValue-512,0,512,0,255);
        
              //digitalWrite(M1,HIGH);
              analogWrite(E1, mappedVal);   //PWM Speed Control
        
              digitalWrite(E1,HIGH);
              analogWrite(M1, mappedVal);   //PWM Speed Control
        
              pixels.setPixelColor(0,255,165,0);
              pixels.show();

              Serial.println("UP");
              currentPosition = myEnc.read();
              //currentPosition = float((currentPosition/100000)*1023);
              //Write as we go
              travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);

              if(useBlynk == true){
                Blynk.virtualWrite(V8, travelPercent);
                Blynk.virtualWrite(V9, travelPercent);
                Blynk.virtualWrite(V3, currentPosition);
              }
              delay(50); 
      }
    } 


                  
    
  } else {

    if (UpButtonActive == true) {

      if (UpLongPressActive == true) { // Button Released following long hold
        UpLongPressActive = false;
        //Button released, stop motors
        Serial.println("Released Button");
        digitalWrite(E1,LOW);
        digitalWrite(M1,LOW);

        //Update new position
        currentPosition = myEnc.read();
        currentPosition = float((currentPosition/100000)*1023);

        if(useBlynk == true){
          Blynk.virtualWrite(V3, currentPosition);
        }

        
        // Wait for 3 seconds, to check if we were just moving or recalibrating
        delay(1000);
        
        // If button held again blink led twice to confrim recalibration
        if(digitalRead(upButtonPin) == HIGH) {

          // Recalibration Instructed, write new up position to memory 
          openPosition = myEnc.read();

          if(useBlynk == true){
          Blynk.virtualWrite(V3, openPosition);
          }
          blindState = 1; //Blinds must be open
  
          //Update to memory
          EEPROM.begin(512);
          EEPROM.put(addrblindState,blindState);
          EEPROM.put(addropenPosition,openPosition);
          EEPROM.put(addrcurrPosition,currentPosition);
          Serial.print("Blinds Open Position Updated!"); 
          EEPROM.commit(); 
          flash_LED(1,String("Blue"));  

          //Update Total Travel Length
          travelLength = abs(openPosition-closedPosition);

          //Updated Blynk app
          travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);

          if(useBlynk == true){
          Blynk.virtualWrite(V8, travelPercent);
          Blynk.virtualWrite(V9, travelPercent);
          }

          //EEPROM.writeInt(0,CurrentState);   
          //delay(3000); //Wait 3 seconds to give user a chance to let go of button
          //Flash LED to confirm
          flash_LED(4,String("Yellow"));
        }

        //else just turn off led
        else {
          pixels.setPixelColor(0,0,0,0);
          pixels.show();
        }

      } else { // Button released following single press
        openBlinds();
      }

      UpButtonActive = false;
      //digitalWrite(LED2, LOW);

    }

  }


  //Check if we are moving if so update Blynk as we do
  if (moveToggle == 1){
      Serial.println("moving");
      currentPosition = myEnc.read();
      travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);
      Serial.println(currentPosition);
      if(useBlynk == true){
        Blynk.virtualWrite(V8, travelPercent);
        Blynk.virtualWrite(V9, travelPercent);
      }
  }

}





void flash_LED(int noTimes, String LEDcolor)
{

int x = 0;
  while (x <= noTimes-1) {
    
    if (LEDcolor == "Yellow"){
      pixels.setPixelColor(0,255,165,0);
    } else if (LEDcolor == "Red"){
      pixels.setPixelColor(0,255,0,0);
    } else if (LEDcolor == "Green"){
      pixels.setPixelColor(0,0,255,0);
    } else if (LEDcolor == "Blue"){
      pixels.setPixelColor(0,0,100,255);
    } else {
      pixels.setPixelColor(0,204,0,204);
    }

  ++x;

  pixels.show(); 
  delay(100);
  pixels.setPixelColor(0,0,0,0);
  pixels.show();
  delay(500);
  }
}




void openBlinds() {
  
    //Get the current blind position
    currentPosition = myEnc.read();

    //Start the timer and recod current position as start
    movementStartTimer = millis();
    movementStartPosition = currentPosition;
    errorFlag = false;

    //First check if blinds are not already open or beyond the open position
    if (currentPosition < openPosition){
      //Begin opening
      int potValue=700;
      int mappedVal = map(potValue-512,0,512,0,255);

      //Turn on the motor
      analogWrite(E1, mappedVal);   //PWM Speed Control
      digitalWrite(E1,HIGH);
      analogWrite(M1, mappedVal);   //PWM Speed Control

      //Turn on LED while we move
      pixels.setPixelColor(0,255,165,0);
      pixels.show();

      Blynk.setProperty(V13, "color","#ffff00"); //Set Blynk LED to orange
    
      while(currentPosition < openPosition) {
        //Write as we go
        travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);

        if(useBlynk == true){
          Blynk.virtualWrite(V8, travelPercent);
          Blynk.virtualWrite(V9, travelPercent);
        }
        
        //Serial.println(currentPosition);
        currentPosition = myEnc.read();
        delay(0);
        yield();
        //Check if button pressed if so stop
        if (digitalRead(downButtonPin) == HIGH) {
          break;
        } else if (digitalRead(upButtonPin) == HIGH) {
          break;
        }

        //Check if 3s has elapsed since we last checked for movement
        if ((millis()-movementStartTimer)>3000){
          
          //Check if any movement has occured in this time
          if (currentPosition == movementStartPosition) {
            //No movement, activate red led and stop movement
            pixels.setPixelColor(0,255,0,0);
            pixels.show();
            errorFlag = true;
            Blynk.setProperty(V13, "color","#ff3232"); //Set Blynk LED to red
            //Reset positions and timer
            break;
          } else {
            //Reset timer and position
            movementStartPosition = currentPosition;
            movementStartTimer = millis();
          }
        }
      }

    //Stop the mmotors
    delay(0);
    digitalWrite(E1,LOW);
    digitalWrite(M1,LOW);

    if (errorFlag == false) {
      //Turn off LED
      pixels.setPixelColor(0,0,0,0); 
      pixels.show();

    
      blindState = 1; //Set blind state to open
    
      //Update memory
      EEPROM.begin(512);
      EEPROM.put(addrcurrPosition,currentPosition);
      EEPROM.put(addrblindState,blindState);
      EEPROM.commit();
      flash_LED(1,String("Blue"));

      Blynk.setProperty(V13, "color","#4ca64c"); //Set Blynk LED to green
      
    } else {
      EEPROM.begin(512);
      EEPROM.put(addrcurrPosition,currentPosition);
      EEPROM.commit();
    }

        
    //Update app with new position
    if(useBlynk == true){
      Blynk.virtualWrite(V3, openPosition);
    }

      
    } else {
      //Blinds already open
      //Flash LED to acknowledge this
      Serial.println("Blinds already open!");
      flash_LED(2,String("Yellow"));
    }
}

void closeBlinds() {
 
    //Get the current blind position
    currentPosition = myEnc.read();

    //Start the timer and recod current position as start
    movementStartTimer = millis();
    movementStartPosition = currentPosition;
    errorFlag = false;
    

    //First check if blinds are not already closed or beyond the closed position
    if (currentPosition > closedPosition){
      //Begin closing
      int potValue=700;
      int mappedVal = map(potValue-512,0,512,0,255);

      //Turn on the motor
      analogWrite(M1, mappedVal);   //PWM Speed Control
      digitalWrite(M1,HIGH);
      analogWrite(E1, mappedVal);   //PWM Speed Control

      //Turn on LED while we move
      pixels.setPixelColor(0,255,165,0);
      pixels.show();

      Blynk.setProperty(V13, "color","#ffff00"); //Set Blynk LED to orange
    
      while(currentPosition > closedPosition) {
        //Write as we go
        travelPercent = abs(((float(currentPosition) - float(openPosition))/float(travelLength))*100);

        if(useBlynk == true){
          Blynk.virtualWrite(V8, travelPercent);
          Blynk.virtualWrite(V9, travelPercent);
        }
        currentPosition = myEnc.read();
        delay(0);
        //Check if button pressed if so stop
        if (digitalRead(downButtonPin) == HIGH) {
          break;
        } else if (digitalRead(upButtonPin) == HIGH) {
          break;
        }  

        //Check if 3s has elapsed since we last checked for movement
        if ((millis()-movementStartTimer)>3000){
          
          //Check if any movement has occured in this time
          if (currentPosition == movementStartPosition) {
            //No movement, activate red led and stop movement
            pixels.setPixelColor(0,255,0,0);
            pixels.show();
            Blynk.setProperty(V13, "color","#ff3232"); //Set Blynk LED to red
            errorFlag = true;
            //Reset positions and timer
            break;
          } else {
            //Reset timer and position
            movementStartPosition = currentPosition;
            movementStartTimer = millis();
          }
        }
         
      }

    //Stop the motors
    delay(0);
    digitalWrite(E1,LOW);
    digitalWrite(M1,LOW);

    if (errorFlag == false) {
      //Turn off LED
      pixels.setPixelColor(0,0,0,0); 
      pixels.show();

    
      blindState = 0; //Set blind state to closed
    
      //Update memory
      EEPROM.begin(512);
      EEPROM.put(addrcurrPosition,currentPosition);
      EEPROM.put(addrblindState,blindState);
      EEPROM.commit();
      flash_LED(1,String("Blue"));

      Blynk.setProperty(V13, "color","#4ca64c"); //Set Blynk LED to green
      
    } else {
      EEPROM.begin(512);
      EEPROM.put(addrcurrPosition,currentPosition);
      EEPROM.commit();
    }
        
    //Update app with new position
    if(useBlynk == true){
    Blynk.virtualWrite(V3, closedPosition);
    }

      
    } else {
      //Blinds already closed
      //Flash LED to acknowledge this
      Serial.println("Blinds already closed!");
      flash_LED(2,String("Yellow"));
    }
}

void lightOn() {
    openBlinds();
}

void lightOff() {
    closeBlinds();
}

void resetAll() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}