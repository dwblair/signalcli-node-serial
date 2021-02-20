
#include <U8x8lib.h>
#include <ArduinoJson.h> //https://arduinojson.org/v6/doc/installation/
#include <SPI.h>
#include <RH_RF95.h>

// heltec wifi lora 32 v2
#define RFM95_CS 18
#define RFM95_RST 14
#define RFM95_INT 26

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

void setup() {

  u8x8.begin();
  
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
   u8x8.clear();
   u8x8.setCursor(0,0); 
   u8x8.print("Starting...");
   delay(1000);
   
  
  Serial.begin(9600);
  Serial.println();

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
   
}

void loop() {


DynamicJsonDocument radiodoc(1024);
  
  if (Serial.available() > 0)
  {
    String s = Serial.readStringUntil("\n");   // Until CR (Carriage Return)
    //s.replace("#", "");
    Serial.println(s);

    DeserializationError error = deserializeJson(radiodoc, s);
   
    if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
   }

   
        //Form the JSON:
        DynamicJsonDocument doc(241);

        char radioJSON[241];
        
        //Serial.println(post_url);

        doc["public_key"] = feed_pubkey;
        doc["private_key"] = feed_privkey;
        doc["co2"] =  co2;
        doc["tempC"]=temp;
        doc["humidity"]=humid;
        doc["mic"]=0.;
        doc["auxPressure"]=aux_press;
        doc["auxTempC"]=aux_temp;
        doc["aux001"]=0.;
        doc["aux002"]=0.;
        doc["log"]=0.;
         
        String json;
        serializeJson(doc, json);
        serializeJson(doc, Serial);
        serializeJson(doc, radioJSON,241);
        
        Serial.println("\n");
        
        // Post over LoRa
        Serial.println("Sending..."); delay(10);
        //char radiopacket[20] = "Hello World #      ";
        Serial.println(radioJSON);
        
        rf95.send((uint8_t *)radioJSON, 241);

        Serial.println("Waiting for packet to complete..."); delay(10);
        rf95.waitPacketSent();
        // Now wait for a reply
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
      
        Serial.println("Waiting for reply..."); delay(10);
        if (rf95.waitAvailableTimeout(1000))
        { 
          // Should be a reply message for us now   
          if (rf95.recv(buf, &len))
         {
            Serial.print("Got reply: ");
            Serial.println((char*)buf);
            Serial.print("RSSI: ");
            Serial.println(rf95.lastRssi(), DEC);   
            u8x8.setFont(u8x8_font_chroma48medium8_r);
            u8x8.setCursor(9,2);
            u8x8.print("LoRa OK");
            u8x8.setCursor(9,3);
            u8x8.print("R:");
            //u8x8.setCursor(9,4);
            u8x8.print(rf95.lastRssi());
          }
          else
          {
            Serial.println("Receive failed");
             u8x8.setFont(u8x8_font_chroma48medium8_r);
            u8x8.setCursor(9,2);
            u8x8.print("L:fail?");
          }
        }
        else
        {
             u8x8.setFont(u8x8_font_chroma48medium8_r);
            u8x8.setCursor(9,2);
            u8x8.print("L:range?");
        }
  
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
