/*
Embarcatech                            
Autor: HIGO RICARDO DE OLIVEIRA MELO   
Email: hrmelo@tjma.jus.br  
Discord:            
Data: 28/01/2025                       
*/

#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

// Pinos do LED RGB
#define LED_R 28
#define LED_G 27
#define COMP 4

// Pinos do teclado matricial (4x4)
const uint Linha[COMP] = {13, 12, 11, 10}; // Linhas do teclado
const uint Coluna[COMP] = {9, 8, 7, 6};    // Colunas do teclado

// Matriz do teclado
char teclado[COMP][COMP] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

/*Variaveis para Captura e Validação da senha*/

const char senha_cofre[COMP + 1] = "1234"; // Variável da senha do COFRE
char senha_digitada[COMP + 1] = {'\0'};    // Variável da senha digitada
int senha_index = 0;

/*LÓGICA DO  BLOQUEIO DO COFRE */
#define tentativas 3
#define bloqueio 5000  // Tempo de bloqueio em milissegundos
int erros = 0; // Variável para controle de tentativas
bool bloqueado = false;

// Função de inicialização do teclado matricial
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

// Função de inicialização do LED
void config() {
  gpio_init(LED_R);
  gpio_init(LED_G);
 
  gpio_set_dir(LED_R, GPIO_OUT);
  gpio_set_dir(LED_G, GPIO_OUT);
 
  gpio_put(LED_R, 0);
  gpio_put(LED_G, 0);
  
}

// Função de controle do LED
void controlLED(char key) {
  gpio_put(LED_R, 0);
  gpio_put(LED_G, 0);

  switch (key) {
    case '1':
    case '4':
    case '7':
    case '*':
      break;
    case '2':
    case '5':
    case '8':
    case '0':
      break;
    case '3':
    case '6':
    case '9':
    case '#':
      break;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
      break;
  }
}

// Função de debounce
bool debounce(uint pin) {
  if (!gpio_get(pin)) { // Detecta tecla pressionada
    sleep_ms(50);       // Aguarda 50ms 
    return !gpio_get(pin); // Verifica novamente se o estado da tecla
  }
  return false;
}

// Função de leitura do teclado com debounce
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

// Função de Validação da senha
void verifica_senha(char key) {
  if (bloqueado) {
    printf("\nCOFRE BLOQUEADO!\n");
    gpio_put(LED_G, 0); // Acende o LED verde
    gpio_put(LED_R, 1); // Acende o LED vermelho
    sleep_ms(3000);
    return;
  }
  if (senha_index < COMP) {
    senha_digitada[senha_index++] = key; // Armazena caractere por caractere a senha digitada
  }

  if (senha_index == COMP) { // Verifica se o tamanho da senha foi atingido
    senha_digitada[COMP] = '\0'; // Finaliza a string
    if (strcmp(senha_digitada, senha_cofre) == 0) {
      printf("\nSenha correta!\n COFRE ESTÁ ABERTO!\n\n");
      erros = 0; // Reseta as tentativas em caso de sucesso
      gpio_put(LED_R, 0); // Acende o LED vermelho
      gpio_put(LED_G, 1); // Acende o LED verde
      sleep_ms(3000);
      gpio_put(LED_G, 0); // Acende o LED verde
    } else {
      printf("\nSenha incorreta! Tente novamente.\n");
      erros++;
      gpio_put(LED_R, 1); // Acende o LED vermelho
      sleep_ms(1000);
      gpio_put(LED_R, 0); // Acende o LED vermelho
      if (erros >= tentativas) {
        printf("\nMáximo de tentativas atingido!\n Cofre bloqueado por %d segundos.\n", bloqueio / 1000);
        bloqueado = true;
        sleep_ms(bloqueio); // Aguarda o tempo de bloqueio
        bloqueado = false;
        erros = 0; // Reseta o contador de tentativas
        printf("\nCofre desbloqueado!\n");
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
    }

    sleep_ms(50);
  }

  return 0;
}
