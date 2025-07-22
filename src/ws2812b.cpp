#include "ws2812b.hpp"

// ########################################### WS2812B #################################################################

namespace WS2812B
{
  LED::LED(uint32_t color) : g{(uint8_t(color >> 8))}, r{(uint8_t)(color >> 16)}, b{(uint8_t)color} {}

  LED::LED(uint8_t r, uint8_t g, uint8_t b) : g{g}, r{r}, b{b} {}

  LED::LED() : LED(0u) {}

  uint8_t& LED::operator[](uint8_t index)
  {
    static uint8_t void_part = 0u;
    index %= 3;
    switch (index)
    {
      case 0: return r;
      case 1: return g;
      case 2: return b;
      default: return void_part;
    }
    return void_part;
  }

  LED& LED::operator=(const LED& other)
  {
    r = other.r, g = other.g, b = other.b;
    return *this;
  }

  LED& LED::operator=(uint32_t c)
  {
    r = (uint8_t)(c >> 16), g = (uint8_t)(c >> 8), b = (uint8_t)c; 
    return *this;
  }

  bool LED::operator==(const LED& other) const
  {
    return r == other.r && g == other.g && b == other.b;
  }

  bool LED::operator==(uint32_t c) const
  {
    return r == (uint8_t)(c >> 16) && g == (uint8_t)(c >> 8) && b == (uint8_t)c;
  }

  bool LED::operator!=(const LED& other) const
  {
    return r != other.r || g != other.g || b != other.b;
  }

  bool LED::operator!=(uint32_t c) const
  {
    return r != (uint8_t)(c >> 16) || g != (uint8_t)(c >> 8) || b != uint8_t(c);
  }

  LED::operator uint32_t()
  {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
  }

  bool begin(uint8_t pin)
  {
    if (pin < 255)
    {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
      return 1;
    }
    return 0;
  }

  static uint8_t gamma8(uint8_t i)
  {
    return pgm_read_byte(&__GAMMA8_TABLE[i]);
  }

  Color& gamma32(Color& color)
  {
    for (uint8_t i = 0; i < 3; ++i) color[i] = gamma8(color[i]);
    return color;
  }

  uint32_t gamma32(uint32_t color)
  {
    uint8_t* b = (uint8_t*)&color;
    for (uint8_t i = 0; i < 4; ++i) b[i] = gamma8(b[i]);
    return color;
  }

  void fill(LED* leds, uint16_t len, uint32_t color)
  {
    if (leds == nullptr) return;
    for (uint16_t i = 0; i < len; ++i) leds[i] = color;
  }

  void fill(LED* leds, uint16_t len, const Color& color)
  {
    if (leds == nullptr) return;
    for (uint16_t i = 0; i < len; ++i) leds[i] = color;
  }

  void fill(LED* leds, uint16_t len, uint8_t r, uint8_t g, uint8_t b)
  {
    if (leds == nullptr) return;
    for (uint16_t i = 0; i < len; ++i) leds[i].r = r, leds[i].g = g, leds[i].b = b;
  }

  void fillFromTo(LED* leds, uint16_t len, uint32_t color, uint16_t from, uint16_t to)
  {
    if (leds == nullptr || from > to || to >= len) return;
    for (uint16_t i = from; i <= to; ++i) leds[i] = color;
  }

  void fillFromTo(LED* leds, uint16_t len, const Color& color, uint16_t from, uint16_t to)
  {
    if (leds == nullptr || from > to || to >= len) return;
    for (uint16_t i = from; i <= to; ++i) leds[i] = color;
  }

  void fillFromTo(LED* leds, uint16_t len, uint8_t r, uint8_t g, uint8_t b, uint16_t from, uint16_t to)
  {
    if (leds == nullptr || from > to || to >= len) return;
    for (uint16_t i = from; i <= to; ++i) leds[i].r = r, leds[i].g = g, leds[i].b = b;
  }

