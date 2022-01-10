
#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#include <Adafruit_NeoPixel.h>
// 16灯珠4号管脚
Adafruit_NeoPixel rgb_display_4 = Adafruit_NeoPixel(16,4,NEO_GRB + NEO_KHZ800);

// --------- Ultrasonic Sensor preparation
#define echoPin 7 // Echo Pin
#define trigPin 8 // Trigger Pin
#define LEDPin 13 // Onboard LED
int maximumRange = 40; // 最远距离
int minimumRange = 30; // 最近距离
long duration, distance; // Duration used to calculate distance

float ta; // 环境温度
float to, stemp; // 人体温度

int loop_interval = 100; // 定义循环定时器
int readcount = 0;

//取16X16汉字字模 逐行式 顺向高位在前 C51格式 注释前缀// 数据前缀0x 数据后缀, 行后缀,
static const unsigned char PROGMEM str1[] =
{
  0x00,0x00,0x00,0x00,0x01,0x01,0x80,0x00,0x03,0x01,0x80,0x00,0x02,0x01,0x80,0x00,
0x04,0x7D,0xBF,0x00,0x0C,0x03,0xC0,0x00,0x1E,0x07,0xA0,0x00,0x36,0x0D,0xA0,0x00,
0x46,0x19,0x90,0x00,0x06,0x31,0x88,0x00,0x06,0x41,0x86,0x00,0x06,0x9F,0xFF,0x80,
0x07,0x01,0x80,0x00,0x06,0x01,0x80,0x00,0x06,0x01,0x80,0x00,0x00,0x00,0x00,0x00,//体1(宋体26*16)
};

