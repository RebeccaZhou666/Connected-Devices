#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Arduino_JSON.h>


const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels
const int OLED_RESET = 0; // Reset pin for display (0 or -1 if no reset pin)


// colors for a monochrome display:
const int foregroundColor = 0x01;  // white
const int backgroundColor = 0x00;  // black

// initialize the display library instance:
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

int buttonPin = 2;
int buttonPinUp = 3;

int scanInterval = 1000;
int restInterval = 15000;
bool isRest = false;
unsigned long currMillis = 0;
unsigned long prevMillis = 0;

JSONVar BLEScanInfo;
int deviceLength = 0;

bool pCursor = 0; // where cursor is
int pDevice = 0; // where the device is being pointed
int scrollInt = 20;
unsigned long prevScrollMillis = 0;
int hCursor = 0;

//button state
bool buttonState = false;
bool prevButtonState = false;

bool buttonStateUp = false;
bool prevButtonStateUp = false;



void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT_PULLUP); //button
  pinMode(buttonPinUp, INPUT_PULLUP); //button

  Serial.begin(9600);
  while (!Serial);

  // start the display:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  Serial.println("BLE Central Scan");

  BLE.scan();

  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(36, 30);
  display.println("START");

  display.display(); // actually display all of the above

  display.setTextWrap(false);

}

void loop() {
  // put your main code here, to run repeatedly:
  currMillis = millis();
  buttonState = digitalRead(buttonPin);
  buttonStateUp = digitalRead(buttonPinUp);

  // status check;
  if (currMillis - prevMillis >= restInterval && isRest) {
    // start scanning
    isRest = false;
    BLE.scan();
    prevMillis = currMillis;

    deviceLength = 0;
    
  } else if (currMillis - prevMillis >= scanInterval && isRest) {
      // wait for connection
      
      if(buttonIsPressed()){
        // change cursor and device vars
        if(pCursor < 1){
          pCursor++;
        }else if(pDevice +1 < deviceLength){
          pDevice ++;
        }
        hCursor = 0;
      }

      if(buttonUpIsPressed()){
        // change cursor and device vars
        if(pCursor > 0){
          pCursor = false;
        }else if(pDevice -1 >= 0){
          pDevice --;
        }
        hCursor = 0;
      }
      
  } else if (currMillis - prevMillis >= scanInterval && !isRest) {
    // stop scanning, wait for connection
    isRest = true;
    BLE.stopScan();
    prevMillis = currMillis;
    prevScrollMillis = currMillis;

    for (int i = 0; i < deviceLength; i++) {
      Serial.println(BLEScanInfo[i]);
    }

    Serial.println();
    Serial.println();

    pCursor = 0; // reset where cursor is
    pDevice = 0; // reset where the device is being pointed
    hCursor = 0;
  }

  prevButtonState = buttonState;
  prevButtonStateUp = buttonStateUp;


  // display
  if (isRest) {
    // wait for connection, display
    int scrollD = pDevice + pCursor;
    int endD = pDevice + 1;

    display.clearDisplay();
    
    if (currMillis - prevScrollMillis >= scrollInt) {
      display.fillRoundRect(0, 16 + pCursor * 24, display.width(), 24,
                            4, SSD1306_INVERSE);
      display.setCursor(hCursor, 20 + pCursor * 24);
      display.setTextColor(SSD1306_BLACK);
      display.print("ADDRESS:");
      display.print(BLEScanInfo[scrollD]["Address"]);
      display.print(" RSSI:");
      display.print(BLEScanInfo[scrollD]["RSSI"]);
      display.print(" LOCALNAME:");
      display.println(BLEScanInfo[scrollD]["LocalName"]);
      hCursor -= 2;
      hCursor = hCursor % 800;

      display.setTextColor(SSD1306_WHITE);
      
      int drawNextD = 0;
      if(pCursor == 0){
        drawNextD = scrollD + 1;
      }else{
        drawNextD = scrollD - 1;
      }
      display.setCursor(0, 20 + (!pCursor) * 24);

      display.print("ADDRESS:");
        display.print(BLEScanInfo[drawNextD]["Address"]);
        display.print("RSSI:");
        display.print(BLEScanInfo[drawNextD]["RSSI"]);
        display.print("LOCALNAME:");
        display.println(BLEScanInfo[drawNextD]["LocalName"]);

       display.setCursor(0,0);
       int restTime = map(currMillis - prevMillis, 0, restInterval, 0, display.width());
       display.fillRect(0,0,restTime, 2, SSD1306_WHITE);
        
        display.display();
    }
    // start display of device: start = pd - c
    // end display of device: end = start + 1;

    //displat start

   
    // update;
    // c move: c -- / c++
    // update pd: pd = pd + c; 


  } else {
    // scan and store

    BLEDevice peripheral = BLE.available();

    if (peripheral) {
      String tempAddress = peripheral.address();
      bool isExist = false;
      for (int i = 0; i < deviceLength; i++) {
        if (BLEScanInfo[i]["Address"] == tempAddress) {
          isExist = true;
          break;
        }
      }

      if (!isExist) {
        //add into JSON array

        //JSONVar ;
        BLEScanInfo[deviceLength]["Address"] = peripheral.address();
        BLEScanInfo[deviceLength]["RSSI"] = peripheral.rssi();

        if (peripheral.hasLocalName()) {
          BLEScanInfo[deviceLength]["LocalName"] = peripheral.localName();
        } else {
          BLEScanInfo[deviceLength]["LocalName"] = "";

          //          BLEScanInfo[BLEScanInfo.length() +1](toArray);
        }

        deviceLength ++;
      }





      //    // check if a peripheral has been discovered
      //    display.clearDisplay();
      //    display.setTextSize(1);
      //
      //    BLEDevice peripheral = BLE.available();
      //
      //    if (peripheral) {
      //
      //      display.println(peripheral.address());
      //      //    Serial.println("Discover a peripheral");
      //      //
      //      //    Serial.print("Address:");
      //      //    Serial.println(peripheral.address());
      //
      //      // print the local name, if present
      //      if (peripheral.hasLocalName()) {
      //        display.print("Local Name: ");
      //        display.println(peripheral.localName());
      //      }
      //
      //      // print the advertised service UUIDs, if present
      //      //    if (peripheral.hasAdvertisedServiceUuid()) {
      //      //      Serial.print("Service UUIDs: ");
      //      //      for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
      //      //        Serial.print(peripheral.advertisedServiceUuid(i));
      //      //        Serial.print(" ");
      //      //      }
      //      //      Serial.println();
      //      //    }
      //
      //      // print the RSSI
      //      display.print("RSSI: ");
      //      display.println(peripheral.rssi());
      //
      //      display.println();
      //
      //      display.display();
    }
  }

}

bool buttonIsPressed(){
  // Check if user presses start button
  if (buttonState != prevButtonState && !buttonState) {
    Serial.print("buttonPressed");
    Serial.println(pCursor, pDevice);
    return true;
  }
  return false;
}

bool buttonUpIsPressed(){
  // Check if user presses start button
  if (buttonStateUp != prevButtonStateUp && !buttonStateUp) {
    Serial.print("buttonPressedUp");
    Serial.println(pCursor, pDevice);
    return true;
  }
  return false;
}