  void clear(LED* leds, uint16_t len)
  {
    if (leds == nullptr) return;
    memset((uint8_t*)leds, 0, len * 3);
  }

  Color hsv(uint16_t hue, uint8_t sat, uint8_t val)
  {
    Color rgb;
    hue = (hue * 1530L + 32768) / 65536L;

    if (hue < 510) { // Red to Green-1
      rgb.b = 0;          // b
      if (hue < 255) { //   Red to Yellow-1
        rgb.r = 255;        // r
        rgb.g = hue;       //     g = 0 to 254
      } else {         //   Yellow to Green-1
        rgb.r = 510 - hue; //     r = 255 to 1
        rgb.g = 255;        // g
      }
    } else if (hue < 1020) { // Green to Blue-1
      rgb.r = 0;            // r
      if (hue < 765) { //   Green to Cyan-1
        rgb.g = 255;        // g
        rgb.b = hue - 510;  //     b = 0 to 254
      } else {          //   Cyan to Blue-1
        rgb.g = 1020 - hue; //     g = 255 to 1
        rgb.b = 255;        // b
      }
    } else if (hue < 1530) { // Blue to Red-1
      rgb.g = 0;            // g
      if (hue < 1275) { //   Blue to Magenta-1
        rgb.r = hue - 1020; //     r = 0 to 254
        rgb.b = 255;        // b
      } else { //   Magenta to Red-1
        rgb.r = 255;        // r
        rgb.b = 1530 - hue; //     b = 255 to 1
      }
    } else { // Last 0.5 Red (quicker than % operator)
      rgb.r = 255;        // r
      rgb.g = rgb.b = 0;      // g = b = 0
    }

    // Apply saturation and value to R,G,B, pack into 32-bit result:
    uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
    uint16_t s1 = 1 + sat;  // 1 to 256; same reason
    uint8_t s2 = 255 - sat; // 255 to 0
    
    rgb.r = (((rgb.r * s1) >> 8) + s2) * v1 >> 8;
    rgb.g = (((rgb.g * s1) >> 8) + s2) * v1 >> 8;
    rgb.b = (((rgb.b * s1) >> 8) + s2) * v1 >> 8;

    return rgb;
  }

}

// ############################################################################################################################

static  WS2812B::LED void_led{0};

// ########################################### WS2812B_STRIP ##################################################################


namespace WS2812B
{
  extern void _extern_timer_show(LED* leds, uint16_t len, uint8_t pin, uint8_t bright, uint32_t& timer);

  Strip::Strip(LED* leds, uint16_t len, uint8_t pin, bool reverse) 
  : is_begin{0}, 
    leds{leds}, 
    count{len}, 
    pin{pin}, 
    reverse{reverse}, 
    timer{0}, 
    bright{255} 
  {}

  Strip::Strip() : Strip(nullptr, 0u, 255u, 0) {}

  bool Strip::begin()
  {
    is_begin = WS2812B::begin(pin);
    return is_begin;
  }

  void Strip::setPin(uint8_t p)
  {
    pin = p;
    begin();
  }

  uint8_t Strip::getPin() const
  {
    return pin;
  }

  void Strip::changeLEDsConfig(LED* _leds, uint16_t len)
  {
    leds = _leds;
    count = len;
  }

  LED& Strip::operator[](uint16_t led)
  {
    if (leds == nullptr || led >= count) return void_led;
    if (reverse) return leds[count - 1 - led];
    return leds[led];
  } 

  uint16_t Strip::numPixels() const
  {
    return count;
  }

  bool Strip::isReverse() const
  {
    return reverse;
  }

  void Strip::setReverse(bool r)
  {
    reverse = r;
  }

  void Strip::fill(uint32_t color)
  {
    WS2812B::fill(leds, count, color);
  }

  void Strip::fillFromTo(uint32_t color, uint16_t from, uint16_t to)
  {
    if (!reverse) return WS2812B::fillFromTo(leds, count, color, from, to);
    if (from > to || to >= count) return;
    for (uint16_t i = count - 1 - from - to; i < count - from; ++i) leds[i] = color;
  }

