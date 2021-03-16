#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Arduino_JSON.h>
#include <Encoder.h>
// for wifi connection
#include <SPI.h>
//#include <WiFi101.h>        // use this for MKR1000 boards
//#include <ESP8266WiFi.h>  // use this instead of WiFi101 for ESP8266 modules
#include <WiFiNINA.h>       // use this for MKR1010 and Nano 33 IoT boards
#include "arduino_secrets.h"
#include <ArduinoHttpClient.h>


const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels
const int OLED_RESET = 0; // Reset pin for display (0 or -1 if no reset pin)

// colors for a monochrome display:
const int foregroundColor = 0x01;  // white
const int backgroundColor = 0x00;  // black

// initialize the display library instance:
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// 'reload', 16x16px
#define RELOAD_HEIGHT   16
#define RELOAD_WIDTH    16

const unsigned char myBitmap [] PROGMEM = {
  0xff, 0xff, 0xff, 0x7f, 0xff, 0x1f, 0xf8, 0x0f, 0xf0, 0x07, 0xe0, 0x1f, 0xc3, 0x3f, 0xc7, 0xff, 
  0xc7, 0xff, 0xc7, 0xe3, 0xc7, 0xe3, 0xc3, 0xc3, 0xe0, 0x07, 0xf0, 0x0f, 0xfc, 0x3f, 0xff, 0xff
};


// wifi setup
WiFiClient netSocket;               // network socket to server
const char server[] = "olivine-flawless-sunscreen.glitch.me";  // server name
String route = "/getDatafromArduino";              // API route


Encoder myEncoder(10, 11);
volatile int counter = 0; // the var can only be changed by one func at a time.
//bouncing effect that leads to increase more than we think
// one way is to add a debouncingDelay.
int debouncingDelay = 3;
int buttonState = 0;
long oldP;
int myMove = 0;

JSONVar foodList;
int foodLength = 0;

bool pCursor = 0; // where cursor is
int pDevice = 0; // where the device is being pointed
int scrollInt = 20;
unsigned long prevScrollMillis = 0;
int hCursor = 0;


int scanInterval = 500;
int restInterval = 10000;
bool isRest = false;
unsigned long currMillis = 0;
unsigned long prevMillis = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(A1, INPUT_PULLUP);
  attachInterrupt(A1, buttonChange, CHANGE);

  Serial.begin(9600);

//  while (!Serial);

  // ----------- wifi setup --------------
  // while you're not connected to a WiFi AP,
  while ( WiFi.status() != WL_CONNECTED) {
//    Serial.print("Attempting to connect to Network named: ");
//    Serial.println(SECRET_SSID);           // print the network name (SSID)
    WiFi.begin(SECRET_SSID, SECRET_PASS);         // try to connect
    delay(50);
  }

  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // ----------- END wifi setup --------------


  // ----------- display setup -----------
  // start the display:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(36, 30);
  display.println("START");

  display.display(); // actually display all of the above

  display.setTextWrap(false);
  // ----------- END display setup -----------

}

void loop() {
  // put your main code here, to run repeatedly:
  currMillis = millis();
  int buttonState = digitalRead(A1);


  // status check;
  if (currMillis - prevMillis >= restInterval && isRest) {
    // start updating from http
    isRest = false;

    // get updates from central;
    prevMillis = currMillis;

    foodLength = 0;

  } else if (currMillis - prevMillis >= scanInterval && isRest) {
    // wait for connection
    int timeChange = readEncoder();
    if (timeChange >= 1) {
      // change cursor and device vars
      if (pDevice + 1 < foodLength) {
        pDevice ++;
      }
    }

    if (timeChange < 0) {
      // change cursor and device vars
      if (pDevice - 1  >= 0) {
        pDevice --;
      }
    }

  } else if (currMillis - prevMillis >= scanInterval && !isRest) {
    // stop scanning, wait for connection
    isRest = true;
    //    BLE.stopScan();
    prevMillis = currMillis;
    prevScrollMillis = currMillis;

    foodLength = foodList.length();
    Serial.println(foodLength);

    for (int i = 0; i < foodLength; i++) {
      Serial.println(foodList[i]);
    }

    Serial.println();
    Serial.println();

  }


  if (isRest) {
    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(6, 20);
    display.setTextColor(SSD1306_WHITE);
    //    display.print(foodList[pDevice]["Name"]);

    String str = JSON.stringify(foodList[pDevice]["Category"]);
    str = str.substring(1, str.length() - 1);
    display.print(str);

    display.setCursor(6, 20 + 21);
    display.print(foodList[pDevice]["TimeLeft"]);
    display.setTextSize(1);
    int timeleft = int(foodList[pDevice]["TimeLeft"]);
    if ( timeleft == 1) {
      display.print(" Day Left");
    } else {
      display.print(" Days Left");
    }


    if (timeleft  <=  2 && timeleft >= 0) {
      display.drawRoundRect(2, 16, display.width() - 12 , display.height() - 20, 4, SSD1306_WHITE);
      display.setCursor(26, 5);
      display.setTextSize(1);
      display.print("EAT ME NOW");
    }else if(timeleft < 0){
      display.drawRoundRect(2, 16, display.width() - 12 , display.height() - 20, 4, SSD1306_WHITE);
      display.setCursor(18, 5);
      display.setTextSize(1);
      display.print("THROW ME AWAY");
    }


    // scroll bar vertical
    display.setCursor(display.width() - 6, 20);
    display.drawRoundRect(display.width() - 6, 16, 4, display.height() - 20, 2, SSD1306_WHITE);
    int perLength = (display.height() - 20) / foodLength;
    display.fillRoundRect(display.width() - 6, 16 + perLength * pDevice, 4, perLength, 2, SSD1306_WHITE);

    display.display();
  } else {
    // request from http server
    Serial.println("making request");
    HttpClient http(netSocket, server, 80);      // make an HTTP client
    http.get(route);  // make a GET request

    while (http.connected()) {       // while connected to the server,
      if (http.available()) {        // if there is a response from the server,
        String result = http.responseBody();  // read it
//        Serial.println(result);               // and print it

//        result = result.substring(1, 60);//result.length());
//        Serial.println(result);               // and print it
//        
        foodList = JSON.parse(result);
        Serial.println(foodList);               // and print it
      }
    }
    //  // when there's nothing left to the response,
    http.stop();                     // close the request
  }



}





int readEncoder() {
  int reading = myEncoder.read();
  int steps = 0;
  if (abs(reading - oldP) > 4) {
    steps = (reading - oldP) / abs(reading - oldP);
    Serial.println("step");
    oldP =  reading;
  }
  return steps;
}

// doesn't have to have if you don't need to click button
void buttonChange() {
  delay(debouncingDelay);
  if (buttonState == LOW) {
    counter++;
  }

}
