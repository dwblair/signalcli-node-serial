
// *** IMPORTANT: First install required libraries via the Arduino IDE by clicking on each of the links below:
// SCD30:  http://librarymanager/All#SparkFun_SCD30  
// U8g2: http://librarymanager/All#u8x8 
// Bounce2: http://librarymanager/All#bounce2  (install the first library that comes up only)
// ArduinoJSON: http://librarymanager/All#arduinojson
// BMP388_DEV: http://librarymanager/All#bmp388_dev

#include "config.h" // My WiFi configuration.
#include <U8x8lib.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"  //  https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library
#include <ArduinoJson.h> //https://arduinojson.org/v6/doc/installation/
#include <BMP388_DEV.h> // https://github.com/MartinL1/BMP388_DEV
#include <Bounce2.h> // https://github.com/thomasfredericks/Bounce2
#include <SPI.h>
#include <RH_RF95.h>

int leftPos = 9;
int shortkeyLength = 3;
int numRemoteNodes = 5;

// heltec wifi lora 32 v2
#define RFM95_CS 18
#define RFM95_RST 14
#define RFM95_INT 26

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

WiFiMulti wifiMulti;

SCD30 airSensor;

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

long lastMeasureTime = 0;  // the last time the output pin was toggled

long measureDelay = interval_sec*1000;

String post_url="http://"+String(bayou_base_url)+"/"+String(feed_pubkey);

String short_feed_pubkey = String(feed_pubkey).substring(0,shortkeyLength);

 int co2 = 0.;
    float temp = 0.;
    float humid = 0.;

    float aux_temp = 0.; // dummy value for now
    float aux_press = 0.; //dummy value for now
    
void setup() {

  u8x8.begin();
  
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
   u8x8.clear();
   u8x8.setCursor(0,0); 
   u8x8.print("Starting...");
   delay(1000);
   
  
  Serial.begin(115200);
  Serial.println();

//Serial.println(node_shortkeys[2]);


  Wire.begin();

  if (airSensor.begin() == false)
  {
     u8x8.setCursor(0,2); 
   u8x8.print("SCD30 missing?");
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }
  u8x8.setCursor(0,2); 
   u8x8.print("SCD30 detected!");
   delay(1000);

if (!setupLoRa()) {
    Serial.println("LoRa setup failed ... ");
     u8x8.setCursor(0,4); 
   u8x8.print("LoRa not set?");
    while (1)
      ;
   }
    u8x8.setCursor(0,4); 
   u8x8.print("LoRa setup!");
   delay(1000);

    u8x8.clear();

   Serial.println(RH_RF95_MAX_MESSAGE_LEN);
   
   wifiMulti.addAP(SSID,WiFiPassword);
}

int firstLoop = 1;

void loop() {

if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
       Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);

      //Form the JSON:
        DynamicJsonDocument radiodoc(1024);
        deserializeJson(radiodoc, buf);

        Serial.println("deserialized:");
        serializeJson(radiodoc,Serial);
        String json;
        serializeJson(radiodoc,json);
        
        Serial.println();
        
        Serial.println("public key:");
        const char* remote_key = radiodoc["public_key"];
        String remote_pub = String(remote_key);
        
        Serial.println(remote_key);
        String remote_shortkey =remote_pub.substring(0,shortkeyLength);

        int matchnum = -1;
        
        for (int i = 0; i< numRemoteNodes; i++) {
        Serial.println(node_shortkeys[i]);
        String valid_key = String(node_shortkeys[i]);
        if(remote_shortkey.equals(valid_key))
          matchnum=i;
        }
        
        if (matchnum>-1) {
          Serial.println("we should post this!");
          
          //serializeJson(doc, json);
          postRemoteData(matchnum,remote_pub, json);
        }
      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(LED, LOW);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }

  
  if (  ( (millis() - lastMeasureTime) > measureDelay) || firstLoop) {


  
  if (firstLoop) firstLoop = 0;

if (airSensor.dataAvailable())
  {

    co2 = airSensor.getCO2();
    temp = roundf(airSensor.getTemperature()* 100) / 100;
    humid = roundf(airSensor.getHumidity()* 100) / 100;

    aux_temp = 0.; // dummy value for now
    aux_press = 0.; //dummy value for now
    
    //u8x8.clear();

    //u8x8.setFont(u8x8_font_7x14B_1x2_f);
    //u8x8.setFont(u8x8_font_inr33_3x6_f);
    u8x8.setFont(u8x8_font_inb21_2x4_n);
    u8x8.setCursor(0,0); 
    u8x8.print("    ");
    
    u8x8.setFont(u8x8_font_7x14B_1x2_f);
    u8x8.setCursor(0,4); 
    u8x8.print("          ");
    //u8x8.print(" C");
    u8x8.setCursor(0,6); 
    u8x8.print("          ");
    //u8x8.print(" RH");
    
    //u8x8.setFont(u8x8_font_7x14B_1x2_f);
    //u8x8.setFont(u8x8_font_inr33_3x6_f);
    u8x8.setFont(u8x8_font_inb21_2x4_n);
    u8x8.setCursor(0,0); 
    u8x8.print(co2);
    
    u8x8.setFont(u8x8_font_7x14B_1x2_f);
    u8x8.setCursor(0,4); 
    u8x8.print(temp);
    u8x8.print(" C");
    u8x8.setCursor(0,6); 
    u8x8.print(humid);
    u8x8.print(" RH");

    postOwnData();
    
  }
    lastMeasureTime = millis(); //set the current time
  }
}