  void Strip::fill(uint8_t r, uint8_t g, uint8_t b)
  {
    WS2812B::fill(leds, count, r, g, b);
  }

  void Strip::fillFromTo(uint8_t r, uint8_t g, uint8_t b, uint16_t from, uint16_t to)
  {
    if (!reverse) return WS2812B::fillFromTo(leds, count, r, g, b, from, to);
    if (from > to || to >= count) return;
    for (uint16_t i = count - 1 - from - to; i < count - from; ++i) leds[i].r = r, leds[i].g = g, leds[i].b = b;
  }

  void Strip::fillFromTo(const Color& color, uint16_t from, uint16_t to)
  {
    if (!reverse) return WS2812B::fillFromTo(leds, count, color, from, to);
    if (from > to || to >= count) return;
    for (uint16_t i = count - 1 - from - to; i < count - from; ++i) leds[i] = color;
  }

  void Strip::fill(const Color& color)
  {
    WS2812B::fill(leds, count, color);
  }

  void Strip::clear()
  {
    WS2812B::clear(leds, count);
  }

  void Strip::show()
  {
    if (is_begin) WS2812B::_extern_timer_show(leds, count, pin, bright, timer);
  }

  uint8_t Strip::getBrightness() const
  {
    return bright;
  }

  Color Strip::getPixelColor(uint16_t n) const
  {
    if (n >= count) return 0;
    return leds[n];
  }

  void Strip::setBrightness(uint8_t b)
  {
    bright = b;
  }

  void Strip::setPixelColor(uint16_t n, uint32_t color)
  {
    if (n >= count) return;
    leds[n] = color;
  }

  void Strip::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
  {
    if (n >= count) return;
    leds[n].r = r; leds[n].g = g; leds[n].b = b;
  }

  void Strip::setPixelColor(uint16_t n, Color color)
  {
    if (n >= count) return;
    leds[n] = color;
  }

}

// #############################################################################################################################



// ############################################ WS2812B_STRIP_GROUP ############################################################

namespace WS2812B
{
  StripGroup::StripGroup(Strip* strips, uint16_t len) : strips{strips}, strip_count{len}, bright{255}
  {
    calcLEDsCount();
  }

  StripGroup::StripGroup() : StripGroup(nullptr, 0) {}

  bool StripGroup::begin()
  {
    for (uint16_t i = 0; i < strip_count; ++i) strips[i].begin();
    return isBegin();
  }

  bool StripGroup::isBegin() const
  {
    if (strip_count == 0) return 0;
    for (uint16_t i = 0; i < strip_count; ++i)
    {
      if (!strips[i].is_begin) return 0;
    }
    return 1;
  }

  void StripGroup::calcLEDsCount()
  {
    led_count = 0u;
    for (uint16_t i = 0; i < strip_count; ++i) led_count += strips[i].count;
  }

  void StripGroup::changeStripsConfig(Strip* _strips, uint16_t _len)
  {
    strips = _strips;
    strip_count = _len;
    calcLEDsCount();
  }

  uint32_t StripGroup::numPixels() const
  {
    return led_count;
  }

  LED& StripGroup::operator[](uint32_t led)
  {
    if (strips == nullptr || led >= led_count) return void_led;
    return getLedReference(led);
  }

  void StripGroup::fill(uint32_t color)
  {
    if (strips == nullptr) return;
    for (uint16_t i = 0; i < strip_count; ++i) 
    {
      strips[i].fill(color);
    }
  }

  void StripGroup::fillFromTo(uint32_t color, uint32_t from, uint32_t to)
  {
    if (from > to || to >= led_count) return;
    for (uint32_t i = from; i <= to; ++i)
    {
      getLedReference(i) = color;
    }
  }

