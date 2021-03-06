#include <U8g2lib.h>

#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>
//#include <ArduinoJson.h> //https://arduinojson.org/v6/doc/installation/

// Radio pins for mothbot
#define RFM95_CS 8
#define RFM95_RST 7
#define RFM95_INT 2

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // pin remapping with ESP8266 HW I2C

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
//#define LED 5

typedef struct {
  char sender[13];
  char message[100]; 
  int timestamp;
} Payload;
Payload theData;



void setup()
{

  u8g2.begin();
    u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.drawStr(0,10,"Hello World!");  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
   delay(1000);
  
Serial.begin(9600);
  //while (!Serial) {
   // delay(1);
  //}
  delay(100);
 
  
  //pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);


   delay(1000);
   
  
  
  //Serial.println("Feather LoRa RX Test!");

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

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  Serial.print("max buff:");
  Serial.println(RH_RF95_MAX_MESSAGE_LEN);
  
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[sizeof(Payload)];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {

 
      
      //digitalWrite(LED, HIGH);
      //RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
       Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);

     theData = *(Payload*)buf;
      Serial.print(" sender=");
      Serial.print(theData.sender);
      Serial.print("message=");
      Serial.print(theData.message);
      
      
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      //digitalWrite(LED, LOW);
      
      
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}
