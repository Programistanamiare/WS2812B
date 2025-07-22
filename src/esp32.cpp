#ifdef ESP32

#include "ws2812b.hpp"
#include <driver/rmt.h>

#if F_CPU == 240000000L

#define WS2812B_RMT_CLK_DIV 6

#elif F_CPU == 160000000L

#define WS2812B_RMT_CLK_DIV 4

#elif F_CPU == 80000000L

#define WS2812B_RMT_CLK_DIV 2

#else

#define WS2812B_RMT_CLK_DIV 1

#error "Only 240, 160 or 80 MHz F_CPU are supported, sorry."

#endif

static unsigned long endTime = 0u;

static bool canShow()
{
  unsigned long now = micros();
  if (endTime > now) {
    endTime = now;
  }
  return (now - endTime) > 50L;
}

static portMUX_TYPE show_mux = portMUX_INITIALIZER_UNLOCKED;


namespace WS2812B
{

  void show(LED* leds, uint16_t len, uint8_t pin, uint8_t bright)
  {
    if (leds == nullptr) return;
    
    volatile uint8_t* bytes = (volatile uint8_t*)leds;
    volatile uint16_t num_bytes = len * 3;


    uint32_t gpio_mask = (1 << pin);
    volatile uint32_t* gpio_reg = (volatile uint32_t*)0x3FF44004;

#ifdef FREERTOS_CONFIG_H
    if (xPortGetCoreID() == 1) taskENTER_CRITICAL(&show_mux);
#else
    // zablokowanie przerwań w inny sposób
#endif

    asm volatile (
      "mov t1, %[num_bytes]\n"        // Załaduj długość (liczba LED-ów) do rejestru t1
      "beqz t1, 4f\n"           // Jeśli num_bytes == 0, zakończ

      "li t2, 3\n"              // T2 = 3 (trzy bajty na LED: G, R, B)

      // Pętla dla każdego LED-a
      "1:\n"
      "li t3, 8\n"              // Przygotowanie do wysyłania 8 bitów dla każdej składowej

      // Wczytanie i przeskalowanie zielonego bajtu
      "lbu t0, 0(%[bytes])\n"   // Załaduj zielony bajt
      "mulhu t0, t0, %[bright]\n" // Skalowanie jasności
      "slli t0, t0, 1\n"        // Przesunięcie bitu

      // Wysłanie zielonego bajtu
      "2:\n"
      "bltz t0, 3f\n"           // Jeśli najstarszy bit to 1, przejdź do wysyłania '1'
      "sw %[gpio_mask], 0(%[gpio_reg])\n"  // Ustaw GPIO na wysoki stan (wysyłanie '0')
      "nop\nnop\nnop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\nnop\nnop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "j 5f\n"                  // Przejdź do końca bloku bitu '0'

      // Wysłanie bitu '1'
      "3:\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"

      // Zmniejszenie licznika bitów i przetwarzanie następnego bitu
      "5:\n"
      "addi t3, t3, -1\n"
      "bnez t3, 2b\n"           // Jeśli są jeszcze bity do wysłania, powrót do etykiety "2"

      // Przejdź do czerwonego bajtu
      "addi %[bytes], %[bytes], 1\n" // Przesuń wskaźnik do następnej składowej

      "li t3, 8\n"              // Przygotowanie do wysyłania 8 bitów dla każdej składowej
      "lbu t0, 0(%[bytes])\n"   // Załaduj czerwony bajt
      "mulhu t0, t0, %[bright]\n" // Skalowanie jasności
      "slli t0, t0, 1\n"        // Przesunięcie bitu
      "2:\n"
      "bltz t0, 3f\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\nnop\nnop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "j 5f\n"
      "3:\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "5:\n"
      "addi t3, t3, -1\n"
      "bnez t3, 2b\n"

      // Przejdź do niebieskiego bajtu
      "addi %[bytes], %[bytes], 1\n"
      "li t3, 8\n"
      "lbu t0, 0(%[bytes])\n"
      "mulhu t0, t0, %[bright]\n"
      "slli t0, t0, 1\n"
      "2:\n"
      "bltz t0, 3f\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\nnop\nnop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "j 5f\n"
      "3:\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "sw %[gpio_mask], 0(%[gpio_reg])\n"
      "nop\nnop\nnop\n"
      "not %[gpio_mask], %[gpio_mask]\n"
      "5:\n"
      "addi t3, t3, -1\n"
      "bnez t3, 2b\n"

      // Przejdź do następnego LED-a
      "addi %[bytes], %[bytes], 1\n"  // Przesuń wskaźnik do następnego LED-a (kolejny bajt)
      "addi t1, t1, -1\n"             // Zmniejsz licznik LED-ów
      "bnez t1, 1b\n"                 // Jeśli są jeszcze LED-y, wróć do początku pętli

      // Koniec transmisji
      "4:\n"

      : [bytes] "+r" (bytes), [gpio_mask] "+r" (gpio_mask)
      : [gpio_reg] "r" (gpio_reg), [num_bytes] "r" (num_bytes), [bright] "r" (bright)
      : "t0", "t1", "t2", "t3", "memory"
    );
  }

}


#endif