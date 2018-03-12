//This Sensor Test Code is for the WaveShare E-INK 2.13" hardware using the IL3820 controller
//Library by Jean-Marc Zingg at: https://github.com/ZinggJM/GxEPD
//Original Weather Code at: https://github.com/G6EJD/ESP32-8266-ePaper-Weather-Display-Examples 
//Install instructions, Code and PCB Design files at: https://github.com/mike-rankin/ESP_trINKet
//TimeLib.h at https://github.com/PaulStoffregen/Time

#include <WiFi.h>
#include <ArduinoJson.h>     // https://github.com/bblanchon/ArduinoJson
#include <WiFiClient.h>

#include "time.h"
#include <Wire.h>      
#include <GxEPD.h>
#include <GxGDEW0213Z16/GxGDEW0213Z16.cpp>  // 2.13" b/w/r
#include GxEPD_BitmapExamples

#include <Fonts/Org_01.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>

#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  900        /* Time ESP32 will go to sleep (in seconds) */  //60 = 1min , 900 = 15min, 3600000 = 1H

//################# LIBRARIES ##########################
String version = "1.0";       // Version of this program
//################ VARIABLES ###########################

// Define each of the *icons for display
const unsigned char sunny[] PROGMEM = { // 56x48
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X80,0X00,0X00,0X00,0X00,0X00,
0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X02,0X00,0X00,0X00,0X00,0X01,0XE0,
0X07,0X00,0X00,0X00,0X00,0X01,0XE0,0X0F,0X00,0X00,0X00,0X00,0X03,0XF0,0X1F,0X00,
0X00,0X00,0X00,0X03,0XF0,0X3E,0X00,0X00,0X00,0X1C,0X03,0XF0,0X7E,0X00,0X00,0X00,
0X1E,0X00,0X00,0X7E,0X00,0X00,0X00,0X0F,0X80,0X00,0X3E,0X00,0X00,0X00,0X0F,0XC0,
0XFF,0X0C,0X00,0X00,0X00,0X07,0XC7,0XFF,0XC0,0X00,0X00,0X00,0X03,0XCF,0XFF,0XF0,
0X00,0X00,0X00,0X03,0X9F,0XFF,0XF8,0X00,0X00,0X00,0X03,0X3F,0XC3,0XFC,0X7F,0X00,
0X00,0X00,0X7F,0X00,0XFC,0X7F,0X00,0X00,0X00,0X7C,0X00,0X7E,0X7E,0X00,0X00,0X00,
0XF8,0X00,0X3E,0X7C,0X00,0X00,0X00,0XF8,0X00,0X1F,0X30,0X00,0X00,0X04,0XF8,0X00,
0X1F,0X20,0X00,0X00,0X7C,0XF0,0X00,0X0F,0X00,0X00,0X03,0XFC,0XF0,0X00,0X0F,0X00,
0X00,0X03,0XFC,0XF0,0X00,0X0F,0X00,0X00,0X01,0XFC,0XF0,0X00,0X0F,0X00,0X00,0X00,
0X1C,0XF0,0X00,0X1F,0X00,0X00,0X00,0X00,0XF8,0X00,0X1F,0X30,0X00,0X00,0X00,0XF8,
0X00,0X3E,0X38,0X00,0X00,0X00,0X7C,0X00,0X3E,0X7C,0X00,0X00,0X00,0X7E,0X00,0XFC,
0X7F,0X00,0X00,0X00,0X3F,0XC3,0XFC,0X7F,0X80,0X00,0X03,0X1F,0XFF,0XF8,0X3F,0X00,
0X00,0X03,0X8F,0XFF,0XF0,0X00,0X00,0X00,0X07,0XC7,0XFF,0XC0,0X00,0X00,0X00,0X07,
0XC1,0XFF,0X80,0X00,0X00,0X00,0X0F,0XC0,0X00,0X1C,0X00,0X00,0X00,0X1F,0X00,0X00,
0X3E,0X00,0X00,0X00,0X1C,0X01,0XE0,0X7E,0X00,0X00,0X00,0X00,0X03,0XF0,0X7E,0X00,
0X00,0X00,0X00,0X03,0XF0,0X3E,0X00,0X00,0X00,0X00,0X03,0XE0,0X1F,0X00,0X00,0X00,
0X00,0X01,0XE0,0X0F,0X00,0X00,0X00,0X00,0X01,0XE0,0X07,0X00,0X00,0X00,0X00,0X01,
0XC0,0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X01,0X80,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

const unsigned char mostlysunny[] PROGMEM = { // 56x48
0X00,0X00,0X00,0X1C,0X00,0X00,0X00,0X00,0X00,0X00,0X1C,0X00,0X00,0X00,0X00,0X00,
0X00,0X1E,0X00,0X60,0X00,0X00,0X00,0X00,0X1E,0X00,0XE0,0X00,0X00,0X00,0X00,0X1E,
0X01,0XE0,0X00,0X00,0X00,0X00,0X1F,0X03,0XC0,0X00,0X00,0X00,0X00,0X1F,0X07,0XC0,
0X00,0X00,0X01,0XC0,0X1E,0X0F,0XC0,0X00,0X00,0X01,0XF0,0X00,0X07,0X80,0X00,0X00,
0X00,0XFC,0X03,0XC3,0X80,0X00,0X00,0X00,0X7E,0X1F,0XF0,0X80,0X00,0X00,0X00,0X7E,
0X7F,0XFC,0X00,0X00,0X00,0X00,0X3C,0XFF,0XFE,0X00,0X00,0X00,0X00,0X19,0XFF,0XFF,
0X00,0XE0,0X00,0X00,0X03,0XF8,0X1F,0X8F,0XE0,0X00,0X00,0X1F,0XF8,0X0F,0X8F,0XE0,
0X00,0X00,0X7F,0XFE,0X07,0XCF,0XC0,0X00,0X01,0XFF,0XFF,0X03,0XCF,0X00,0X00,0X03,
0XFF,0XFF,0X83,0XE6,0X00,0X00,0X07,0XF0,0X1F,0XC1,0XE0,0X00,0X00,0X07,0XE0,0X07,
0XE1,0XE0,0X00,0X00,0X0F,0X80,0X03,0XF1,0XE0,0X00,0X00,0X3F,0X00,0X01,0XF1,0XE0,
0X00,0X00,0XFE,0X00,0X00,0XF1,0XE0,0X00,0X01,0XFE,0X00,0X00,0XFB,0XE4,0X00,0X03,
0XFE,0X00,0X00,0X7B,0XC6,0X00,0X07,0XF0,0X00,0X00,0XFF,0XCF,0X80,0X07,0XC0,0X00,
0X01,0XFF,0XCF,0XC0,0X0F,0X80,0X00,0X03,0XFF,0XCF,0XE0,0X0F,0X00,0X00,0X01,0XFF,
0XEF,0XE0,0X1F,0X00,0X00,0X01,0XC7,0XE0,0X60,0X1E,0X00,0X00,0X00,0X01,0XF0,0X00,
0X1E,0X00,0X00,0X00,0X00,0XF0,0X00,0X1E,0X00,0X00,0X00,0X00,0XF8,0X00,0X1E,0X00,
0X00,0X00,0X00,0X78,0X00,0X1E,0X00,0X00,0X00,0X00,0X78,0X00,0X1F,0X00,0X00,0X00,
0X00,0X78,0X00,0X0F,0X00,0X00,0X00,0X00,0XF0,0X00,0X0F,0X80,0X00,0X00,0X00,0XF0,
0X00,0X07,0XC0,0X00,0X00,0X01,0XF0,0X00,0X07,0XF0,0X00,0X00,0X07,0XE0,0X00,0X03,
0XFF,0XFF,0XFF,0XFF,0XE0,0X00,0X01,0XFF,0XFF,0XFF,0XFF,0XC0,0X00,0X00,0XFF,0XFF,
0XFF,0XFF,0X80,0X00,0X00,0X3F,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

const unsigned char rain[] PROGMEM = { // 56x48
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XFF,0X80,0X00,0X00,0X00,0X00,
0X07,0XFF,0XC0,0X00,0X00,0X00,0X00,0X07,0XFF,0XC0,0X00,0X00,0X00,0X00,0X3F,0XFF,
0XF8,0X00,0X00,0X00,0X00,0X7F,0X01,0XFC,0X00,0X00,0X00,0X01,0XFC,0X00,0X3E,0X00,
0X00,0X00,0X01,0XF8,0X00,0X3F,0X00,0X00,0X00,0X03,0XF0,0X00,0X3F,0X00,0X00,0X00,
0X03,0XE0,0X00,0X1F,0X00,0X00,0X00,0X3F,0XC0,0X00,0X07,0XC0,0X00,0X00,0XFF,0X80,
0X00,0X03,0XC0,0X00,0X01,0XFE,0X00,0X00,0X03,0XC0,0X00,0X03,0XFC,0X00,0X00,0X03,
0XE0,0X00,0X03,0XE0,0X00,0X00,0X1F,0XFC,0X00,0X07,0XC0,0X00,0X00,0X3F,0XFF,0X00,
0X07,0X80,0X00,0X00,0X3F,0XFF,0X80,0X07,0X80,0X00,0X00,0X00,0X07,0XC0,0X0F,0X00,
0X00,0X00,0X00,0X03,0XC0,0X0F,0X00,0X00,0X00,0X00,0X03,0XC0,0X0F,0X00,0X00,0X00,
0X00,0X01,0XE0,0X0F,0X00,0X00,0X00,0X00,0X01,0XE0,0X07,0X00,0X18,0X00,0X00,0X01,
0XE0,0X07,0X80,0X3C,0X00,0X30,0X01,0XE0,0X07,0X80,0X3C,0X00,0X78,0X03,0XC0,0X07,
0XC0,0X3C,0X00,0X78,0X03,0XC0,0X03,0XE0,0X3C,0X18,0X78,0X07,0XC0,0X03,0XF8,0X3C,
0X3C,0X78,0X1F,0X80,0X00,0XFC,0X3C,0X3C,0X78,0X7F,0X00,0X00,0XFC,0X3C,0X3C,0X78,
0X7F,0X00,0X00,0X1C,0X18,0X3C,0X30,0X7C,0X00,0X00,0X00,0X00,0X3C,0X00,0X00,0X00,
0X00,0X00,0X00,0X3C,0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X00,0X30,0X00,0X00,0X00,0X00,0X3C,0X00,
0X78,0X00,0X00,0X00,0X00,0X3C,0X00,0X78,0X00,0X00,0X00,0X00,0X3C,0X18,0X78,0X00,
0X00,0X00,0X00,0X3C,0X3C,0X78,0X00,0X00,0X00,0X00,0X3C,0X3C,0X78,0X00,0X00,0X00,
0X00,0X3C,0X3C,0X78,0X00,0X00,0X00,0X00,0X3C,0X3C,0X78,0X00,0X00,0X00,0X00,0X18,
0X3C,0X30,0X00,0X00,0X00,0X00,0X00,0X3C,0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,  
};

const unsigned char tstorms[] PROGMEM = { // 56x48
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X3C,0X00,0X00,0X00,0X00,0X00,0X03,0XFF,0X80,0X00,0X00,0X00,0X00,0X0F,0XFF,
0XF0,0X00,0X00,0X00,0X00,0X3F,0XFF,0XFC,0X00,0X00,0X00,0X00,0X7F,0XFF,0XFE,0X00,
0X00,0X00,0X00,0XFF,0X81,0XFF,0X00,0X00,0X00,0X01,0XFC,0X00,0X7F,0X80,0X00,0X00,
0X03,0XF8,0X00,0X0F,0XC0,0X00,0X00,0X0F,0XF0,0X00,0X07,0XC0,0X00,0X00,0X3F,0XC0,
0X00,0X03,0XE0,0X00,0X00,0X7F,0XC0,0X00,0X03,0XE0,0X00,0X00,0XFF,0X80,0X00,0X01,
0XE0,0X00,0X01,0XFC,0X00,0X00,0X03,0XFC,0X00,0X03,0XF0,0X00,0X00,0X0F,0XFE,0X00,
0X07,0XE0,0X00,0X00,0X1F,0XFF,0XC0,0X07,0XC0,0X00,0X00,0X0F,0XFF,0XE0,0X0F,0X80,
0X00,0X00,0X06,0X07,0XE0,0X1F,0X80,0X00,0X00,0X00,0X03,0XF0,0X1F,0X80,0X00,0X00,
0X00,0X03,0XF0,0X1F,0X80,0X00,0X00,0X00,0X01,0XF0,0X1F,0X80,0X00,0X00,0X00,0X01,
0XF0,0X0F,0X80,0X00,0X00,0X00,0X01,0XF0,0X07,0XC0,0X00,0X00,0X00,0X01,0XF0,0X07,
0XC0,0X00,0X00,0X00,0X03,0XF0,0X03,0XE0,0X01,0XC1,0XF0,0X03,0XF0,0X03,0XF8,0X01,
0XC1,0XE0,0X0F,0XE0,0X01,0XFF,0X03,0XC3,0XE0,0XFF,0XE0,0X00,0XFF,0X03,0XC3,0X80,
0XFF,0XC0,0X00,0X7F,0X03,0X87,0X80,0XFF,0X00,0X00,0X1F,0X03,0X87,0X00,0XFC,0X00,
0X00,0X03,0X07,0X8F,0XF0,0XE0,0X00,0X00,0X00,0X07,0X1F,0XE0,0X00,0X00,0X00,0X00,
0X07,0X27,0X80,0X00,0X00,0X00,0X00,0X0F,0XEF,0X00,0X00,0X00,0X00,0X00,0X0F,0XDE,
0X00,0X00,0X00,0X00,0X00,0X01,0XFC,0X00,0X00,0X00,0X00,0X00,0X03,0XF8,0X00,0X00,
0X00,0X00,0X00,0X03,0XF0,0X00,0X00,0X00,0X00,0X00,0X03,0XE0,0X00,0X00,0X00,0X00,
0X00,0X03,0XC0,0X00,0X00,0X00,0X00,0X00,0X07,0X80,0X00,0X00,0X00,0X00,0X00,0X07,
0X00,0X00,0X00,0X00,0X00,0X00,0X06,0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X00,0X00,
0X00,0X00,0X00,0X00,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

const unsigned char cloudy[] PROGMEM =  { // 56x48
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X78,
0X00,0X00,0X00,0X00,0X00,0X01,0XFF,0X80,0X00,0X00,0X00,0X00,0X0F,0XFF,0XF0,0X00,
0X00,0X00,0X00,0X3F,0XFF,0XF8,0X00,0X00,0X00,0X00,0X3F,0XFF,0XFC,0X00,0X00,0X00,
0X00,0XFF,0XC3,0XFE,0X00,0X00,0X00,0X00,0XFE,0X00,0XFF,0X00,0X00,0X00,0X03,0XF8,
0X00,0X3F,0X80,0X00,0X00,0X03,0XF0,0X00,0X0F,0XC0,0X00,0X00,0X07,0XC0,0X00,0X07,
0XE0,0X00,0X00,0X07,0XC0,0X00,0X03,0XE0,0X00,0X00,0X07,0XC0,0X00,0X03,0XE0,0X00,
0X00,0X7F,0X80,0X00,0X01,0XF0,0X00,0X00,0XFF,0X00,0X00,0X01,0XF0,0X00,0X03,0XFF,
0X00,0X00,0X00,0XF0,0X00,0X07,0XFF,0X00,0X00,0X00,0XF0,0X00,0X0F,0XE0,0X00,0X00,
0X00,0XFE,0X00,0X1F,0X80,0X00,0X00,0X01,0XFF,0X00,0X1F,0X80,0X00,0X00,0X03,0XFF,
0X80,0X1F,0X00,0X00,0X00,0X0F,0XFF,0XC0,0X1F,0X00,0X00,0X00,0X0F,0XFF,0XE0,0X3E,
0X00,0X00,0X00,0X03,0X83,0XF0,0X3C,0X00,0X00,0X00,0X03,0X01,0XF8,0X3C,0X00,0X00,
0X00,0X00,0X00,0XF8,0X78,0X00,0X00,0X00,0X00,0X00,0XFC,0X78,0X00,0X00,0X00,0X00,
0X00,0X7C,0X78,0X00,0X00,0X00,0X00,0X00,0X3E,0X78,0X00,0X00,0X00,0X00,0X00,0X3E,
0X78,0X00,0X00,0X00,0X00,0X00,0X3E,0X7C,0X00,0X00,0X00,0X00,0X00,0X1E,0X3C,0X00,
0X00,0X00,0X00,0X00,0X3E,0X3E,0X00,0X00,0X00,0X00,0X00,0X3E,0X1E,0X00,0X00,0X00,
0X00,0X00,0X3E,0X1F,0X00,0X00,0X00,0X00,0X00,0X7C,0X1F,0X00,0X00,0X00,0X00,0X00,
0XFC,0X1F,0X80,0X00,0X00,0X00,0X00,0XF8,0X0F,0XE0,0X00,0X00,0X00,0X01,0XF8,0X07,
0XF8,0X00,0X00,0X00,0X0F,0XF0,0X03,0XFF,0XF0,0X07,0XFF,0XFF,0XE0,0X01,0XFF,0XFF,
0XFF,0XFF,0XFF,0XC0,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X3F,0XFF,0XFF,0XFF,
0XFF,0X00,0X00,0X07,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

const unsigned char snow[] PROGMEM = { // 56x48
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X3E,0X00,0X00,0X00,0X00,0X00,
0X01,0XFF,0XC0,0X00,0X00,0X00,0X00,0X07,0XFF,0XF0,0X00,0X00,0X00,0X00,0X1F,0XFF,
0XFC,0X00,0X00,0X00,0X00,0X3F,0XFF,0XFE,0X00,0X00,0X00,0X00,0X7F,0XE3,0XFF,0X00,
0X00,0X00,0X00,0XFF,0X00,0X7F,0X80,0X00,0X00,0X00,0XFE,0X00,0X1F,0XC0,0X00,0X00,
0X03,0XF8,0X00,0X0F,0XC0,0X00,0X00,0X0F,0XF8,0X00,0X07,0XE0,0X00,0X00,0X3F,0XF0,
0X00,0X07,0XE0,0X00,0X00,0X7F,0XE0,0X00,0X03,0XE0,0X00,0X00,0X7F,0XE0,0X00,0X07,
0XF8,0X00,0X00,0XFE,0X00,0X00,0X0F,0XFE,0X00,0X01,0XFC,0X00,0X00,0X1F,0XFF,0X00,
0X01,0XF8,0X00,0X00,0X1F,0XFF,0X80,0X01,0XF0,0X00,0X00,0X0F,0XFF,0XC0,0X03,0XF0,
0X00,0X00,0X06,0X0F,0XC0,0X03,0XE0,0X00,0X00,0X00,0X07,0XE0,0X03,0XE0,0X00,0X00,
0X00,0X07,0XE0,0X03,0XE0,0X00,0X00,0X00,0X03,0XE0,0X03,0XF0,0X00,0X00,0X00,0X03,
0XE0,0X01,0XF0,0X00,0X00,0X00,0X03,0XE0,0X01,0XF0,0X00,0X00,0X00,0X03,0XE0,0X01,
0XF8,0X00,0X44,0X40,0X07,0XE0,0X00,0XFE,0X00,0X24,0X80,0X0F,0XC0,0X00,0XFF,0X80,
0X15,0X00,0X3F,0XC0,0X00,0X7F,0XC0,0X0E,0X01,0XFF,0X80,0X00,0X3F,0XC0,0X7B,0XC1,
0XFF,0X00,0X00,0X1F,0XC0,0X0E,0X01,0XFE,0X00,0X00,0X07,0XC0,0X15,0X01,0XFC,0X00,
0X00,0X00,0XC0,0X24,0X81,0XE0,0X00,0X00,0X00,0X00,0X44,0X40,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X88,0X80,0X00,0X00,0X00,0X00,0X00,
0X49,0X00,0X00,0X00,0X01,0X11,0X00,0X2A,0X00,0X00,0X00,0X00,0X92,0X00,0X1C,0X00,
0X00,0X00,0X00,0X54,0X00,0XF7,0X80,0X00,0X00,0X00,0X38,0X00,0X1C,0X00,0X00,0X00,
0X01,0XEF,0X00,0X2A,0X00,0X00,0X00,0X00,0X38,0X00,0X49,0X00,0X00,0X00,0X00,0X54,
0X00,0X88,0X80,0X00,0X00,0X00,0X92,0X00,0X00,0X00,0X00,0X00,0X01,0X11,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

const unsigned char sleet[] PROGMEM = { // 56x48
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XFF,0X80,0X00,0X00,0X00,0X00,
0X07,0XFF,0XC0,0X00,0X00,0X00,0X00,0X07,0XFF,0XC0,0X00,0X00,0X00,0X00,0X3F,0XFF,
0XF8,0X00,0X00,0X00,0X00,0X7F,0X01,0XFC,0X00,0X00,0X00,0X00,0XFC,0X00,0X3E,0X00,
0X00,0X00,0X01,0XF8,0X00,0X3F,0X00,0X00,0X00,0X03,0XF0,0X00,0X3F,0X00,0X00,0X00,
0X03,0XE0,0X00,0X1F,0X00,0X00,0X00,0X3F,0XC0,0X00,0X07,0XC0,0X00,0X00,0XFF,0X80,
0X00,0X03,0XC0,0X00,0X01,0XFC,0X00,0X00,0X03,0XC0,0X00,0X03,0XFC,0X00,0X00,0X03,
0XE0,0X00,0X03,0XE0,0X00,0X00,0X1F,0XFC,0X00,0X07,0XC0,0X00,0X00,0X3F,0XFF,0X00,
0X07,0X80,0X00,0X00,0X3F,0XFF,0X80,0X07,0X80,0X00,0X00,0X00,0X07,0XC0,0X0F,0X00,
0X00,0X00,0X00,0X03,0XC0,0X0F,0X00,0X00,0X00,0X00,0X03,0XC0,0X0F,0X00,0X00,0X00,
0X00,0X01,0XE0,0X0F,0X00,0X00,0X00,0X00,0X01,0XE0,0X07,0X00,0X00,0X00,0X00,0X01,
0XE0,0X07,0X80,0X49,0X00,0X30,0X01,0XE0,0X07,0X80,0X2A,0X00,0X78,0X03,0XC0,0X07,
0XC0,0X1C,0X00,0X78,0X03,0XC0,0X03,0XE0,0X77,0X00,0X78,0X07,0XC0,0X03,0XF8,0X1C,
0X00,0X78,0X1F,0X80,0X00,0XFC,0X2A,0X00,0X78,0X7F,0X00,0X00,0XFC,0X48,0X92,0X78,
0X7F,0X00,0X00,0X1C,0X00,0X54,0X30,0X7C,0X00,0X00,0X00,0X00,0X38,0X00,0X00,0X00,
0X00,0X00,0X00,0XEE,0X00,0X00,0X00,0X00,0X00,0X00,0X38,0X00,0X00,0X00,0X00,0X00,
0X00,0X54,0X00,0X00,0X00,0X00,0X01,0X24,0X92,0X30,0X00,0X00,0X00,0X00,0XA8,0X00,
0X78,0X00,0X00,0X00,0X00,0X70,0X00,0X78,0X00,0X00,0X00,0X01,0XDC,0X18,0X78,0X00,
0X00,0X00,0X00,0X70,0X3C,0X78,0X00,0X00,0X00,0X00,0XA8,0X3C,0X78,0X00,0X00,0X00,
0X01,0X24,0X3C,0X78,0X00,0X00,0X00,0X00,0X00,0X3C,0X78,0X00,0X00,0X00,0X00,0X00,
0X3C,0X30,0X00,0X00,0X00,0X00,0X00,0X3C,0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

const unsigned char fog[] PROGMEM = { // 56x48
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X3E,0X00,0X00,0X00,0X00,0X00,
0X01,0XFF,0XC0,0X00,0X00,0X00,0X00,0X07,0XFF,0XF0,0X00,0X00,0X00,0X00,0X1F,0XFF,
0XFC,0X00,0X00,0X00,0X00,0X3F,0XFF,0XFE,0X00,0X00,0X00,0X00,0X7F,0XE3,0XFF,0X00,
0X00,0X00,0X00,0XFF,0X00,0X7F,0X80,0X00,0X00,0X00,0XFE,0X00,0X1F,0XC0,0X00,0X00,
0X03,0XF8,0X00,0X0F,0XC0,0X00,0X00,0X0F,0XF8,0X00,0X07,0XE0,0X00,0X00,0X3F,0XF0,
0X00,0X07,0XE0,0X00,0X00,0X7F,0XE0,0X00,0X03,0XE0,0X00,0X00,0X7F,0XE0,0X00,0X07,
0XF8,0X00,0X00,0XFE,0X00,0X00,0X0F,0XFE,0X00,0X01,0XFC,0X00,0X00,0X1F,0XFF,0X00,
0X01,0XF8,0X00,0X00,0X1F,0XFF,0X80,0X01,0XF0,0X00,0X00,0X0F,0XFF,0XC0,0X03,0XF0,
0X00,0X00,0X06,0X0F,0XC0,0X03,0XE0,0X00,0X00,0X00,0X07,0XE0,0X03,0XE0,0X00,0X00,
0X00,0X07,0XE0,0X03,0XE0,0X00,0X00,0X00,0X07,0XE0,0X03,0XE0,0X00,0X00,0X00,0X03,
0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X03,0XFF,0XFF,
0XFF,0XFF,0XFF,0XE0,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,
0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE0,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,
0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

const unsigned char nodata[] PROGMEM = 
{ // 56x48
};

GxIO_Class io(SPI, SS, 17, 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, 16, 4); // arbitrary selection of (16), 4

//------ NETWORK VARIABLES---------
// Use your own API key by signing up for a free developer account at http://www.wunderground.com/weather/api/
String apikey     = "*****";            // See: http://www.wunderground.com/weather/api/d/docs (change here with your KEY)
String City       = "*****";                     // Your home city
String Country    = "*****";                          // Your country   
String Conditions = "*****";                  // See: http://www.wunderground.com/weather/api/d/docs?d=data/index&MR=1
char   wxserver[] = "api.wunderground.com";        // Address for WeatherUnderGround
unsigned long     lastConnectionTime = 0;          // Last time you connected to the server, in milliseconds
const unsigned long  postingInterval    = 15L*60L*1000L; // Delay between updates, in milliseconds, WU allows 500 requests per-day maximum, set to every 15-mins or 144/day
String Units      =  "X"; // M for Metric, X for Mixed and I for Imperial
String time_str, currCondString; // strings to hold time and received weather data;

//################ PROGRAM VARIABLES and OBJECTS ################

// Conditions
String WDay0, Day0, Icon0, High0, Low0, Conditions0, Pop0, Averagehumidity0,
       WDay1, Day1, Icon1, High1, Low1, Conditions1, Pop1, Averagehumidity1,
       WDay2, Day2, Icon2, High2, Low2, Conditions2, Pop2, Averagehumidity2,
       WDay3, Day3, Icon3, High3, Low3, Conditions3, Pop3, Averagehumidity3;

String  DphaseofMoon, Sunrise, Sunset, Moonrise, Moonset, Moonlight;

const char* ssid     = "*****";
const char* password = "*****";
const char* host     = "api.wunderground.com";

WiFiClient client; 

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  display.init();
  Serial.println("setup done");

  delay(500);
  StartWiFi(ssid,password);
  SetupTime();
  lastConnectionTime = millis();
  obtain_forecast("forecast");
  obtain_forecast("astronomy");
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop()
{    
  display.setRotation(1);      //Rotation
  delay(1000);
  
  DisplayForecast();
  display.update(); 
  
  esp_deep_sleep_start();
}


void DisplayForecast()  // Display is 400x300 resolution
 {
  const char* FreeSansBoldOblique = "FreeSansBoldOblique9pt7b";
  const GFXfont* f = &FreeSansBoldOblique9pt7b;

  const char* name = "Org_01";
  const GFXfont* g = &Org_01;
 
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(g);                   
  
  DisplayWXicon(20,25, Icon0);  //Weather Icon
  
  DisplayText(10,10,"Weather for " + time_str);   //Small Text
  DisplayText(145,25,"High:"); 
  DisplayText(145,55,"Low:"); 
  DisplayText(75,85,"Weather Conditions: " + Conditions0); 
  DisplayText(75,95,"Humidity: " + Averagehumidity0 + "%");
  DisplayText(150,95,"Pop: " + Pop0 + "%");

  display.setTextColor(GxEPD_RED);
  display.setFont(f);
  DisplayText(145,42,High0 + " C");       //Large Text
  DisplayText(145,72,Low0 + " C"); 
}

void DisplayText(int x, int y, String text)
{
  display.setCursor(x,y);
  display.println(text);
}

void DisplayWXicon(int x, int y, String IconName){
  int len = 56*48; char myChar[56*48]={};;
  for (int k = 0; k < len; k++) {
    myChar[k] =  IconName[k];
  }
  Serial.println(IconName);
  if      (IconName == "rain"            || IconName == "nt_rain"         ||
           IconName == "chancerain"      || IconName == "nt_chancerain")
           display.drawBitmap(x,y, rain, 56,48, GxEPD_BLACK);
  else if (IconName == "snow"            || IconName == "nt_snow"         ||
           IconName == "flurries"        || IconName == "nt_flurries"     ||
           IconName == "chancesnow"      || IconName == "nt_chancesnow"   ||
           IconName == "chanceflurries"  || IconName == "nt_chanceflurries")
           display.drawBitmap(x,y, snow, 56,48, GxEPD_RED);
  else if (IconName == "sleet"           || IconName == "nt_sleet"        ||
           IconName == "chancesleet"     || IconName == "nt_chancesleet")
           display.drawBitmap(x,y, sleet, 56,48, GxEPD_BLACK);
  else if (IconName == "sunny"           || IconName == "nt_sunny"        ||
           IconName == "clear"           || IconName == "nt_clear")
           display.drawBitmap(x,y, sunny, 56,48, GxEPD_BLACK);
  else if (IconName == "partlysunny"     || IconName == "nt_partlysunny"  ||
           IconName == "mostlysunny"     || IconName == "nt_mostlysunny")
           display.drawBitmap(x,y, mostlysunny, 56,48, GxEPD_BLACK);
  else if (IconName == "cloudy"          || IconName == "nt_cloudy"       ||
           IconName == "mostlycloudy"    || IconName == "nt_mostlycloudy" ||
           IconName == "partlycloudy"    || IconName == "nt_partlycloudy")  
           display.drawBitmap(x,y, cloudy, 56,48, GxEPD_BLACK);
  else if (IconName == "tstorms"         || IconName == "nt_tstorms"      ||
           IconName == "chancetstorms"   || IconName == "nt_chancetstorms")
           display.drawBitmap(x,y, tstorms, 56,48, GxEPD_BLACK);
  else if (IconName == "fog"             || IconName == "nt_fog"          ||
           IconName == "hazy"            || IconName == "nt_hazy")
           display.drawBitmap(x,y, fog, 56,48, GxEPD_BLACK);
  else if (IconName == "thermo")
           delay(1);
  else     display.drawBitmap(x,y, nodata, 56,48, GxEPD_BLACK);
}

bool obtain_forecast (String forecast_type) {
  client.stop();  // Clear any current connections
  Serial.println("Connecting to "+String(host)); // start a new connection
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
   Serial.println("Connection failed");
   return false;
  }
  // Weather Underground API address http://api.wunderground.com/api/YOUR_API_KEY/conditions/q/YOUR_STATE/YOUR_CITY.json
  String url = "http://api.wunderground.com/api/"+apikey+"/"+forecast_type+"/q/"+Country+"/"+City+".json";
  Serial.println("Requesting URL: "+String(url));
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Connection Timeout...Stopping");
      client.stop();
      return false;
    }
  }
  Serial.print("Receiving API weather data");
  while(client.available()) {
    currCondString = client.readStringUntil('\r');
    Serial.print(".");
  }
  Serial.println("\r\nClosing connection");
  //Serial.println(*currCondString);
  if (forecast_type == "forecast"){
    if (showWeather_forecast(&currCondString)); else Serial.println("Failed to get Weather Data");
  }
  if (forecast_type == "astronomy"){
    //if (showWeather_astronomy(&currCondString)); else Serial.println("Failed to get Astronomy Data");
  }
  return true;
}

bool showWeather_forecast(String* currCondString) {
  Serial.println("Creating object...");
  DynamicJsonBuffer jsonBuffer(8*1024);
  // Create root object and parse the json file returned from the api. The API returns errors and these need to be checked to ensure successful decoding
  JsonObject& root = jsonBuffer.parseObject(*(currCondString));
  if (!root.success()) {
    Serial.println(F("jsonBuffer.parseObject() failed"));
  }
  JsonObject& forecast = root["forecast"]["simpleforecast"];
  String wday0  = forecast["forecastday"][0]["date"]["weekday_short"];  WDay0 = wday0;
  int    day0   = forecast["forecastday"][0]["date"]["day"];            day0<10?(Day0="0"+String(day0)):(Day0=String(day0));
  String mon0   = forecast["forecastday"][0]["date"]["monthname_short"];
  String year0  = forecast["forecastday"][0]["date"]["year"];           Day0 += " " + mon0 + " " + year0.substring(2); //Day0 += "-" + mon0 + "-" + year0.substring(2); 
  String icon0  = forecast["forecastday"][0]["icon"];                   Icon0 = icon0;
  String high0  = forecast["forecastday"][0]["high"]["celsius"];        High0 = high0;
  String low0   = forecast["forecastday"][0]["low"]["celsius"];         Low0  = low0;
  String conditions0 = forecast["forecastday"][0]["conditions"];        Conditions0  = conditions0;
  String pop0        = forecast["forecastday"][0]["pop"];               Pop0  = pop0;
  String averagehumidity0 = forecast["forecastday"][0]["avehumidity"];  Averagehumidity0 = averagehumidity0;
  return true;
}

int StartWiFi(const char* ssid, const char* password)
{
 int connAttempts = 0;
 Serial.print(F("\r\nConnecting to: ")); Serial.println(String(ssid));
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED ) {
   delay(500); Serial.print(".");
   if(connAttempts > 20) return -5;
   connAttempts++;
 }
 Serial.print(F("WiFi connected at: "));
 Serial.println(WiFi.localIP());
 return 1;
}

void clear_screen() {
   display.fillScreen(GxEPD_WHITE);
   display.update();
}  


void SetupTime()
{
  configTime(0 * 3600, 3600, "0.uk.pool.ntp.org", "time.nist.gov");
  UpdateTime();
}

void UpdateTime()
{
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  //See http://www.cplusplus.com/reference/ctime/strftime/
  Serial.println(&timeinfo, "%a %b %d %Y   %I:%M:%p"); // Displays: Saturday, June 24 2017 14:05:49  %Y
  time_str = asctime(&timeinfo); // Displays: Sat Jun 24 14:05:49 2017
  time_str = time_str.substring(0,24); // Displays: Sat Jun 24 14:05
}

