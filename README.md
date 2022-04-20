# LoRa-APRS-Monitor
a simple monitor to test the signal of an LoRa APRS tracker

it shows the callsign and RSSI of received stations

## required hardware
HELTEC WiFi Lora 32(V2) Board

## usage
load the project with Arduino IDE

## Required Settings in Arduino IDE:
Board Definition: https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.5/package_heltec_esp32_index.json

Board: WiFi LoRa 32(V2)

Region: EU433

## Required Libraries:
U8g2 by Oliver (Graphics Library for Displays)

## installation
compile and flash into Heltec board, then watch the display:

Callsign   min-RSSI   actual-RSSI   max-RSSI

the display can show up to 6 different callsigns, the last RXed callsign is highlighted

![alt text](https://github.com/dj0abr/LoRa-APRS-Monitor/hmon.png)