  void StripGroup::fill(uint8_t r, uint8_t g, uint8_t b)
  {
    if (strips == nullptr) return;
    for (uint16_t i = 0; i < strip_count; ++i) strips[i].fill(r, g, b);
  }

  void StripGroup::fillFromTo(uint8_t r, uint8_t g, uint8_t b, uint32_t from, uint32_t to)
  {
    if (from > to || to >= led_count) return;
    for (uint32_t i = from; i <= to; ++i)
    {
      getLedReference(i) = {r, g, b};
    }
  }

  void StripGroup::fill(const Color& led_color)
  {
    if (strips == nullptr) return;
    for (uint16_t i = 0; i < strip_count; ++i) strips[i].fill(led_color);
  }

  void StripGroup::fillFromTo(const Color& led_color, uint32_t from, uint32_t to)
  {
    if (from > to || to >= led_count) return;
    for (uint32_t i = from; i <= to; ++i)
    {
      getLedReference(i) = led_color;
    }
  }

  uint16_t StripGroup::getStripByLED(uint32_t led) const
  {
    if (strips == nullptr) return 0;
    for (uint16_t i = 0; i < strip_count; ++i)
    {
      if (led < strips[i].count) return i;
      led -= strips[i].count;
    } 
    return 0;
  }

  void StripGroup::show()
  {
    if (strips == nullptr) return;
    for (uint16_t i = 0; i < strip_count; ++i)
    {
      if (strips[i].is_begin) WS2812B::show(strips[i].leds, strips[i].count, strips[i].pin, bright);
    }
  }

  void StripGroup::show(uint16_t strip)
  {
    if (strips == nullptr || strip >= strip_count || !strips[strip].is_begin) return; 
    WS2812B::_extern_timer_show(strips[strip].leds, strips[strip].count, strips[strip].pin, bright, strips[strip].timer);
  }

  void StripGroup::show(bool* strip_update_list, bool reset_list)
  {
    if (!isBegin()) return;
    for (uint16_t i = 0; i < strip_count; ++i)
    {
      if (strip_update_list[i]) WS2812B::show(strips[i].leds, strips[i].count, strips[i].pin, bright);
      if (reset_list) strip_update_list[i] = false;
    }
  }

  void StripGroup::clear()
  {
    if (strips == nullptr) return;
    for (uint16_t i = 0; i < strip_count; ++i) strips[i].clear();
  }

  Strip* StripGroup::getStripPtr(uint16_t index) const
  {
    if (index < strip_count) return (strips+index);
    return nullptr;
  }

  uint16_t StripGroup::numStrips() const
  {
    return strip_count;
  }

  uint8_t StripGroup::getBrightness() const
  {
    return bright;
  }

  uint32_t StripGroup::getPixelColor(uint32_t n) const
  {
    if (n >= led_count) return 0;
    return getLedReference(n);
  }

  void StripGroup::setBrightness(uint8_t b)
  {
    bright = b;
  }

  void StripGroup::setPixelColor(uint32_t n, uint32_t color)
  {
    if (n >= led_count) return;
    getLedReference(n) = color;
  }

  void StripGroup::setPixelColor(uint32_t n, uint8_t r, uint8_t g, uint8_t b)
  {
    if (n >= led_count) return;
    getLedReference(n) = {r, g, b};
  }

  LED& StripGroup::getLedReference(uint32_t n) const
  {
    for (uint16_t i = 0; i < strip_count; ++i)
    {
      if (n < strips[i].count)
      {
        if (strips[i].reverse)
        {
          return strips[i].leds[strips[i].count - 1 - n];
        }
        return strips[i].leds[n];
      }
      n -= strips[i].count;
    }
    return void_led;
  }
}

// ###################################################  WS2812B_Rect ##########################################################################

// namespace WS2812B
// {
//   Pos::Pos(int16_t x, int16_t y) : x{x}, y{y} {}
//   Pos::Pos() : Pos(0, 0) {}

//   Size::Size(uint16_t w, uint16_t h) : w{w}, h{h} {}
//   Size::Size() : Size(0u, 0u) {}

