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
#include <U8x8lib.h>  // to be included with the arduino library manager (U8g2 by Oliver)

// define display
U8X8_SSD1306_128X64_NONAME_SW_I2C oled(15, 4, 16);

// pin numbers for LoRa Chip
#define ss 18
#define rst 14
#define dio0 26

void setup() 
{
  Serial.begin(115200);
  
  // print Welcome message
  oled.begin();
  oled.setFont(u8x8_font_courB18_2x3_r);
  oled.drawString(0, 0, "LoRaAPRS");
  oled.drawString(0, 3, "Monitor");
  oled.setFont(u8x8_font_amstrad_cpc_extended_r);
  oled.drawString(0, 6, "by DJ0ABR");

  // init SPI interface
  SPI.begin(5, 19, 27, 18);
  // define pins for LoRa chip
  LoRa.setPins(ss, rst, dio0);

  // Initialize LoRa chip 
  if (!LoRa.begin(433775000)) {

    oled.drawString(0, 6, "LoRa failed");
    while (1);
  }

  // set parameters required for LoRa APRS
  LoRa.setFrequency(433775000);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();

  delay(1000);
  oled.drawString(0, 7, "Init OK !");
  delay(2000);
}

#define MAXRXLEN 200
String rxstr;
int rssi;

void loop() 
{
  
  // check if packet was received
  int n = LoRa.parsePacket();
  if (n) 
  {
    // read packet
    int idx = 0;
    rxstr = "";
    while (LoRa.available()) 
    {
      rxstr += (char)LoRa.read();
      if(++idx >= MAXRXLEN) break;
    }

    // ignore non-APRS packets
    if(rxstr[0] != '<') return;

    Serial.print(rxstr);

    // read rssi of last reception
    rssi = LoRa.packetRssi();

    // extract source callsign
    int pos = rxstr.indexOf('>'); // source callsign ends here
    String source = rxstr.substring(3,pos);

    // print RX in oled display
    printLine(source,rssi);
  }
}

int line = 0;
String lines[8];

void printLine(String source, int rssi)
{
  // clear display if it is the first data after reset
  if(line == 0) oled.clearDisplay();
  
  if(line < 8)
  {
    // display is not full, print line by line
    lines[line++] = source + " " + String(rssi);    
  }
  else
  {
    // display is full, scroll display then print last line
    for(int i=0; i<7; i++)
      lines[i] = lines[i+1];
      
    lines[7] = source + " " + String(rssi);    
  }

  // draw lines to display
  for(int i=0; i<8; i++)
    oled.drawString(0, i, lines[i].c_str());    
}
