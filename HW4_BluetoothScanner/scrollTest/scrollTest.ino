#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels
const int OLED_RESET = 0; // Reset pin for display (0 or -1 if no reset pin)


// colors for a monochrome display:
const int foregroundColor = 0x01;  // white
const int backgroundColor = 0x00;  // black

// initialize the display library instance:
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);


void setup() {
  // put your setup code here, to run once:
  // start the display:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(36, 30);
  display.println("START");

  display.display(); // actually display all of the above

  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // disable text wrap
  display.setTextWrap(false);
  
  testscrolltext();    // Draw scrolling text
}

void loop() {
  // put your main code here, to run repeatedly:

}

void testscrolltext(void) {
  display.clearDisplay();

  
  for(int i = 0; i < 800; i+= 2){
    display.clearDisplay();
    display.fillRoundRect(0,16, display.width(), 24,
                        4, SSD1306_INVERSE);
    display.setCursor(0-i, 20);
    display.setTextColor(SSD1306_BLACK);
    display.print(F("ADDRESS: 2d:5d:8e:05 "));
    display.print(F("RSSI:-41 "));
    display.println(F("LocalName: Hello"));
    display.display();      // Show initial text
    delay(20);
  }
  
// Scroll in various directions, pausing in-between:
//  display.startscrollright(0x00, 0x07);
//  delay(8000);
//  display.stopscroll();
//  delay(5000);
//  display.startscrollleft(0x00, 0x07);
//  delay(8000);
//  display.stopscroll();
//  delay(5000);
//  display.startscrolldiagright(0x00, 0x07);
//  delay(4000);
//  display.startscrolldiagleft(0x00, 0x07);
//  delay(4000);
//  display.stopscroll();
//  delay(5000);
}
