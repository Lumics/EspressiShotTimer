#include <Arduino.h>
#include <Wire.h>

// Display
// This Gadget has a 1.14inch TFT display with a width of 135 and a height of 240
// Go to TTGO T-Display's Github Repository
// Download the code as zip, extract it and copy the Folder TFT_eSPI
//  => https://github.com/Xinyuan-LilyGO/TTGO-T-Display/archive/master.zip
// to your Arduino library path
#include "TFT_eSPI.h" // Graphics and font library for ST7735 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI(135,240);  // Invoke library, pins defined in User_Setup.h

#define REED_SENSOR_PIN 26
int proximity;

// display functions
void display_voltage(int voltage){
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Magnetism detected:", 0, 10, 4);
  tft.setTextSize(3);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  if(voltage == 1){
    tft.drawString("NO", 70, 50, 4);
  } else{
    tft.drawString("YES", 70, 50, 4);
  }
  
  
  Serial.println("Display updated");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();                           //init serial connection
  
  // Setup Display
  tft.init();
  tft.setRotation(1); 

  pinMode(REED_SENSOR_PIN, INPUT_PULLUP);
  
}

void loop() {
  delay(100);
  proximity = digitalRead(REED_SENSOR_PIN);
  display_voltage(proximity);
  Serial.print("Voltage is: ");
  Serial.println(proximity);
}
