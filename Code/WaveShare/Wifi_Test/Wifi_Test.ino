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


#include <WiFi.h>
#include <WiFiUdp.h>

#include <SPI.h> 
#include <Wire.h>      
#include <TimeLib.h>

#include "epd2in13b.h"
#include "imagedata.h"
#include "epdpaint.h"

#include "ClosedCube_HDC1080.h"

#define COLORED     0
#define UNCOLORED   1

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300        /* Time ESP32 will go to sleep (in seconds) */  //60 = 1min , 900 = 15min


RTC_DATA_ATTR int bootCount = 0;

const char* ssid = "*****";              // your network SSID (name)
const char* password = "*****";  // your network password
//int status = WL_IDLE_STATUS;        // the Wifi radio's status


static const char ntpServerName[] = "time.nist.gov";
const int timeZone = -4;  //Time Adjust depending on where you live

ClosedCube_HDC1080 hdc1080;
WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

bool updateTime=1;
byte old_minute=0,old_hour=0; 
 

time_t prev = 0, prevNow=0;
Epd epd;


void print_wakeup_reason()
{ 
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); 
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}



void setup() 
{
  Serial.begin(9600);
  delay(100);
  
  hdc1080.begin(0x40);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  print_wakeup_reason();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);  //NEW
  //Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  
  if (epd.Init() != 0) 
  {
   Serial.print("e-Paper init failed"); 
   delay(1000);
   return;
  }
  Serial.print("e-Paper init success\n");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  //bool state = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("Attempting to connect to WiFi.. ");
    delay(5000);
  }
  
  Serial.println("You're connected to the network");

  Serial.println("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300); 
}


void loop() 
{
  
  char temp[]= "****";
  char humid[]= "****";
  char IP[]= "***.***.***.***";
  char time_out[]= "***";

  old_hour = hour();
  old_minute = minute();


  yield();  //Let the ESP32 handle the wifi stack

  epd.ClearFrame();
  
  unsigned char image[3072]; 
  Paint paint(image, 248, 64); 
  
  paint.SetWidth(104);  //128 now prints across
  paint.SetHeight(212);  //was 128, now prints down

  paint.SetRotate(1);   

  paint.Clear(UNCOLORED);

  paint.DrawStringAt(5, 5, "trINKet Test", &Font16, COLORED);
  paint.DrawStringAt(5, 20, "WaveShare", &Font16, COLORED);

  //epd.DisplayFrame(IMAGE_BLACK, IMAGE_RED);  //Image

   //First Line
  Serial.println("First Line");
  IPAddress ip = WiFi.localIP();
  ip.toString().toCharArray(IP, 16); 
  paint.DrawStringAt(5, 35, IP, &Font16, COLORED); 
                                        
  //Second Line
  Serial.println("Second Line");
  float temperature = hdc1080.readTemperature();
  dtostrf(temperature,4,1,temp);                          
  paint.DrawStringAt(5, 50, temp, &Font16, COLORED); //over,down 
  paint.DrawStringAt(55, 50, "C", &Font16, COLORED); 
                      
  //Third Line
  Serial.println("Third Line");
  float humidity = hdc1080.readHumidity();
  dtostrf(humidity,4,1,humid);             
  paint.DrawStringAt(5, 65, humid, &Font16, COLORED);
  paint.DrawStringAt(55, 65, "%", &Font16, COLORED);

  if (old_hour > 12) old_hour = old_hour - 12; 

  //Fourth Line
  Serial.println("Fourth Line");
  sprintf (time_out, "%2u:%02u", old_hour,old_minute);    //converts Byte to Char Try %02d
  paint.DrawStringAt(5, 80, time_out, &Font16, COLORED); 
  
  if (hour() > 12) paint.DrawStringAt(65, 80, "pm", &Font16, COLORED);
 
  else paint.DrawStringAt(65, 80, "am", &Font16, COLORED);
  
  epd.SetPartialWindowBlack(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());  

  epd.DisplayFrame();
  
  Serial.print("Start Sleep");
  epd.Sleep();
  esp_deep_sleep_start();
}


const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets


time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}



