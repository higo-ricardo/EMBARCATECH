///Blink LED_BUILTIN Pi Pico W
//Autor: HIGO RICARDO
//Discord: higo-ricardo
//Email: hrmelo@tjma.jus.br

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

# define LED CYW43_WL_GPIO_LED_PIN //Cria a variável LED e atribui o valor CYW43_WL_GPIO_LED_PIN
bool ON = true;
bool OFF = false;

int main() {
  stdio_init_all(); //Inicializa a placa Pi Pico W
  cyw43_arch_init(); //Inicializa a placa WiFi
  while (true) {
    
    cyw43_arch_gpio_put(LED, ON); //Liga o LED
    sleep_ms(500);
    cyw43_arch_gpio_put(LED, OFF);//Desliga o LED
    sleep_ms(500);
  }
}
