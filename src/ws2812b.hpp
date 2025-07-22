#pragma once
#include <Arduino.h>


static const uint8_t PROGMEM __GAMMA8_TABLE[256] = 
{
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,   2,   3,
  3,   3,   3,   3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   5,   6,
  6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,   10,  10,  10,
  11,  11,  11,  12,  12,  13,  13,  13,  14,  14,  15,  15,  16,  16,  17,
  17,  18,  18,  19,  19,  20,  20,  21,  21,  22,  22,  23,  24,  24,  25,
  25,  26,  27,  27,  28,  29,  29,  30,  31,  31,  32,  33,  34,  34,  35,
  36,  37,  38,  38,  39,  40,  41,  42,  42,  43,  44,  45,  46,  47,  48,
  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
  64,  65,  66,  68,  69,  70,  71,  72,  73,  75,  76,  77,  78,  80,  81,
  82,  84,  85,  86,  88,  89,  90,  92,  93,  94,  96,  97,  99,  100, 102,
  103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120, 122, 124, 125,
  127, 129, 130, 132, 134, 136, 137, 139, 141, 143, 145, 146, 148, 150, 152,
  154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182,
  184, 186, 188, 191, 193, 195, 197, 199, 202, 204, 206, 209, 211, 213, 215,
  218, 220, 223, 225, 227, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252,
  255
};

namespace WS2812B
{
  struct __attribute__((packed)) LED 
  {
    LED(uint32_t color);
    LED(uint8_t r, uint8_t g, uint8_t b);
    LED();
    uint8_t g;
    uint8_t r;
    uint8_t b;

    operator uint32_t();
    uint8_t& operator[](uint8_t index);
    LED& operator=(uint32_t color);
    LED& operator=(const LED& other);

    bool operator==(const LED& other) const;
    bool operator==(uint32_t color) const;
    bool operator!=(const LED& other) const;
    bool operator!=(uint32_t color) const;
  };

  using Color = LED;

  bool begin(uint8_t pin);

  Color& gamma32(Color& color);
  uint32_t gamma32(uint32_t color);

  void clear(LED* leds, uint16_t len);

  void fill(LED* leds, uint16_t len, uint32_t color);

  void fill(LED* leds, uint16_t len, const Color& color);

  void fill(LED* leds, uint16_t len, uint8_t r, uint8_t g, uint8_t b);

  void fillFromTo(LED* leds, uint16_t len, uint32_t color, uint16_t from, uint16_t to);

  void fillFromTo(LED* leds, uint16_t len, const Color& color, uint16_t from, uint16_t to);

  void fillFromTo(LED* leds, uint16_t len, uint8_t r, uint8_t g, uint8_t b, uint16_t from, uint16_t to);

  LED hsv(uint16_t hue, uint8_t sat = 255u, uint8_t val = 255u);

  void show(LED* leds, uint16_t len, uint8_t pin, uint8_t bright = 255);

  class StripGroup;

  class Strip
  {
  public:
    Strip();
    Strip(LED* leds, uint16_t len, uint8_t pin, bool reverse = false);
    bool begin();
    void changeLEDsConfig(LED* leds, uint16_t len);
    void clear();
    void fill(uint32_t color);
    void fillFromTo(uint32_t color, uint16_t from, uint16_t to);
    void fill(uint8_t r, uint8_t g, uint8_t b);
    void fillFromTo(uint8_t r, uint8_t g, uint8_t b, uint16_t from, uint16_t to);
    void fill(const Color& color);
    void fillFromTo(const Color& color, uint16_t from, uint16_t to);
    uint8_t getBrightness() const;
    uint8_t getPin() const;
    Color getPixelColor(uint16_t n) const;
    bool isReverse() const;
    uint16_t numPixels() const;
    void setBrightness(uint8_t b);
    void setPin(uint8_t pin);
    void setPixelColor(uint16_t n, uint32_t color);
    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    void setPixelColor(uint16_t n, Color color);
    void setReverse(bool);
    void show();
    LED& operator[](uint16_t led);

  private:
    bool is_begin;
    LED* leds;
    uint16_t count;
    uint8_t pin;
    bool reverse;
    uint32_t timer;

  public:
    uint8_t bright;

    friend StripGroup;
  };

