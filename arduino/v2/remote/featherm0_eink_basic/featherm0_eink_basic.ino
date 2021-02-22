#include <SPI.h>

// include library, include base class, make path known
#include <GxEPD.h>

#include <GxGDEW042T2/GxGDEW042T2.h>      // 4.2" b/w

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
//#include <Fonts/FreeMonoBold12pt7b.h>

#include GxEPD_BitmapExamples

GxIO_Class io(SPI, /*CS=*/ 9, /*DC=*/ 10, /*RST=*/ 11); /// MODIFIED FROM ORIGINAL -- DWB
GxEPD_Class display(io, /*RST=*/ 11, /*BUSY=*/ 12); /// MODIFIED FROM ORIGINAL -- DWB


#if defined(_GxGDEP015OC1_H_) || defined(_GxGDEH0154D67_H_)
const uint32_t partial_update_period_s = 1;
const uint32_t full_update_period_s = 6 * 60 * 60;
#elif defined(_GxGDE0213B1_H_) || defined(_GxGDEH029A1_H_) || defined(_GxGDEW042T2_H_)
const uint32_t partial_update_period_s = 2;
const uint32_t full_update_period_s = 1 * 60 * 60;
#endif

// lora



#include <RH_RF95.h>

// Radio pins for feather M0
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0

#define max_messages 8

RH_RF95 rf95(RFM95_CS, RFM95_INT);

typedef struct {
  char sender[13];
  char message[40]; 
  char timestamp[14];
} Payload;

Payload theData[max_messages];

void setup(void)
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  //display.init(115200); // enable diagnostic output on Serial
  display.init(); // disable diagnostic output on Serial
  display.eraseDisplay();
  Serial.println("setup done");
  display.setTextColor(GxEPD_BLACK);
  //display.setRotation(0);
  //display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
  display.update();
  display.setFont(&FreeMonoBold9pt7b);
  display.setRotation(0);
  //display.eraseDisplay();
  //display.update();
  
  //radio
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

  display.eraseDisplay();
  
}

int line = 0;
int msg = 0;

int receive_count = 0;

void loop()
{
 if (rf95.available())
  {

    uint8_t buf[sizeof(Payload)];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      Serial.println("hello");
      theData[msg%max_messages] = *(Payload*)buf;
      int rssi = rf95.lastRssi();
  
      Serial.print(" sender=");
      Serial.print(theData[line%max_messages].sender);

      //display on e-ink
      //String content = "hello"+String(line);
      //display.update();
      //delay(1000);

      display.setFont(&FreeMono9pt7b);
      showPartialUpdate(line,String(receive_count)+": ");
      display.setFont(&FreeMonoBold9pt7b);
      showPartialUpdate(line,String(theData[msg%max_messages].timestamp)+" | "+String(theData[msg%max_messages].sender)); 
      line = line + 1;
      display.setFont(&FreeMono9pt7b);
      showPartialUpdate(line,"-> "+String(theData[msg%max_messages].message).substring(0,20));
      line = line+1; 
      display.update();  
      if (msg%max_messages==0) line=0;
      //delay(1000);
      msg = msg+1;
    }
    
    }
    
}

void showPartialUpdate(int line, String content)
{
  uint16_t box_x = 10;
  uint16_t box_y = 18*line;
  uint16_t box_w = 390;
  uint16_t box_h = 17;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  display.print(content);
  //display.updateWindow(box_x, box_y, box_w, box_h, true);
  //display.update();
}