static const unsigned char PROGMEM str2[] =
{
  0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x0C,0x3F,0xF8,0x00,0x04,0x20,0x18,0x00,
0x01,0x3F,0xF8,0x00,0x21,0x20,0x18,0x00,0x19,0x20,0x18,0x00,0x02,0x3F,0xF8,0x00,
0x02,0x00,0x00,0x00,0x04,0xFF,0xFE,0x00,0x0C,0xCC,0x44,0x00,0x38,0xCC,0x44,0x00,
0x08,0xCC,0x44,0x00,0x18,0xCC,0x44,0x00,0x1F,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,//温2(宋体26*16)
};
static const unsigned char PROGMEM str3[] =
{ 
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0C,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0E,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//：3(宋体26*16)
};
static const unsigned char PROGMEM str4[] =
{ 
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE7,0xFF,0x00,0x10,0x44,0x00,0x00,
0x10,0x44,0x00,0x00,0x10,0x67,0xFC,0x00,0x1F,0xE4,0x04,0x00,0x03,0x04,0x04,0x00,
0x13,0x44,0x04,0x00,0x13,0x87,0xFC,0x00,0x13,0x04,0x04,0x00,0x13,0x04,0x00,0x00,
0x13,0xE4,0x00,0x00,0x3C,0x04,0x03,0x00,0x20,0x07,0xFC,0x00,0x00,0x00,0x00,0x00,//距4(宋体26*16)
};
static const unsigned char PROGMEM str5[] =
{ 
  0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x04,0x06,0x00,0x3F,0xFB,0xF8,0x00,
0x03,0x33,0x18,0x00,0x02,0x0E,0x10,0x00,0x02,0x71,0x90,0x00,0x03,0xF3,0xF0,0x00,
0x02,0x0C,0x10,0x00,0x0F,0xFF,0xFE,0x00,0x08,0x30,0x04,0x00,0x08,0x41,0x84,0x00,
0x09,0xFE,0x64,0x00,0x08,0x00,0x44,0x00,0x08,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,//离5
};
static const unsigned char PROGMEM str6[] =
{ 
  0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x08,0x00,0x00,
0x00,0x08,0x00,0x00,0x3F,0xFF,0xFF,0x00,0x00,0x1A,0x00,0x00,0x00,0x12,0x00,0x00,
0x00,0x31,0x00,0x00,0x00,0x20,0x80,0x00,0x00,0x60,0xC0,0x00,0x00,0xF0,0x60,0x00,
0x01,0x0C,0x18,0x00,0x06,0x06,0x0F,0x00,0x38,0x00,0x06,0x00,0x00,0x00,0x00,0x00,//太6
};
static const unsigned char PROGMEM str7[] =
{ 
  0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x0C,0x1F,0xFC,0x00,0x06,0x10,0x00,0x00,
0x00,0x10,0x00,0x00,0x00,0x10,0x02,0x00,0x7E,0x1F,0xFF,0x00,0x04,0x10,0x20,0x00,
0x04,0x10,0x20,0x00,0x04,0x10,0x20,0x00,0x04,0x20,0x20,0x00,0x04,0x40,0x20,0x00,
0x0B,0x80,0x20,0x00,0x30,0xC0,0x00,0x00,0x20,0x3F,0xFF,0x00,0x00,0x00,0x00,0x00,//近7
};
static const unsigned char PROGMEM str8[] =
{ 
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x3F,0xF8,0x00,0x06,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x00,0x73,0xBE,0x00,0x00,0x0C,0x40,0x00,0x3E,0x0C,0x40,0x00,
0x06,0x0C,0x40,0x00,0x06,0x08,0x42,0x00,0x06,0x10,0x42,0x00,0x06,0x60,0x67,0x00,
0x0A,0x80,0x18,0x00,0x71,0xC0,0x00,0x00,0x00,0x3F,0xFF,0x00,0x00,0x00,0x00,0x00,//远8
};
static const unsigned char PROGMEM str9[] =
{ 
  0x00,0x00,0xF7,0xE0,0x20,0x80,0x21,0x00,0x21,0x00,0xF3,0x40,0x25,0x20,0x29,0x20,
0x21,0x00,0x31,0x00,0xC1,0x00,0x01,0x00,//环9(宋体12*12)
};
static const unsigned char PROGMEM str10[] =
{ 
  0x42,0x00,0x4F,0xC0,0x44,0x80,0x5F,0xE0,0xE0,0x00,0x4F,0xC0,0x48,0x40,0x4F,0xC0,
0x48,0x40,0x6F,0xC0,0xC4,0xA0,0x18,0xE0,//境10(宋体12*12)
};
static const unsigned char PROGMEM str11[] =
{ 
  0x0F,0x80,0x88,0x80,0x4F,0x80,0x08,0x80,0x0F,0x80,0x80,0x00,0x5F,0xC0,0x15,0x40,
0x35,0x40,0x55,0x40,0x95,0x40,0x3F,0xE0,//温11(宋体12*12)
};
static const unsigned char PROGMEM str12[] =
{ 
  0x02,0x00,0x7F,0xE0,0x48,0x80,0x7F,0xE0,0x48,0x80,0x4F,0x80,0x40,0x00,0x5F,0xC0,
0x48,0x40,0x44,0x80,0x43,0x00,0x9C,0xE0,//度12(宋体12*12)
};
static const unsigned char PROGMEM str13[] =
{ 
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x30,0x00,0x00,0x00,0x00,0x00,
0x30,0x00,0x30,0x00,0x00,0x00,0x00,0x00,//：13(宋体12*12)
};
static const unsigned char PROGMEM str14[] =
{ 
  0x00,0x00,0x7B,0xE0,0x4A,0x00,0x4A,0x00,0x7B,0xE0,0x12,0x20,0x12,0x20,0x5A,0x20,
0x53,0xE0,0x52,0x00,0x5A,0x00,0xE3,0xE0,//距14
};
static const unsigned char PROGMEM str15[] =
{ 
  0x04,0x00,0xFF,0xE0,0x11,0x00,0x4E,0x40,0x51,0x40,0x7F,0xC0,0x04,0x00,0xFF,0xE0,
0x88,0x20,0x92,0x20,0xBF,0x20,0x81,0x60,//离15
};
static const unsigned char PROGMEM str16[] =
{ 
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x04,0x00,0x00,
0x00,0x04,0x00,0x00,0x00,0x04,0x00,0x00,0x03,0x84,0x00,0x00,0x03,0x07,0xFC,0x00,
0x03,0x04,0x00,0x00,0x03,0x04,0x00,0x00,0x03,0x04,0x00,0x00,0x03,0x04,0x00,0x00,
0x03,0x04,0x00,0x00,0x03,0x04,0x06,0x00,0x3C,0xFB,0xF8,0x00,0x00,0x00,0x00,0x00,//正16(宋体26*16)
};
static const unsigned char PROGMEM str17[] =
{ 
  0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x10,0x00,0x00,0x1F,0xBF,0xFE,0x00,
0x00,0x60,0x00,0x00,0x00,0xC1,0x00,0x00,0x01,0x81,0x00,0x00,0x07,0x01,0x00,0x00,
0x06,0x7F,0xFE,0x00,0x1E,0x01,0x00,0x00,0x66,0x01,0x00,0x00,0x06,0x01,0x00,0x00,
0x06,0x01,0x00,0x00,0x07,0xFF,0xFF,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//在17(宋体26*16)
};
static const unsigned char PROGMEM str18[] =
{ 
  0x00,0x00,0x00,0x00,0x10,0x00,0x02,0x00,0x0C,0xFF,0x02,0x00,0x04,0x81,0x22,0x00,
0x00,0x99,0x32,0x00,0x31,0x99,0x32,0x00,0x1A,0x99,0x32,0x00,0x02,0x99,0x32,0x00,
0x04,0x99,0x32,0x00,0x04,0x91,0x32,0x00,0x28,0x91,0x32,0x00,0x18,0x38,0x32,0x00,
0x18,0x26,0x02,0x00,0x18,0xC3,0x02,0x00,0x1B,0x01,0x1E,0x00,0x04,0x00,0x00,0x00,//测18(宋体26*16)
};
static const unsigned char PROGMEM str19[] =
{ 
  0x00,0x00,0x00,0x00,0x03,0xFF,0xF0,0x00,0x02,0x00,0x30,0x00,0x03,0xFF,0xF0,0x00,
0x03,0xFF,0xF0,0x00,0x02,0x00,0x20,0x00,0x3F,0xFF,0xFF,0x80,0x03,0xF3,0xF0,0x00,
0x02,0x0C,0x10,0x00,0x03,0xFF,0xF0,0x00,0x02,0x0C,0x10,0x00,0x03,0xFF,0xF0,0x00,
0x07,0xFF,0xF8,0x00,0x00,0x0C,0x00,0x00,0x3F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,//量19
};

