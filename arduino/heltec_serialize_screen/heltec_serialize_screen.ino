#include <ArduinoJson.h>
#include <U8x8lib.h>

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

     u8x8.clear();
     u8x8.setCursor(0,0);
     u8x8.print(srce);
     u8x8.setCursor(0,2);
     u8x8.print(msg);
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
