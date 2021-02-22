
// include library, include base class, make path known
#include <GxEPD.h>

#include <GxGDEW042T2/GxGDEW042T2.h>      // 4.2" b/w

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

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
}

int line = 0;
void loop()
{
    //display.update();
    String content = "hello "+String(line);
    showPartialUpdate(line, content);
    line = line+1;
    //if (line > 19) line=0;
    delay(300);
}

void print02d(uint32_t d)
{
  if (d < 10) display.print("0");
  display.print(d);
}

//#if !defined(__AVR)

void showPartialUpdate(int line, String content)
{
  uint16_t box_x = 10;
  uint16_t box_y = 18*(line%18);
  uint16_t box_w = 170;
  uint16_t box_h = 17;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  display.print(content); 
  display.updateWindow(box_x, box_y, box_w, box_h, true);
  //display.update();
}
/*
#else

void drawCallback()
{
  uint16_t box_x = 10;
  uint16_t box_y = 15;
  uint16_t box_w = 170;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  display.print(days); display.print("d "); print02d(hours); display.print(":"); print02d(minutes); display.print(":"); print02d(seconds);
}

void showPartialUpdate_AVR()
{
  uint16_t box_x = 10;
  uint16_t box_y = 15;
  uint16_t box_w = 170;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + 14;
  display.drawPagedToWindow(drawCallback, box_x, box_y, box_w, box_h);
}

#endif
*/
