//Blink LED_RGB_BUILTIN BitDogLab
//Autor: HIGO RICARDO
//Discord: higo-ricardo
//Email: hrmelo@tjma.jus.br

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LEDR 12
#define LEDG 11
#define LEDB 13
bool ON = true;
bool OFF = false;

void LED_RGB(bool r, bool g, bool b) {
    gpio_put(LEDR, r);
    gpio_put(LEDG, g);
    gpio_put(LEDB, b);
}

int main() {
    stdio_init_all();

    gpio_init(LEDR);
    gpio_init(LEDG);
    gpio_init(LEDB);

    gpio_set_slew_rate(LEDR, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDG, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDB, GPIO_SLEW_RATE_SLOW);

    gpio_set_dir(LEDR, true);
    gpio_set_dir(LEDG, true);
    gpio_set_dir(LEDB, true);

    while (true) {
        LED_RGB(ON, OFF, OFF); // Acende o LED na cor vermelha
        sleep_ms(500);
        LED_RGB(OFF, ON, OFF); //Acende o LED na cor verde
        sleep_ms(500);
        LED_RGB(OFF, OFF, ON); //Acende o LED na cor azul
        sleep_ms(500);
        LED_RGB(ON, ON, ON); //Acende O LED na cor branca
        sleep_ms(500);
    }
}
