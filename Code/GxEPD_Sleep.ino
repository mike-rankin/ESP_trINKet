// This Sensor Test Code is for the WaveShare E-INK 2.13" hardware using the IL3820 controller
//
// Originally Created by Jean-Marc Zingg at: https://github.com/ZinggJM/GxEPD
//

//Install instructions, Code and PCB Design files at: https://github.com/mike-rankin/ESP_trINKet

#include <TimeLib.h>
#include <Wire.h>      
#include "ClosedCube_HDC1080.h"
#include <GxEPD.h>
#include <GxGDEW0213Z16/GxGDEW0213Z16.cpp>  // 2.13" b/w/r
#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  15        /* Time ESP32 will go to sleep (in seconds) */  //60 = 1min , 900 = 15min, 3600000 = 1H

ClosedCube_HDC1080 hdc1080;

GxIO_Class io(SPI, SS, 17, 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, 16, 4); // arbitrary selection of (16), 4

void setup()
{
  Serial.begin(115200);
  hdc1080.begin(0x40);
  Serial.println();
  Serial.println("setup");
  display.init();
  Serial.println("setup done");
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop()
{
  char temp[]= "****";
  char humid[]= "****";
  
  float temperature = hdc1080.readTemperature();
  dtostrf(temperature,4,1,temp);

  float humidity = hdc1080.readHumidity(); 
  dtostrf(humidity,4,1,humid);
  
  const char* name = "FreeMonoBold9pt7b";
  const GFXfont* f = &FreeMonoBold9pt7b;

  const char* namex = "FreeMonoBold12pt7b";
  const GFXfont* g = &FreeMonoBold12pt7b;
  
  display.setRotation(1);      //Rotation
  showBitmapExample();
  delay(1000);

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_RED);
  display.setFont(g);
  display.setCursor(15,15);      //over,down
  display.println("ESP32 trINKet"); 

  display.setTextColor(GxEPD_BLACK);
  display.setCursor(10, 40);
  display.println("WaveShare 2.13");  
  display.println();
  display.setCursor(40, 70); 
  display.println("SKU: 13368");  

  display.setTextColor(GxEPD_RED);
  display.setFont(f);
  display.setCursor(20,100);      
  display.print(temp);
  display.print("C"); 
 
  display.setCursor(120,100);     
  display.print(humid);
  display.println("%"); 
  display.update();
 
  esp_deep_sleep_start();
}

void showBitmapExample()
{
  display.drawPicture(BitmapWaveshare_black, BitmapWaveshare_red, sizeof(BitmapWaveshare_black), sizeof(BitmapWaveshare_red));
  delay(5000);
  display.update();
}

