

#include <Arduino.h>
#include <U8g2lib.h>

//#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
//#endif
//#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
//#endif

#include <RH_RF95.h>

// Radio pins for mothbot
#define RFM95_CS 8
#define RFM95_RST 7
#define RFM95_INT 2

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // SSD1306 128x32

#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

typedef struct {
  char sender[13];
  char message[40]; 
  char timestamp[14];
} Payload;
Payload theData;


void setup(void) {
  u8g2.begin();

  Serial.begin(9600);

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(23, false);
  Serial.print("max buff:");
  Serial.println(RH_RF95_MAX_MESSAGE_LEN);

  
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_nokiafc22_tf);
  u8g2.drawStr(0,10,"Starting up!");  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000);  

}

int index = 0;

void loop(void) {
  

 if (rf95.available())
  {

    uint8_t buf[sizeof(Payload)];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      Serial.println("hello");
      theData = *(Payload*)buf;

      int rssi = rf95.lastRssi();
  
      Serial.print(" sender=");
      Serial.print(theData.sender);
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.drawStr(0,10,theData.timestamp);  // write something to the internal memory
      u8g2.drawStr(0,20,theData.sender);  // write something to the internal memory
      u8g2.drawStr(0,30,theData.message);  // write something to the internal memory
      u8g2.sendBuffer();   

    }
    
    }
  }
