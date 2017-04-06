
/*
 * On NodeMCU ESP-12E clock and data are pins 5 and 4 which are 
 * D1 (GPIO5 i2c clock) D2 (GPIO4 i2c Data)
 */

#include <ESP8266WebServer.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <FS.h>

#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"

#define MODE_SCROLL  1
#define MODE_SHAPE  2

const char WiFiAPPSK[] = "funfunfun";
ESP8266WebServer server(80);
String scroll_message=" I am working";
int8_t my_mode=MODE_SCROLL;
int scroll_position=0;

String http_header="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";

uint8_t shape_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 };

static const uint8_t PROGMEM
  smile_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  neutral_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
  frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 };

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();
void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  doSpiffs();
  matrix.begin(0x70);  // pass in the address
  setupWiFi();
  server.on("/", handleRoot);
  server.on("/scroll",setup_message);
  server.on("/shape",shape_update);
  server.on("/cool", [](){  
    server.send(200, "text/plain", "elvis");              
  });
  server.on("/x",[](){
    File file = SPIFFS.open("/index.html", "r");
    size_t sent = server.streamFile(file, "text/html");
    file.close();
  });
  server.on("/milligram.min.css",[](){
    File file = SPIFFS.open("/milligram.min.css", "r");
    size_t sent = server.streamFile(file, "text/css");
    file.close();
  });
  server.on("/reset", [](){ 
    server.send(200, "text/plain", "resetting");      
    ESP.eraseConfig();
    // Fire Watchdog Reset to crash
    //while(1);
    //ESP.reset(); 
    ESP.restart();     
  });
  server.begin();
}

void loop() {
  //wifi_command();
  server.handleClient();
  if (my_mode==MODE_SCROLL)
  {
    do_scroll();
  }
  /*
  matrix.clear();
  matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(500);

  matrix.clear();
  matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(500);

  matrix.clear();
  matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(500);

  matrix.clear();      // clear display
  matrix.drawPixel(0, 0, LED_ON);  
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.clear();
  matrix.drawLine(0,0, 7,7, LED_ON);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.clear();
  matrix.drawRect(0,0, 8,8, LED_ON);
  matrix.fillRect(2,2, 4,4, LED_ON);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.clear();
  matrix.drawCircle(3,3, 3, LED_ON);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  matrix.setRotation(1);
  for (int8_t x=0; x>=-36; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("Hi");
    matrix.writeDisplay();
    delay(100);
  }
  for (int8_t x=7; x>=-36; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("World");
    matrix.writeDisplay();
    delay(100);
  }
  matrix.setRotation(0);
  */
}

void shape_update(){
    matrix.clear();
    for (int c=1;c<9;c++)
    {
      if (server.hasArg("n"+String(c)))
      {
        shape_bmp[c-1]= (uint8_t) server.arg("n"+String(c)).toInt();
      }
    }
    matrix.drawBitmap(0, 0, shape_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    my_mode=MODE_SHAPE;
    server.send(200, "text/plain", "Sure I'll update the shape "+server.arg("n"+String(1)));    
}

void setup_message(){
  if (server.hasArg("txt"))
  {
    scroll_message=" "+server.arg("txt");
    scroll_position=0;
    my_mode=MODE_SCROLL;
    server.send(200, "text/plain", "Sure I'll scroll - "+scroll_message);    
  }
}

void handleRoot(){
  String content = "<html><body><H2>hello, you successfully connected!</H2><br>";
  content+="<a href='/scroll?txt=you%27re%20message'>/scroll?txt=you%27re%20message</a><br>";
  content+="<form action='/scroll' method='post'><div><label>Message:</label><input type='text' name='txt'></input></div></form>";
  content+="</body></html>";
  server.send(200, "text/html", content);
}

void do_scroll()
{
  matrix.setTextSize(1);
  matrix.setTextWrap(false); 
  matrix.setRotation(1);
  matrix.clear();
  matrix.setCursor(-scroll_position,0);
  scroll_position=(scroll_position+1) % (scroll_message.length()*6);
  matrix.print(scroll_message);
  matrix.writeDisplay();
  delay(100);
}
/*
void wifi_command()
{
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  String req=client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  if (req.indexOf("/text/") != -1)
  {
    client.print(http_header+"you want to change the text");
  }
  else
  {
    client.print(http_header+"Hi Children");
  }
}
*/
void setupWiFi()
{
  //
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "Cool Thing " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);

  //WiFi.softAP("Cool Thing", WiFiAPPSK);
}

void doSpiffs()
{
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}

String formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

