#include <ArduinoJson.h>
#include <SPI.h>
#include <RH_RF95.h>

 
// for feather m0  
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3



// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

char abbrev_message[10];

typedef struct {
  char sender[13];
  char message[100]; 
  int timestamp;
} Payload;
Payload theData;


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);


   if (!setupLoRa()) {
    while (1)
      ;
   }
   
   
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
    memcpy(theData.message,msg,100);
    theData.timestamp = timestamp;

    memcpy(abbrev_message,msg,10);
  

     rf95.send((const void*)&theData, sizeof(theData));

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
