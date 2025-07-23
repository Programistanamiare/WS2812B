# Library for static allocate memory WS2812B driver.

## Authors

- [@Programistanamiare](https://www.github.com/Programistanamiare)

## API Reference

### Basic LED structure

```cpp
  WS2812B::LED led1{0xff0000} // one led initial with red color
  WS2812B::LED led2{255, 0, 0} // another led initial with red color
  WS2812B::LED led3; // next led initial with blank color (default {0, 0, 0})
```
### Basic LED functions

#### Function to initialize LED pin

```cpp
  bool WS2812B::begin(uint8_t pin);
```
| Parameter | Type | Description |
| :--- | :--- | :--- |
| `pin` | `uint8_t` | Pin connected to led strip |

|Retval | If |
| :--- | :--- |
| `true` | Successfully initialized leds pin |
| `false` | Pin number is equal 255 |

---
#### Function to clear entire LED buffer

```cpp
  void WS2812B::clear(LED* leds, uint16_t len);
```

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `leds` | `LED*` | Pointer to leds buffer |
| `len` | `uint16_t` | Leds buffer len |

---
#### Function to filling the entire LED buffer with one color

```cpp
  void WS2812B::fill(LED* leds, uint16_t len, uint32_t color);
```
| Parameter | Type | Description |
| :--- | :--- | :--- |
| `leds` | `LED*` | Pointer to leds buffer |
| `len` | `uint16_t` | Leds buffer len |
| `color` | `uint32_t` | Color decode to 0xrrggbb |

---
#### Function to filling the entire LED buffer with one color

```cpp
  void WS2812B::fill(LED* leds, uint16_t len, uint8_t r, uint8_t g, uint8_t b);
```

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `leds` | `LED*` | Pointer to leds buffer |
| `len` | `uint16_t` | Leds buffer len |
| `r` | `uint8_t` | Red color part |
| `g` | `uint8_t` | Green color part |
| `b` | `uint8_t` | Blue color part |

---
#### Function to filling the entire LED buffer with one color acording to another LED color

```cpp
  // there is using Color = LED in library, you can use it in this way
  void WS2812B::fill(LED* leds, uint16_t len, const Color& color)
```
| Parameter | Type | Description |
| :--- | :--- | :--- |
| `leds` | `LED*` | Pointer to leds buffer |
| `len` | `uint16_t` | Leds buffer len |
| `color` | `Color&` | Color taken from another LED pixel |

---
#### Function to filling LED buffer from index **x to index **y

```cpp
  void WS2812B::fillFromTo(LED* leds, uint16_t len, uint32_t color, uint16_t from, uint16_t to);
```
| Parameter | Type | Description |
| :--- | :--- | :--- |
| `leds` | `LED*` | Pointer to leds buffer |
| `len` | `uint16_t` | Leds buffer len |
| `color` | `uint32_t` | Color decode to 0xrrggbb |
| `from` | `uint16_t` | First led index |
| `to` | `uint16_y` | Last led index |

---

#### Function to filling LED buffer from index x to index y 

```cpp
  void WS2812B::fillFromTo(LED* leds, uint16_t len, uint8_t r, uint8_t g, uint8_t b, uint16_t from, uint16_t to);
```
| Parameter | Type | Description |
| :--- | :--- | :--- |
| `leds` | `LED*` | Pointer to leds buffer |
| `len` | `uint16_t` | Leds buffer len |
| `r` | `uint8_t` | Red color part |
| `g` | `uint8_t` | Green color part |
| `b` | `uint8_t` | Blue color part |
| `from` | `uint16_t` | First led index |
| `to` | `uint16_y` | Last led index |

---

#### Function to filling LED buffer from index x to index y 

```cpp
  void WS2812B::fillFromTo(LED* leds, uint16_t len, const Color& color, uint16_t from, uint16_t to);
```

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `leds` | `LED*` | Pointer to leds buffer |
| `len` | `uint16_t` | Leds buffer len |
| `color` | `Color&` | Color taken from another LED pixel |
| `from` | `uint16_t` | First led index |
| `to` | `uint16_y` | Last led index |

---
#### Function to convert HSV values to RGB color
```cpp
  WS2812B::Color WS2812B::hsv(uint16_t hue, uint8_t sat = 255, uint8_t val = 255);
```
| Parameter | Type | Description |
| :--- | :--- | :--- |
| `hue` | `uint16_t` | Hue value |
| `sat` | `uint8_t` | Saturation value |
| `val` | `uint8_t` | Value value (i know how it's sounds XD) |

---
#### Function to send data from LED buffer to strip

```cpp
  void WS2812B::show(LED* leds, uint16_t len, uint8_t pin, uint8_t brightness = 255);
```
| Parameter | Type | Description |
| :--- | :--- | :--- |
| `leds` | `LED*` | Pointer to LED buffer |
| `len` | `uint16_t` | Leds buffer len |
| `pin` | `uint8_t` | Pin connected to leds |
| `brightness` | `uint8_t` | Strip brightness |

---

## Usage examples

### Functional option

#### 1. First step; create LEDS buffer

```cpp
  constexpr uint16_t LEDS_COUNT = 50;

  WS2812B::LED leds[LEDS_COUNT];
```
#### 2. Initial pin connect to led strip

```cpp
constexpr uint8_t LEDS_PIN = 3;

void setup()
{
  WS2812B::begin(LEDS_PIN);
}
```
#### 3. Fill your LEDS buffer with some color and show it :)
```cpp

constexpr uint8_t BASE_BRIGHTNESS = 120;

void loop()
{
  WS2812B::fill(leds, LEDS_COUNT, 255, 0, 0) // red color fill
  WS2812B::show(leds, LEDS_COUNT, LEDS_PIN, BASE_BRIGHTNESS); // show colors on strip
  delay(1000); // wait 1s
  WS2812B::clear(leds, LEDS_COUNT); // clear strip
  WS2812B::show(leds, LEDS_COUNT, LEDS_PIN, BASE_BRIGHTNESS); // show cleared strip
  delay(1000); // wait 1s
}
```
