 //This is a test sketch for the WaveShare E-INK 2.13" hardware using the IL3820 controller

//Setup
//Arduino File/Preferences/Additional Boards Manager URLs: http://arduino.esp8266.com/versions/2.3.0/package_esp8266com_index.json

//In your Documents/Arduino/Libraries/E-paper folder modify the epdif.h file to
//#define RST_PIN         27 
//#define DC_PIN          5   
//#define CS_PIN          4
//#define BUSY_PIN        17

//Tools/Board/ESP32 Dev Module
//The link for the ESP32 Module install instructions for Windows or a Mac is here: https://github.com/espressif/arduino-esp32/
//You may have to enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into the Arduino IDE under File, Preferences, Additional Board Manager URLs.

//Display Library: https://www.waveshare.com/wiki/File:2.13inch-e-paper-hat-b-code.7z

//Original License Info from Waveshare
/**
 *  @filename   :   epd2in9b-demo.ino
 *  @brief      :   2.9inch e-paper display (B) demo
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     July 17 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */



#include <WiFi.h>
#include <SPI.h>
#include "epd2in13b.h"    //Only for the 2.13" Screen!
#include "imagedata.h"
#include "epdpaint.h"
#include <Wire.h> 
#include "ClosedCube_HDC1080.h"

#define COLORED     0
#define UNCOLORED   1


char ssid[] = "***";         // your network SSID (name)
char pass[] = "***";  // your network password
int status = WL_IDLE_STATUS;  // the Wifi radio's status

ClosedCube_HDC1080 hdc1080;
Epd epd;

void setup() 
{
  Serial.begin(9600);
  hdc1080.begin(0x40);
  
  if (epd.Init() != 0) 
  {
   Serial.print("e-Paper init failed");
   return;
  }
  Serial.print("e-Paper init success\n");

  while (status != WL_CONNECTED) 
  {
   Serial.print("Attempting to connect to WPA SSID: ");
   Serial.println(ssid);
   status = WiFi.begin(ssid, pass);  //Connect to WPA/WPA2 network:
   delay(5000);                      //has to be 5 seconds
 }
   Serial.print("You're connected to the network");
}

void loop() 
{
  char temp[]= "****";
  char humid[]= "****";
  char IP[]= "***.***.***.***";

  epd.ClearFrame();
  
  unsigned char image[3072];    
  Paint paint(image,248,64);    
  
  paint.SetWidth(104);  
  paint.SetHeight(212); 
   
  paint.SetRotate(1);   
 
  paint.Clear(UNCOLORED);

  paint.DrawStringAt(5, 5, "trINKet Test", &Font16, COLORED);
  paint.DrawStringAt(5, 20, "WaveShare", &Font16, COLORED);

  epd.DisplayFrame(IMAGE_BLACK, IMAGE_RED);  //Image 
  
  //First Line
  IPAddress ip = WiFi.localIP();
  ip.toString().toCharArray(IP, 16); 
  paint.DrawStringAt(5, 38, IP, &Font16, COLORED); 
                                        
  //Second Line
  float temperature = hdc1080.readTemperature();
  dtostrf(temperature,4,1,temp);                          
  paint.DrawStringAt(5, 58, temp, &Font16, COLORED); //over,down 
  paint.DrawStringAt(55, 58, "C", &Font16, COLORED); 
                      
  //Third Line
  float humidity = hdc1080.readHumidity();
  dtostrf(humidity,4,1,humid);             
  paint.DrawStringAt(5, 78, humid, &Font16, COLORED);
  paint.DrawStringAt(55, 78, "%", &Font16, COLORED); 

  //Fourth Line
  paint.DrawStringAt(5, 93, "Thanks to @pdp7", &Font12, COLORED); 
                        
  epd.SetPartialWindowBlack(paint.GetImage(),0, 0, paint.GetWidth(), paint.GetHeight());  //down,over 0,0

  epd.DisplayFrame();
  delay(30000);
  
  //epd.Sleep();  //Sleep forever...
}

void printWifiData() 
{
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);
}

void printCurrentNet() 
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
}