//   Rect::Rect() : is_begin{0}, pin{255}, dim{0u, 0u}, led_matrix{nullptr}, led_count{0u} {}
//   Rect::Rect(LED* matrix, uint16_t w, uint16_t h, uint8_t p, connection_t connection) : bright{255}, is_begin{0}, pin{p}, x_ori{NORMAL}, y_ori{NORMAL}, dim{w, h}, led_matrix{matrix}, led_count{0u}, con{connection}
//   {
//     calcLedCount();
//   }

//   bool Rect::begin()
//   {
//     is_begin = WS2812B::begin(pin);
//     return is_begin;
//   }

//   void Rect::setConnectionType(Rect::connection_t connection)
//   {
//     con = connection;
//   }

//   Rect::connection_t Rect::getConnectionType() const
//   {
//     return con;
//   }

//   void Rect::changeLEDsConfig(LED* matrix, uint16_t width, uint16_t height)
//   {
//     led_matrix = matrix;
//     dim.w = width;
//     dim.h = height;
//   }

//   void Rect::setXOrientation(Rect::orientation_t orientation)
//   {
//     x_ori = orientation;
//   }

//   void Rect::setYOrientation(Rect::orientation_t orientation)
//   {
//     y_ori = orientation;
//   }

//   Rect::orientation_t Rect::getXOrientation() const
//   {
//     return x_ori;
//   }

//   Rect::orientation_t Rect::getYOrientation() const
//   {
//     return y_ori;
//   }

//   uint16_t Rect::getWidth() const
//   {
//     return dim.w;
//   }

//   uint16_t Rect::getHeight() const
//   {
//     return dim.h;
//   }

//   uint16_t Rect::numLeds() const
//   {
//     return led_count;
//   }

//   void Rect::calcLedCount()
//   {
//     led_count = uint16_t(dim.w * dim.h);
//   }

//   LED& Rect::operator[](Pos pos)
//   {
//     if (pos.x >= dim.w || pos.x < 0 || pos.y >= dim.h || pos.y < 0) return void_led;
//     if (x_ori == REVERSE) pos.x = dim.w - 1 - pos.x;
//     if (y_ori == REVERSE) pos.y = dim.h - 1 - pos.y;
//     switch (con)
//     {
//     case LINEAR:
//       return led_matrix[pos.x * dim.h + ((pos.x & 1) == 0 ? pos.y : dim.h - 1 - pos.y)];
//     case CROSS:
//       return led_matrix[pos.x * dim.h + pos.y];
//     default:
//       break;
//     }
//     return led_matrix[0];
//   }

//   LED& Rect::operator[](uint16_t i)
//   {
//     if (i >= led_count) return void_led;
//     return led_matrix[i];
//   }

//   void Rect::fill(uint32_t c)
//   {
//     WS2812B::fill(led_matrix, led_count, c);
//   }

//   void Rect::fill(uint8_t r, uint8_t g, uint8_t b)
//   {
//     WS2812B::fill(led_matrix, led_count, r, g, b);
//   }

//   void Rect::fill(const Color& c)
//   {
//     WS2812B::fill(led_matrix, led_count, c);
//   }

//   void Rect::clear()
//   {
//     WS2812B::clear(led_matrix, led_count);
//   }

//   void Rect::show()
//   {
//     if (!is_begin) return;
//     WS2812B::show(led_matrix, led_count, pin, bright);
//   }

//   void Rect::drawRectangle(Pos pos, Size size, uint32_t color, bool fill)
//   {
//     if (pos.x >= (int16_t)dim.w || pos.y >= (int16_t)dim.h || size.w < 1 || size.h < 1) return;
    
//     if (fill)
//     {
//       for (int16_t i = 0; i < size.h; ++i)
//         for (int16_t j = 0; j < size.w; ++j)
//           this->operator[]({pos.x + j, pos.y + i}) = color;
//     }
//     else
//     {
//       for (int16_t i = 0; i < size.w; ++i)
//       {
//         this->operator[]({pos.x + i, pos.y}) = color;
//         this->operator[]({pos.x + i, pos.y + size.h - 1}) = color;
//       }