  class StripGroup
  {
  public:
    StripGroup();
    StripGroup(Strip* strips, uint16_t len);
    bool begin();
    void changeStripsConfig(Strip* strips, uint16_t len);
    void clear();
    void fill(uint32_t color);
    void fillFromTo(uint32_t color, uint32_t from, uint32_t to);
    void fill(uint8_t r, uint8_t g, uint8_t b);
    void fillFromTo(uint8_t r, uint8_t g, uint8_t b, uint32_t from, uint32_t to);
    void fill(const Color& led_color);
    void fillFromTo(const Color& led_color, uint32_t from, uint32_t to);
    uint8_t getBrightness() const;
    uint32_t getPixelColor(uint32_t n) const;
    uint16_t getStripByLED(uint32_t led) const;
    Strip* getStripPtr(uint16_t index) const;
    uint32_t numPixels() const;
    uint16_t numStrips() const;
    void setPixelColor(uint32_t n, uint32_t color);
    void setPixelColor(uint32_t n, uint8_t r, uint8_t g, uint8_t b);
    void setPixelColor(uint32_t n, Color color);
    void setBrightness(uint8_t b);
    void show();
    void show(uint16_t strip);
    void show(bool* strip_update_list, bool reset_list = true);
    LED& operator[](uint32_t led);
    
  private:
    void calcLEDsCount();
    bool isBegin() const;
    LED& getLedReference(uint32_t n) const;
    Strip* strips;
    uint16_t strip_count;
    uint32_t led_count;
  
  public:
    uint8_t bright;
  };

  // struct Pos
  // {
  //   Pos();
  //   Pos(int16_t x, int16_t y);
  //   int16_t x;
  //   int16_t y;
  // };

  // struct Size
  // {
  //   Size();
  //   Size(uint16_t w, uint16_t h);
  //   union
  //   {
  //     uint16_t w;
  //     uint16_t width;
  //   };
  //   union
  //   {
  //     uint16_t h;
  //     uint16_t height;
  //   };
  // };

  // class Rect
  // {
  // public:
  //   enum connection_t : uint8_t
  //   {
  //     LINEAR,
  //     CROSS
  //   };

  //   enum orientation_t : uint8_t
  //   {
  //     REVERSE,
  //     NORMAL
  //   };

  //   Rect();
  //   Rect(LED* led_matrix, uint16_t width, uint16_t height, uint8_t pin, connection_t connection_type = LINEAR);

  //   uint8_t bright;
  //   bool begin();
  //   void changeLEDsConfig(LED* led_matrix, uint16_t width, uint16_t height);
  //   void setConnectionType(Rect::connection_t);
  //   void setXOrientation(Rect::orientation_t);
  //   void setYOrientation(Rect::orientation_t);
  //   Rect::connection_t getConnectionType() const;
  //   Rect::orientation_t getXOrientation() const;
  //   Rect::orientation_t getYOrientation() const;
  //   uint16_t getWidth() const;
  //   uint16_t getHeight() const;
  //   uint16_t numLeds() const;
  //   void drawRectangle(Pos pos, Size size, uint32_t color, bool fill = false);
  //   void drawRectangle(Pos pos, uint16_t width, uint16_t height, uint32_t color, bool fill = false);
  //   void drawRectangle(int16_t x, int16_t y, Size size, uint32_t color, bool fill = false);
  //   void drawRectangle(int16_t x, int16_t y, uint16_t width, uint16_t height, uint32_t color, bool fill = false);
  //   void drawLine(Pos begin, Pos end, uint32_t color);
  //   void drawLine(int16_t x_begin, int16_t y_begin, Pos end, uint32_t color);
  //   void drawLine(Pos begin, int16_t x_end, int16_t y_end, uint32_t color);
  //   void drawLine(int16_t x_begin, int16_t y_begin, int16_t x_end, int16_t y_end, uint32_t color);
  //   void drawCircle(Pos center, uint16_t r, uint32_t color, bool fill = false);
  //   void drawCircle(int16_t x_center, int16_t y_center, uint16_t r, uint32_t color, bool fill = false);
  //   void fill(uint32_t color);
  //   void fill(uint8_t r, uint8_t g, uint8_t b);
  //   void fill(const Color& color);
  //   void clear();
  //   LED& operator[](Pos pos);
  //   LED& operator[](uint16_t);
  //   void show();

  // private:
  //   void calcLedCount();
  //   bool is_begin;
  //   uint8_t pin;
  //   orientation_t x_ori;
  //   orientation_t y_ori;
  //   Size dim;
  //   LED* led_matrix;
  //   uint16_t led_count;
  //   connection_t con;
  // };

}
