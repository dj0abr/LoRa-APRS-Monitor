/*
 * LoRa APRS Monitor for Heltec Wifi Lora ESP32 V.2
 * 
 * Required Settings in Arduino IDE:
 * =================================
 * Board Definition: https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.5/package_heltec_esp32_index.json
 * Board: WiFi LoRa 32(V2)
 * Region: EU433
 * 
 * Required Libraries:
 * ===================
 * U8g2 by Oliver (Graphics Library for Displays)
 *  
 */

#include <SPI.h>      // standard Arduino Library
#include "LoRa.h"     // LoRa for Semtech SX1276/77/78/79 Chipy by Sandeep Mistry (included with this source)
#include <U8g2lib.h>

// define display
U8G2_SSD1306_128X64_NONAME_1_SW_I2C oled(U8G2_R0, 15, 4, 16);

// pin numbers for LoRa Chip
#define ss 18
#define rst 14
#define dio0 26

#define MAXLINES 6
String call[MAXLINES];
int act_rssi[MAXLINES];
int min_rssi[MAXLINES];
int max_rssi[MAXLINES];

void setup() 
{
  Serial.begin(115200);
  
  // print Welcome message
  oled.begin();
  drawDisplay(1,0);

  // init SPI interface
  SPI.begin(5, 19, 27, 18);
  // define pins for LoRa chip
  LoRa.setPins(ss, rst, dio0);

  // Initialize LoRa chip 
  if (!LoRa.begin(433775000)) {
    drawDisplay(3,0);
    while (1);
  }

  // set parameters required for LoRa APRS
  LoRa.setFrequency(433775000);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();

  delay(2000);

  // clear display and reset variables
  oled.clearDisplay();
  for(int i=0; i<MAXLINES; i++)
  {
    call[i] = "";
    act_rssi[i] = 0;
    min_rssi[i] = 1000;
    max_rssi[i] = -1000;
  }
}

#define MAXRXLEN 200

void loop() 
{
  
  // check if packet was received
  int n = LoRa.parsePacket();
  if (n) 
  {
    // read packet
    int idx = 0;
    String rxstr = "";
    while (LoRa.available()) 
    {
      rxstr += (char)LoRa.read();
      if(++idx >= MAXRXLEN) break;
    }

    Serial.print(rxstr);
    
    // ignore non-APRS packets
    if(rxstr[0] != '<') return;

    // read rssi of last reception
    int rssi = LoRa.packetRssi();
    Serial.print(rssi);

    // extract source callsign
    int pos = rxstr.indexOf('>'); // source callsign ends here
    String source = rxstr.substring(3,pos);

    // print RX in oled display
    printLine(source,rssi);
  }
}

void printLine(String source, int rssi)
{
  // check if the new source callsign is already in the screen
  for(int i=0; i<MAXLINES; i++)
  {
    Serial.print(source);
    Serial.print(call[i]);
    if(source == call[i])
    {
      Serial.print("found");
      // callsign exists, update line
      act_rssi[i] = rssi;
      if(rssi < min_rssi[i]) min_rssi[i] = rssi;
      if(rssi > max_rssi[i]) max_rssi[i] = rssi;
      // draw lines to display
      drawDisplay(2,i);
      return;
    }
  }

  // new callsign, make new entry

  // search free line
  int line = -1;
  for(int i=0; i<MAXLINES; i++)
  {
    if(call[i] == "")
    {
      line = i;
      break;
    }
  }

  if(line == -1) return; // no more free lines

  // enter new data in line "line"
  call[line] = source;
  act_rssi[line] = min_rssi[line] = max_rssi[line] = rssi;

  // draw lines to display
  drawDisplay(2,line);
}

void drawDisplay(int mode, int actline)
{
char s[50];

  oled.firstPage();
  do {
    if(mode == 1)
    {
      // welcome
      oled.setFont(u8g2_font_ncenB14_tr);
      oled.drawStr(0,24,"LoRa APRS");
      oled.drawStr(0,40,"Monitor");
      oled.setFont(u8g2_font_helvB08_tf);
      oled.drawStr(0,60,"DJ0ABR");
    }
  
    if(mode == 2)
    {
      // APRS monitor data    
      oled.setFont(u8g2_font_profont11_tr);
      for(int i=0; i<MAXLINES; i++)
      {
        if(call[i] == "") continue;
        if(i == actline)
        {
          oled.setDrawColor(1);
          oled.drawBox(0,i*10+1,127,10+1);
          oled.setDrawColor(0);        
        }
        else
        {
          oled.setDrawColor(1);
        }
        sprintf(s,"%8.8s %3d %3d %3d",call[i],min_rssi[i],act_rssi[i],max_rssi[i]);
        oled.drawStr(0,i*10+10,s);
      }
    }
    
    if(mode == 3)
    {
      // LoRa Error
      oled.setFont(u8g2_font_ncenB14_tr);
      oled.drawStr(0,24,"LoRa ERROR");
    }
  } while ( oled.nextPage() );
}
