/******************************************
  || Embarcatech                            ||
  || Autor: HIGO RICARDO DE OLIVEIRA MELO   ||
  || Email: hrmelo@tjma.jus.br              ||
  || Data: 17/01/2025                       ||
*******************************************/

#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

// Definindo os pinos para LEDs RGB
#define LED_R 28
#define LED_G 27
#define LED_B 26
#define COMP 4
// Definindo os pinos para o teclado matricial (4x4)
const uint Linha[COMP] = {13, 12, 11, 10}; // Pinos para as linhas do teclado
const uint Coluna[COMP] = {9, 8, 7, 6};    // Pinos para as colunas do teclado

// Mapeamento do teclado
char teclado[COMP][COMP] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

/*LÓGICA DE TESTE DA SENHA*/
// vaRIAVEL APRA ARAZAMENAR A Senha DO COFRE
const char senha_padrao[COMP + 1] = "1234";
// Variável para armazenar a senha digitada
char senha_digitada[COMP + 1] = {'\0'};
int senha_index = 0;

/*LÓGICA DO  BLOQUEIO DO COFRE */
#define MAX_TENTATIVAS 3
#define TEMPO_BLOQUEIO 5000 // Tempo de bloqueio em milissegundos
// Variáveis para controle de tentativas e bloqueio
int tentativas_erradas = 0;
bool bloqueado = false;

// Inicializa o teclado matricial
void keypad_init() {
  for (int i = 0; i < COMP; i++) {
    gpio_init(Linha[i]);
    gpio_set_dir(Linha[i], GPIO_OUT);
    gpio_put(Linha[i], 1); // Puxa para alto
  }

  for (int i = 0; i < COMP; i++) {
    gpio_init(Coluna[i]);
    gpio_set_dir(Coluna[i], GPIO_IN);
    gpio_pull_up(Coluna[i]); // Ativa o pull-up nas colunas
  }
}

// Inicializa os LEDs
void config() {
  gpio_init(LED_R);
  gpio_init(LED_G);
  gpio_init(LED_B);

  gpio_set_dir(LED_R, GPIO_OUT);
  gpio_set_dir(LED_G, GPIO_OUT);
  gpio_set_dir(LED_B, GPIO_OUT);

  gpio_put(LED_R, 0);
  gpio_put(LED_G, 0);
  gpio_put(LED_B, 0);
}

// Controla os LEDs com base na tecla pressionada
void controlLED(char key) {
  gpio_put(LED_R, 0);
  gpio_put(LED_G, 0);
  gpio_put(LED_B, 0);

  switch (key) {
    case '1':
    case '4':
    case '7':
    case '*':
      gpio_put(LED_R, 1); // Acende o LED vermelho
      break;
    case '2':
    case '5':
    case '8':
    case '0':
      gpio_put(LED_G, 1); // Acende o LED verde
      break;
    case '3':
    case '6':
    case '9':
    case '#':
      gpio_put(LED_B, 1); // Acende o LED azul
      break;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
      // Todos os LEDs permanecem apagados
      break;
  }
}

// Função de debounce
bool debounce(uint pin) {
  if (!gpio_get(pin)) { // Detecta tecla pressionada
    sleep_ms(50);       // Aguarda 50ms para estabilizar
    return !gpio_get(pin); // Verifica novamente se o estado é estável
  }
  return false;
}

// Lê a tecla pressionada com debounce
char read_keypad() {
  for (int row = 0; row < COMP; row++) {
    gpio_put(Linha[row], 0); // Ativa a linha atual

    for (int col = 0; col < COMP; col++) {
      if (debounce(Coluna[col])) { // Verifica a tecla pressionada com debounce
        gpio_put(Linha[row], 1);   // Desativa a linha
        return teclado[row][col]; // Retorna a tecla pressionada
      }
    }

    gpio_put(Linha[row], 1); // Desativa a linha
  }
  return '\0'; // Se nenhuma tecla foi pressionada
}

// Função para verificar o tamanho da senha e compará-la com a senha padrão
void verifica_senha(char key) {
  if (bloqueado) {
    printf("Cofre bloqueado. Aguarde o tempo de desbloqueio.\n");
    return;
  }
  if (senha_index < COMP) {
    senha_digitada[senha_index++] = key; // Armazena o caractere na senha digitada
  }

  if (senha_index == COMP) { // Verifica se o tamanho da senha foi atingido
    senha_digitada[COMP] = '\0'; // Finaliza a string
    if (strcmp(senha_digitada, senha_padrao) == 0) {
      printf("\nSenha correta! Acesso liberado.\n");
      tentativas_erradas = 0; // Reseta as tentativas em caso de sucesso
    } else {
      printf("\nSenha incorreta! Tente novamente.\n");
      tentativas_erradas++;
      if (tentativas_erradas >= MAX_TENTATIVAS) {
        printf("Máximo de tentativas atingido! Cofre bloqueado por %d segundos.\n", TEMPO_BLOQUEIO / 1000);
        bloqueado = true;
        sleep_ms(TEMPO_BLOQUEIO); // Aguarda o tempo de bloqueio
        bloqueado = false;
        tentativas_erradas = 0; // Reseta o contador de tentativas
        printf("Cofre desbloqueado. Você pode tentar novamente.\n");
      }
    }
    senha_index = 0; // Reinicia para nova tentativa
  }
}

int main() {
  stdio_init_all();  // Inicializa o sistema de entrada e saída
  keypad_init();     // Inicializa o teclado matricial
  config();          // Inicializa os LEDs

  char key = '\0';

  while (true) {
    key = read_keypad(); // Lê a tecla pressionada

    if (key) {
      printf("Tecla pressionada: %c\n", key); // Imprime a tecla pressionada
      controlLED(key);                        // Controla os LEDs com base na tecla pressionada
      verifica_senha(key);                    // Verifica a senha digitada
    } else {
      gpio_put(LED_R, 0); // Desliga os LEDs
      gpio_put(LED_G, 0);
      gpio_put(LED_B, 0);
    }

    sleep_ms(50);
  }

  return 0;
}
