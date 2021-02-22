#include <Arduino_JSON.h>

int buttonPin = 2;

bool lastButtonState = HIGH;
bool buttonState;
bool flag = false;

int intensity = 0;
int lastIntensity = 0;
int threshold = 1;

bool isChanged = false;

JSONVar toApp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP); //button

  toApp["button"] = flag;
  toApp["intensity"] = 0;

}

void loop() {
  // put your main code here, to run repeatedly:
  
  buttonState = digitalRead(buttonPin);
  intensity = analogRead(A0);
  
  if(buttonState != lastButtonState && !buttonState){
    flag = !flag;
    isChanged = true;
    toApp["button"] = flag;
  }
  
  lastButtonState = buttonState;

  if(abs(intensity - lastIntensity) > threshold){
    toApp["intensity"] = intensity;
    isChanged = true;
  }

  lastIntensity = intensity;

  if(isChanged){
    Serial.println(toApp);
    isChanged = false;
//    flag = false;
  }

}