int postOwnData() {

    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(leftPos,0);
    u8x8.print("      ");
    u8x8.setCursor(leftPos,0);
    u8x8.print(short_feed_pubkey);
    u8x8.print(":");

  if((wifiMulti.run() == WL_CONNECTED)) {

        //Form the JSON:
        DynamicJsonDocument doc(1024);
        
        Serial.println(post_url);
        
        doc["private_key"] = feed_privkey;
        doc["co2"] =  co2;
        doc["tempC"]=temp;
        doc["humidity"]=humid;
        doc["mic"]=0.;
        doc["auxPressure"]=aux_press;
        doc["auxTempC"]=aux_temp;
        doc["aux001"]=0.;
        doc["aux002"]=0.;
        doc["aux003"]=0.;
        doc["log"]=0.;
         
        String json;
        serializeJson(doc, json);
        serializeJson(doc, Serial);
        Serial.println("\n");
        
        // Post online

        HTTPClient http;
        int httpCode;
        
        http.begin(post_url);
        http.addHeader("Content-Type", "application/json");
        //Serial.print("[HTTP] Connecting ...\n");
      
        httpCode = http.POST(json);        

        if(httpCode > 0) {
          
            //Serial.printf("[HTTP code]: %d\n", httpCode);

            //u8x8.setFont(u8x8_font_7x14B_1x2_f);
            //u8x8.setFont(u8x8_font_chroma48medium8_r);
            //u8x8.setCursor(10,2);
            u8x8.print(httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
              
            }
        } else {
            //Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            //u8x8.setFont(u8x8_font_7x14B_1x2_f);
              //u8x8.setFont(u8x8_font_chroma48medium8_r);
              //u8x8.setCursor(10,2);
              u8x8.print(http.errorToString(httpCode).c_str());
       }

        http.end();
        
}
else { // no wifi connection
             //u8x8.setFont(u8x8_font_7x14B_1x2_f);
            //u8x8.setFont(u8x8_font_chroma48medium8_r);
              //u8x8.setCursor(10,2);
              u8x8.print("wifi?");
}
}

int setupLoRa() {

  int errCode=1;
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    errCode = 0;
    
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    //while (1);
    errCode = 0;
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  return errCode;
  
}

int postRemoteData(int matchnum, String pubkey, String payload) {

  String remote_shortkey =pubkey.substring(0,shortkeyLength);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
     u8x8.setCursor(leftPos,matchnum+1);
    u8x8.print("      ");
     u8x8.setCursor(leftPos,matchnum+1);
    u8x8.print(remote_shortkey);
    u8x8.print(":");
  
  if((wifiMulti.run() == WL_CONNECTED)) {



        //Form the JSON:
        //DynamicJsonDocument doc(1024);

        //String pubkey = mydoc["public_key"];
        String remote_post_url="http://"+String(bayou_base_url)+"/"+pubkey;

        //mydoc.remove("public_key");
        //Serial.println("pared doc:");
       
        Serial.println(remote_post_url);
         
        //String json;
        //serializeJson(mydoc, json);
        //serializeJson(mydoc, Serial);
        Serial.println("\n");
        
        // Post online

        HTTPClient http;
        int httpCode;
        
        http.begin(remote_post_url);
        http.addHeader("Content-Type", "application/json");
        //Serial.print("[HTTP] Connecting ...\n");
      
        httpCode = http.POST(payload);        

        if(httpCode > 0) {
          
            //Serial.printf("[HTTP code]: %d\n", httpCode);

            //u8x8.setFont(u8x8_font_7x14B_1x2_f);
            //u8x8.setFont(u8x8_font_chroma48medium8_r);
            
            //u8x8.setCursor(10,6);
            u8x8.print(httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
              
            }
        } else {
            //Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            //u8x8.setFont(u8x8_font_7x14B_1x2_f);
            //u8x8.setFont(u8x8_font_chroma48medium8_r);
              //u8x8.setCursor(10,2);
              u8x8.print(http.errorToString(httpCode).c_str());
       }

        http.end();
        
}
else { // no wifi connection
             //u8x8.setFont(u8x8_font_7x14B_1x2_f);
            //u8x8.setFont(u8x8_font_chroma48medium8_r);
              //u8x8.setCursor(10,2);
              u8x8.print("wifi?");
}
}
