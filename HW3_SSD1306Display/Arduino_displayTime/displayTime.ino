/*
  Time displat for SSD1306 Display
   Displays a time and a potentiometer parameter on a SSD1306 128x64 pixel display
   Uses Adafruit EPD library: http://librarymanager/All#Adafruit_SSD1306
   Circuit:
   - 128x64 SSD1306 OLED display connected to I2C pins. 
   
  created 21 Feb 2021
  by Rebecca Jingyi Zhou
*/

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <RTCZero.h>

const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels
const int OLED_RESET = 0; // Reset pin for display (0 or -1 if no reset pin)

// colors for a monochrome display:
const int foregroundColor = 0x01;  // white
const int backgroundColor = 0x00;  // black

// initialize the display library instance:
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// initialize the RTC time monitor:
RTCZero rtc;

int buttonPin = 2;

#define LOGO_HEIGHT   24
#define LOGO_WIDTH    24

// 'sound icon', 24x24px:
// check out this blpg to get bmp code: https://create.arduino.cc/projecthub/najad/interfacing-and-displaying-images-on-oled-59344a
const unsigned char logo_bmp [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x7c, 0x00, 0x00, 0xfc, 0x18, 0x01,
  0xfc, 0x1c, 0x03, 0xfc, 0x0e, 0x7f, 0xfc, 0x46, 0xff, 0xfc, 0x66, 0xff, 0xfc, 0x73, 0xff, 0xfc,
  0x33, 0xff, 0xfc, 0x33, 0xff, 0xfc, 0x33, 0xff, 0xfc, 0x33, 0xff, 0xfc, 0x73, 0xff, 0xfc, 0x62,
  0x7f, 0xfc, 0x46, 0x07, 0xfc, 0x0e, 0x01, 0xfc, 0x1c, 0x00, 0xfc, 0x18, 0x00, 0x7c, 0x00, 0x00,
  0x3c, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00
};

// time variables
int prevSec, currentSec;
int prevMin, currentMin;
int prevHour, currentHour;
bool isChanged = false;

// sound/analog input variables
int intensity = 0;
int lastIntensity = 0;
int threshold = 10;

bool buttonState = false;
bool prevButtonState = false;

bool isBegin = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT_PULLUP); //button

  // initialize serial:
  Serial.begin(9600);

  Serial.setTimeout(10);
  // wait 3 sec. for serial monitor to open:
  if (!Serial) delay(3000);

  // start the display:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // initialize RTC Time:
  rtc.begin();
  rtc.setTime(0, 0, 0);

  // text display tests
  // Clear the buffer
  display.clearDisplay();

  // Draw onload screen
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Press Button to Start");

  display.drawRoundRect(10, 20, display.width() - 20, 36,
                        8, SSD1306_WHITE);

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(36, 30);
  display.println("START");

  display.display(); // actually display all of the above

  // init time variables
  prevSec = 0;
  prevMin = 0;
  prevHour = 0;
}

void loop() {
  // put your main code here, to run repeatedly:

  // read all inputs
  buttonState = digitalRead(buttonPin);
  intensity = analogRead(A0);

  currentSec = rtc.getSeconds();
  currentMin = rtc.getMinutes();
  currentHour = rtc.getHours();

  // Check if user presses start button
  if (buttonState != prevButtonState && !buttonState && !isBegin) {
    isBegin = true;
    // if start, show start screen, reset rtc time
    startScreen();
  }

  prevButtonState = buttonState;

  // if start, enter main screen
  if (isBegin) {
    // Check time
    if (prevSec != currentSec) {
      prevSec = currentSec;
      isChanged = true;
    }

    if (prevMin != currentMin) {
      prevMin = currentMin;
      isChanged = true;
    }

    if (prevHour != currentHour) {
      prevHour = currentHour;
      isChanged = true;
    }

    // check sound
    if (abs(intensity - lastIntensity) > threshold) {
      lastIntensity = intensity;
      isChanged = true;
    }

    // if any of these changed, refresh display
    if (isChanged) {
      Serial.println("change");
      display.clearDisplay();

      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(1, 5);
      display.print("TIME");
      display.setTextSize(2);
      display.setCursor(32, 0);
      // force two digit display for hour, minute, and second.
      twoDigitPrint(prevHour);
      display.print(":");
      twoDigitPrint(prevMin);
      display.print(":");
      twoDigitPrint(prevSec);
      display.println();

      // draw sound icon
      display.drawBitmap(8, (display.height() - LOGO_HEIGHT) / 2 + 4,
                         logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);

      // draw sound input UI
      int width = map(lastIntensity, 0, 1023, 0, (display.width() - 48));
      int percentage = map(lastIntensity, 0, 1023, 0, 100);

      display.drawRect(40, 26, display.width() - 44, display.height() - 36, SSD1306_WHITE);
      display.fillRect(42, 28, width, display.height() - 40, SSD1306_WHITE);

      // display sound input as numbers
      display.setCursor(10, (display.height() + LOGO_HEIGHT) / 2 + 6);
      display.setTextSize(1);
      display.println(percentage);

      display.display();
      isChanged = false;
    }
  }

}

void startScreen() {
  display.drawRoundRect(10, 20, display.width() - 20, 36,
                        8, SSD1306_WHITE);
  display.fillRoundRect(12, 22, display.width() - 24, 32,
                        8, SSD1306_INVERSE);

  display.setTextSize(2);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(36, 30);
  display.println("START");

  display.display(); // actually display all of the above
  delay(200);

  rtc.setTime(0, 0, 0);

}

void twoDigitPrint(int n) {
  if (n < 10) {
    display.print(0);
  }

  display.print(n);
}
