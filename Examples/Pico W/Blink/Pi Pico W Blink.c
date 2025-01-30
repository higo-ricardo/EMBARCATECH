
/*                   BLINK THE LED RGB INTO PI PICO W
  Qualquer reprodução (total ou parcial) do conteúdo divulgado no site, 
  sem menção do autor (HIGO RICARDO), configura plágio. Para aprender
  mais, visite https://github.com/higo-ricardo.
  Dê um "like" no projeto.
*/

#include <stdio.h>
#include "pico/stdlib.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

// Definindo os pinos para LEDs RGB
const uint LED_R = 14, LED_G = 13, LED_B = 12;
bool ON = true;
bool OFF = false;

// Função para controlar o estado dos LEDs
void blink_RGB(bool r, bool g, bool b, uint time) {
  gpio_put(LED_R, r);
  gpio_put(LED_G, g);
  gpio_put(LED_B, b);
  sleep_ms(time);
}

int main(void) {
  // Inicializando o sistema de entrada e saída padrão (para printf)
  stdio_init_all();

#ifdef CYW43_WL_GPIO_LED_PIN
  if (cyw43_arch_init()) {
    printf("Falha ao inicializar a placa WiFi\n");
    return -1;
  }
#endif

  // Inicializando os pinos GPIO para os LEDs
  gpio_init(LED_R);
  gpio_init(LED_G);
  gpio_init(LED_B);

  // Configurando os pinos como saída
  gpio_set_dir(LED_R, GPIO_OUT);
  gpio_set_dir(LED_G, GPIO_OUT);
  gpio_set_dir(LED_B, GPIO_OUT);

  // Loop principal
  while (ON) {
    blink_RGB(ON, OFF, OFF, 1000);  // LED exibe cor vermelha
    blink_RGB(OFF, ON, OFF, 1000); // LED exibe cor Verde
    blink_RGB(OFF, OFF, ON, 1000); // LED exibe cor aazul
    blink_RGB(ON, ON, ON, 1000);   // LED exibe cor branca
    blink_RGB(OFF, OFF, OFF, 1000); // Desliga o LED
  }
  return 0;
}
