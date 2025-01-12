/****************************************************
************ Blink na placa Pi Pico *****************
*************Autor: Higo Ricardo*********************
*************Discord: higo-ricardo*******************
*************GitHub: higo-ricardo********************
*************Data: 12/01/2025************************
*****************************************************/

#include <stdio.h>
#include "pico/stdlib.h"

//const uint8_t LED_BUILTIN = 25; // Pino do LED integrado
#define LED_BUILTIN 25

int main() {
  stdio_init_all(); // Inicializa a placa Pi Pico
  gpio_init(LED_BUILTIN);
  gpio_set_dir(LED_BUILTIN, GPIO_OUT); // Define o pino como saída


  while (true) {
    gpio_put(LED_BUILTIN, true); //Liga o LED embutido
    sleep_ms(500);
    gpio_put(LED_BUILTIN, false); //Desliga o LED embutido
    sleep_ms(500);
  }
}