//       for (int16_t i = 0; i < size.h; ++i)
//       {
//         this->operator[]({pos.x, pos.y + i}) = color;
//         this->operator[]({pos.x + size.w - 1, pos.y + i}) = color;
//       }
//     }
//   }

//   void Rect::drawRectangle(Pos pos, uint16_t width, uint16_t height, uint32_t color, bool fill)
//   {
//     return drawRectangle(pos, {width, height}, color, fill);
//   }

//   void Rect::drawRectangle(int16_t x, int16_t y, Size size, uint32_t color, bool fill)
//   {
//     return drawRectangle({x, y}, size, color, fill);
//   }

//   void Rect::drawRectangle(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t color, bool fill)
//   {
//     return drawRectangle({x, y}, {w, h}, color, fill);
//   }

//   void Rect::drawLine(Pos begin, Pos end, uint32_t color)
//   {
//     int16_t dx = abs(end.x - begin.x);
//     int16_t dy = abs(end.y - begin.y);
//     int16_t sx = (begin.x < end.x) ? 1 : -1;
//     int16_t sy = (begin.y < end.y) ? 1 : -1;
//     int16_t err = dx - dy;

//     while (true)
//     {
//       this->operator[](begin) = color;

//       if (begin.x == end.x && begin.y == end.y) break;

//       int16_t e2 = 2 * err;

//       if (e2 > -dy) 
//       {
//         err -= dy;
//         begin.x += sx;
//       }

//       if (e2 < dx)
//       {
//         err += dx;
//         begin.y += sy;
//       }
//     }
//   }

//   void Rect::drawLine(int16_t x0, int16_t y0, Pos end, uint32_t color)
//   {
//     return drawLine({x0, y0}, end, color);
//   }

//   void Rect::drawLine(Pos begin, int16_t x1, int16_t y1, uint32_t color)
//   {
//     return drawLine(begin, {x1, y1}, color);
//   }

//   void Rect::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
//   {
//     return drawLine({x0, y0}, {x1, y1}, color);
//   }

//   void Rect::drawCircle(Pos center, uint16_t r, uint32_t color, bool fill)
//   {
//     int16_t x = 0;
//     int16_t y = r;
//     int16_t d = 1 - r;

//     while (y >= x)
//     {
//       if (fill)
//       {
//         for (int16_t i = -x; i <= x; ++i)
//         {
//           this->operator[]({center.x + i, center.y + y}) = color;
//           this->operator[]({center.x + i, center.y - y}) = color;
//         }
//         for (int16_t i = -y; i <= y; ++i)
//         {
//           this->operator[]({center.x + i, center.y + x}) = color;
//           this->operator[]({center.x + i, center.y - x}) = color;
//         }
//       }
//       else
//       {
//         this->operator[]({center.x + x, center.y + y}) = color;
//         this->operator[]({center.x - x, center.y + y}) = color;
//         this->operator[]({center.x + x, center.y - y}) = color;
//         this->operator[]({center.x - x, center.y - y}) = color;
//         this->operator[]({center.x + y, center.y + x}) = color;
//         this->operator[]({center.x - y, center.y + x}) = color;
//         this->operator[]({center.x + y, center.y - x}) = color;
//         this->operator[]({center.x - y, center.y - x}) = color;
//       }

//       ++x;

//       if (d >= 0)
//       {
//         --y;
//         d = d + 2 * (x - y) + 1;
//       }
//       else
//       {
//         d = d + 2 * x + 1;
//       }
//     }
//   }

//   void Rect::drawCircle(int16_t x_center, int16_t y_center, uint16_t r, uint32_t color, bool fill)
//   {
//     return drawCircle({x_center, y_center}, r, color, fill);
//   }
// }