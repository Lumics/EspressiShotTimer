#include <Arduino.h>
#include <Wire.h>


// Display
// This Gadget has a 1.14inch TFT display with a width of 135 and a height of 240
// Go to TTGO T-Display's Github Repository
//  => https://github.com/Xinyuan-LilyGO/TTGO-T-Display/archive/master.zip
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI(135,240);  // Invoke library, pins defined in User_Setup.h

#define REED_SENSOR_PIN 21


bool displayOn = true;
int timerCount = 0;
int prevTimerCount = 0;
bool timer_started = false;
long timerStartMillis = 0;
long timerStopMillis = 0;
long timerDisplayOffMillis = 0;
long serialUpdateMillis = 0;
String old_time = "00";

// display functions
void display_timer(String time){
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  if(old_time != time){
    tft.fillScreen(TFT_BLACK);
    tft.drawString(time, 50, 20, 7);
    old_time = time;
  }
  if(time == "00"){
    tft.fillScreen(TFT_BLACK);
  }
  Serial.println("Display updated");
}

bool sample_reed_sensor() {
  bool detected_change = false;
  // Sample read sensor 500 times to be sure that the pump is on
  for (int i = 0; i < 500; i++){
    detected_change = !digitalRead(REED_SENSOR_PIN);
    delay(1);
    if(detected_change){
      return true;
    }
  }
  return false;
}

void detectChanges() {
  bool reed_sensor = sample_reed_sensor();
  if (!timer_started && reed_sensor) {
    timerStartMillis = millis();
    timer_started = true;
    // Start Display Backlight
    digitalWrite(4, HIGH);
    Serial.println("Start pump");
  }
  if (timer_started && !reed_sensor) {
    timer_started = false;
    timerStopMillis = 0;
    timerDisplayOffMillis = millis();
    Serial.println("Stop pump");
  } else {
    timerStopMillis = 0;
  }
  // When shot got pulled, turn off the display after 60s
  if (!timer_started && timerDisplayOffMillis >= 0 && ((millis() - timerDisplayOffMillis) > 60000)) {
    timerDisplayOffMillis = 0;
    timerCount = 0;
    prevTimerCount = 0;
    // Turn display off
    tft.fillScreen(TFT_BLACK);
    digitalWrite(4, LOW);
    Serial.println("Sleep");
  }
}

String getTimer() {
  char outMin[2];
  if (timer_started) {
    timerCount = (millis() - timerStartMillis ) / 1000;
    // if timer is over 15s, assume that this is an esspresso shot which gets pulled
    // Then save the value that is displayed after shot pulling is finished until next time
    // the 15s shot pulling happens
    if (timerCount > 15) {
      prevTimerCount = timerCount;
    }
  } else {
    timerCount = prevTimerCount;
  }
  if (timerCount > 99) {
    return "99";
  }
  sprintf( outMin, "%02u", timerCount);
  return outMin;
}

void setup() {
  //init serial connection
  Serial.begin(115200);
  Serial.println();     
  
  // Setup Display
  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);
  // Turn display light off
  digitalWrite(4, LOW);

  // Setup Pin
  pinMode(REED_SENSOR_PIN, INPUT_PULLUP);
  
}


void loop() {
  detectChanges();
  display_timer(getTimer());
}
