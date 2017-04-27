## Overview

I developed this with my kids and now we take it into Schools to introduce a
variety of concepts to the children and teachers.

It uses a cheap ESP8266 and i2c driven 8x8 LED Matrix. Both bought for under
£10.

The device acts as a wireless access point they connect to with an iPad (or any
other wifi device with a web browser) and can make the LED Matrix scroll
messages. They can also turn 8x8 drawings into binary, then convert to decimal
and send those numbers to see their design come up.

### Setup

We used Arduino IDE 1.8.2 on a mac and had to set up [ESP8266 Sketch Data Upload](https://github.com/esp8266/arduino-esp8266fs-plugin/releases/latest)

On Feather Huzzah ESP8266 Wifi Data SDA was pin 4 clock SCL was pin 5 as labeled on the board. These are the last 2 pins same side as lipo socket furthest away from usb socket.

On Tinxi NodeMCU Lua ESP-12E board the pins were on the other side 2nd and 3rd to last far end from the usb. Labeled D1 (Clock) D2 (Data).

### Built with

* [Pure CSS](https://purecss.io) - Web framework used
* [Adafruit LED Lib](https://github.com/adafruit/Adafruit_LED_Backpack) -
  Matrix driver code 
* [ESP8266 filesystem uploader](https://github.com/esp8266/arduino-esp8266fs-plugin) - To upload webserver content
