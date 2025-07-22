#ifdef AVR
#include "ws2812b.hpp"

#if F_CPU == 16000000L


static uint32_t endTime = 0u;

namespace WS2812B
{

  void _extern_timer_show(LED* leds, uint16_t len, uint8_t pin, uint8_t bright, uint32_t& timer)
  {
    if (leds == nullptr) return;

    volatile uint8_t* port = portOutputRegister(digitalPinToPort(pin));
    uint8_t pinMask = digitalPinToBitMask(pin);
    volatile uint16_t i = len * 3;
    volatile uint8_t *ptr = (volatile uint8_t*)leds;
    volatile uint8_t b = *ptr++;
    volatile uint8_t hi = *port | pinMask; // Ustawienie stanu wysokiego pinu
    volatile uint8_t lo = *port & ~pinMask; // Ustawienie stanu niskiego pinu
    volatile uint8_t next = lo, bit = 8;    // Inicjalizacja zmiennych


    while (micros() - timer < 50ul) {}  // Czekanie na możliwość transmisji
    noInterrupts();  // Wyłączenie przerwań, aby transmisja była dokładna

    /**
     * t0h = 250ns - 550ns    | 5   -   8   clock ticks
     * t1h = 650ns - 950ns    | 11  -   15  clock ticks
     * t0l = 700ns - 1000ns   | 12  -   16  clock ticks
     * t1l = 300ns - 600ns    | 5   -   9   clock ticks
     */

    asm volatile(
      "first_byte_map:"               // Przeskalowanie jasności dla pierwszej składowej barwy
      "\n\t"
      "\n\t"
      "mul %[byte], %[bright]"        // Skalowanie składowej koloru wg jasności
      "\n\t"
      "mov %[byte], r1"               // Zapisz wynik do rejestru
      "\n\t"
      "clr r1"                        // Wyczyszczenie rejestru r1
      "\n\t"
      "ws2812b_transmission:"         // Start transmisji
      "\n\t"
      "st   %a[port], %[hi]"          // Ustawienie pinu na high (wysokie napięcie) - 125 ns
      "\n\t"
      "sbrc %[byte], 7"               // Sprawdzenie najstarszego bitu składowej koloru - 62.5/125 ns
      "\n\t"
      "mov  %[next], %[hi]"           // Jeśli bit to 1, przygotuj wysoki stan - 62.5 ns
      "\n\t"
      "dec  %[bit]"                   // Zmniejszenie licznika bitów - 62.5 ns
      "\n\t"
      "st   %a[port], %[next]"        // Wyślij bit - 125 ns
      "\n\t"
      "mov  %[next], %[lo]"           // Przygotowanie do kolejnego bitu (stan niski) - 62.5 ns
      "\n\t"
      "breq nextbyte"                 // Sprawdzenie, czy wszystkie bity zostały wysłane - 62.5/125 ns
      "\n\t"
      "rol  %[byte]"                  // Przesunięcie kolejnego bitu - 62.5 ns
      "\n\t"
      "rjmp .+0"                           // 125  ns <-dddddddddddddddd
      "\n\t"
      "nop"                           // 62.5 ns
      "\n\t"
      "nop"                           // 62.5 ns
      "\n\t"
      "nop"                           // 62.5 ns
      "\n\t"
      "st   %a[port], %[lo]"          // Ustawienie pinu na low (niskie napięcie) - 125 ns
      "\n\t"
      "rjmp .+0"                           // Dodatkowe opóźnienie - 125 ns <-dddddddddddddddddddddd
      "\n\t"
      "rjmp ws2812b_transmission"     // Powrót do początku pętli - 125 ns
      "\n\t"
      "nextbyte:"
      "\n\t"
      "ldi  %[bit], 8"                // Inicjalizacja licznika bitów dla następnej składowej - 62.5 ns
      "\n\t"
      "ld   %[byte], %a[ptr]+"        // Załaduj następną składową koloru - 125 ns
      "\n\t"
      "mul %[byte], %[bright]"        // Przeskalowanie składowej koloru wg jasności - 125 ns
      "\n\t"
      "mov %[byte], r1"               // Zapisz wynik do rejestru - 125 ns
      "\n\t"
      "clr r1"                        // Wyczyszczenie rejestru r1 - 125 ns
      "\n\t"
      "st   %a[port], %[lo]"          // Ustawienie pinu na low (niskie napięcie) - 125 ns
      "\n\t"
      "nop"                           // 62.5 ns
      "\n\t"
      "nop"                           // 62.5 ns
      "\n\t"
      "sbiw %[count], 1"              // Zmniejsz licznik składowych - 125 ns
      "\n\t"
      "brne ws2812b_transmission"     // Powrót do początku pętli, jeśli są jeszcze dane - 62.5/125 ns
      "\n"
      : [port] "+e" (port), [byte] "+r" (b), [bit] "+r" (bit), [next] "+r" (next), [count] "+w" (i)
      : [ptr] "e" (ptr), [hi] "r" (hi), [lo] "r" (lo), [bright] "r" (bright)
    );

    interrupts();  // Włącz przerwania
    timer = micros();  // Zapisz czas zakończenia transmisji
  }

  void show(LED* leds, uint16_t len, uint8_t pin, uint8_t bright)
  {
    _extern_timer_show(leds, len, pin, bright, endTime);
  }
}


#elif F_CPU == 8000000L 

namespace WS2812B
{
  void _extern_timer_show(LED* leds, uint16_t len, uint8_t pin, uint8_t bright)
  {
    
  }
}


#else 

#error "Only 16 Mhz F_CPU now is supported, sorry."

#endif // F_CPU == 16000000L

#endif // AVR