void setup() {
  pinMode(trigPin, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(1000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  rgb_display_4.begin();
}

void loop() {
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  // 读取人体距离
  distance = duration / 58.2;


  // 读取体温温度补偿和环境温度
  //to = 8 + round(mlx.readObjectTempC() * 10) * .1;
  to = round(mlx.readObjectTempC() * 10) * .1;
  ta = round(mlx.readAmbientTempC() * 10) * .1;

  // display on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.drawBitmap(0, 39, str14, 12, 12, 1); //距离：
  display.drawBitmap(12, 39, str15, 12, 12, 1); 
  display.drawBitmap(24, 39, str13, 12, 12, 1); 
  display.setCursor(36, 41);
  display.print(String(distance) + "cm");
  
  display.drawBitmap(0, 52, str9, 12, 12, 1); //环境温度：
  display.drawBitmap(12, 52, str10, 12, 12, 1); 
  display.drawBitmap(24, 52, str11, 12, 12, 1);   
  display.drawBitmap(36, 52, str12, 12, 12, 1);  
  display.drawBitmap(48, 52, str13, 12, 12, 1); 
  display.setCursor(60, 54);
  display.print(String(ta) + "C");
  
  display.setTextSize(2);
  display.setCursor(0, 0);
  if (distance > maximumRange) {
    display.drawBitmap(0, 0, str4, 26, 16, 1); //距离太远
    display.drawBitmap(26, 0, str5, 26, 16, 1); 
    display.drawBitmap(52, 0, str6, 26, 16, 1); 
    display.drawBitmap(78, 0, str8, 26, 16, 1); 
  }
  if (distance < minimumRange) {
    display.drawBitmap(0, 0, str4, 26, 16, 1); //距离太近
    display.drawBitmap(26, 0, str5, 26, 16, 1); 
    display.drawBitmap(52, 0, str6, 26, 16, 1); 
    display.drawBitmap(78, 0, str7, 26, 16, 1); 
  }
  if ((distance >= minimumRange) && (distance <= maximumRange)) {
    if (readcount == 5) {   // 判断测温次数变量
      to = stemp / 5;       // 获取5次测温平均数
      display.drawBitmap(0, 0, str1, 26, 16, 1); //体温：
      display.drawBitmap(26, 0, str2, 26, 16, 1); 
      display.drawBitmap(52, 0, str3, 26, 16, 1); 
      display.setTextSize(2);
      display.setCursor(20, 18);
      display.print(String(to).substring(0, 4) + "C");
      display.display();
      readcount = 0;
      stemp = 0;
      if (to >= 37.5) {
        play_alert();
      } else {
        play_ok();
      }
      loop_interval = 5000; // wait for 5 seconds
    } else {
      display.drawBitmap(0, 0, str16, 26, 16, 1); //正在测量
      display.drawBitmap(26, 0, str17, 26, 16, 1); 
      display.drawBitmap(52, 0, str18, 26, 16, 1); 
      display.drawBitmap(78, 0, str19, 26, 16, 1); 
      stemp = stemp + to;
      readcount++;
      loop_interval = 200;      // 取5此体温，等待1秒钟
    }
  } else {                      // 人体超出范围重置定时器和读取次数以及5此体温之和
    loop_interval = 100;
    readcount = 0;
    stemp = 0;
  }
  display.display();
  delay(loop_interval);  //执行循环定时

  pinMode(echoPin, OUTPUT);
  digitalWrite(echoPin, 0);
  delayMicroseconds(200);
}

void play_ok() {  // 37.5以内
  for (int i = 1; i <= 16; i = i + (1)) {
    rgb_display_4.setPixelColor((i)-1, 0x33cc00);
    rgb_display_4.show();
  }
  tone(3, 600, 200);
  delay(100);
  for (int i = 1; i <= 16; i = i + (1)) {
    rgb_display_4.setPixelColor((i)-1, 0x3366ff);
    rgb_display_4.show();
  }
  tone(3, 750, 200);
  delay(100);
  for (int i = 1; i <= 16; i = i + (1)) {
    rgb_display_4.setPixelColor((i)-1, 0x33cc00);
    rgb_display_4.show();
  }
  tone(3, 1000, 200);
  delay(100);
  for (int i = 1; i <= 16; i = i + (1)) {
    rgb_display_4.setPixelColor((i)-1, 0x000000);
    rgb_display_4.show();
  }
  noTone(3);  
}

void play_alert() { // 高于37.5报警
  tone(3, 1000, 1000);
  for (int i = 1; i <= 16; i = i + (1)) {
    rgb_display_4.setPixelColor((i)-1, 0xcc0000);
    rgb_display_4.show();
  }
  delay(1000);
  noTone(3);
  for (int i = 1; i <= 16; i = i + (1)) {
    rgb_display_4.setPixelColor((i)-1, 0x000000);
    rgb_display_4.show();
  }
  delay(1000);
  tone(3, 1000, 1000);
  for (int i = 1; i <= 16; i = i + (1)) {
    rgb_display_4.setPixelColor((i)-1, 0xcc0000);
    rgb_display_4.show();
  }
  delay(1000);
  noTone(3);
  for (int i = 1; i <= 16; i = i + (1)) {
    rgb_display_4.setPixelColor((i)-1, 0x000000);
    rgb_display_4.show();
  }
  delay(1000);
  tone(3, 1000, 1000);
  for (int i = 1; i <= 16; i = i + (1)) {
    rgb_display_4.setPixelColor((i)-1, 0xcc0000);
    rgb_display_4.show();
  }
  delay(1000);
  noTone(3);
  for (int i = 1; i <= 16; i = i + (1)) {
    rgb_display_4.setPixelColor((i)-1, 0x000000);
    rgb_display_4.show();
  }
}
