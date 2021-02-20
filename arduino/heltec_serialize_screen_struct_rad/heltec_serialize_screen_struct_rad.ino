#include <ArduinoJson.h>
#include <U8x8lib.h>
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

char abbrev_message[10];

typedef struct {
  char sender[13];
  char message[40]; 
  int timestamp;
} Payload;
Payload theData;

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

u8x8.begin();
  
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
   u8x8.clear();
   u8x8.setCursor(0,0); 
   u8x8.print("Starting...");
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
   
}

void loop() {

  if (Serial.available()) {

    StaticJsonDocument<300> doc;
    DeserializationError err = deserializeJson(doc, Serial);

    if (err == DeserializationError::Ok) 
    {
     String json;
    serializeJson(doc, json);;

    const char* srce = doc["source"];
    const char* msg = doc["msg"];
    int timestamp = 1000;

    memcpy(theData.sender,srce,13);
    memcpy(theData.message,msg,40);
    theData.timestamp = timestamp;

    memcpy(abbrev_message,msg,10);
    
     u8x8.clear();
     u8x8.setCursor(0,0);
     u8x8.print(srce);
     u8x8.setCursor(0,2);
     u8x8.print(abbrev_message);

     rf95.send((uint8_t *)&theData, sizeof(theData));

    Serial.println("Waiting for packet to complete..."); delay(10);
    rf95.waitPacketSent();
   Serial.println("Sent."); delay(10);

        
    } 
    else 
    {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());
  
      // Flush all bytes in the "link" serial port buffer
      while (Serial.available() > 0)
        Serial.read();
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
