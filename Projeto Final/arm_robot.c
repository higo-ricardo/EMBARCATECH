/* 
┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃ Raspberry Pi Pico W Project (c)                          ┃
┃                                                          ┃
┃ Braço Robótico acionado por joystick analógico           ┃
┃ Capacitação Profissional em Sistemas Embarcados          ┃
┃ Embarcatech IFMA Turma 20024.2                           ┃
┃                                                          ┃
┃ Copyright (c) 2025 Higo Ricardo de Oliveira Melo         ┃
┃ GitHub: github.com/higo-ricardo                          ┃
┃ Discord: higo-ricardo                                    ┃
┃ License: MIT                                             ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
// =============================================//
//            DEPURAÇÃO DO CÓDIGO              //
// =============================================//
#define DEBUG 1 // Ativa (1) ou desativa (0) as mensagens de depuração

#if DEBUG
#define LOG(level, msg, ...) printf("[%s] " msg "\n", level, ##__VA_ARGS__)
#else
#define LOG(level, msg, ...)
#endif
// =============================================//
//          CONFIGURAÇÃO DE HARDWARE            //
// =============================================//
#define BTN_A  5          // BOTÃO AZUL
#define BTN_B  6         // BOTÃO AMARELO
#define LED_G  11       //LED RGB GREEN
#define LED_R  12      //LED RGB RED
#define LED_B  13     // LED RGB BLUE
#define JOY_X 26     // GPIO26 (ADC0) VERTICAL
#define JOY_Y 27    // GPIO27 (ADC1) HORINZONTAL
#define JOYBTN 22   // GPIO22 - BOTÃO DO JOYSTICK
#define SERVO_OMBRO 16      // GPIO16  VERTICAL
#define SERVO_COTOVELO 17  // GPIO17   HORIZONTAL
#define SERVO_GARRA 18    // GPIO18    EIXO Z
/* */
// =============================================//
//         PARAMETROS DE MOVIMENTOS             //
// =============================================//
const uint8_t SERVO_OMBRO_MIN = 10;
const uint8_t SERVO_OMBRO_MAX = 170;
const uint8_t SERVO_COTOVELO_MIN = 10;
const uint8_t SERVO_COTOVELO_MAX = 170;
const uint8_t SERVO_GARRA_MIN = 0;
const uint8_t SERVO_GARRA_MAX = 90;

typedef struct {
  uint16_t x;
  uint16_t y;
  bool joy_btn;
  bool last_joybtn;      // Estado anterior do botão para detecção de mudança
  uint32_t last_press; // Timestamp do último pressionamento para debounce
} JoystickData;

typedef struct {
  uint8_t ombro;
  uint8_t cotovelo;
  uint8_t garra;
  bool garra_estado;   // 0 = aberta, 1 = fechada
} ServoPosition;

// =============================================//
//             VARIAVEIS GLOBAIS                //
// =============================================//
JoystickData joystickData = {0};
ServoPosition servoPos = {90, 90, 0, false};    // Posição inicial dos servos
JoystickData joystickCenter = {0};             //referência para calibração
uint slice_ombro, slice_cotovelo, slice_garra; //Controle PWM dos servos
uint channel_ombro, channel_cotovelo, channel_garra;

// =============================================//
//                  FUNÇÕES                     //
// =============================================//
void init_adc(void);
void init_joystick(void);
void init_servos(void);
uint16_t read_adc(uint8_t gpio);
void read_joystick_data(JoystickData *data);
void set_servo_position(uint8_t gpio, uint8_t angle);
void update_robot_arm(void);
void calibrate_joystick(void);

void init_adc(void) {

  adc_init();                       // Inicializa o ADC
  adc_gpio_init(JOY_X);   // Prepara GPIO26 para uso ADC (ADC0)
  adc_gpio_init(JOY_Y);  // Prepara GPIO27 para uso ADC (ADC1)
  LOG("INFO", "O ADC foi inicializado com sucesso!");
  sleep_ms(1000);

}

// Leitura dos canais ADC
uint16_t read_adc(uint8_t gpio) {
  // Mapeia GPIO para canal ADC
  uint8_t channel;
  switch (gpio) {
    case 26: channel = 0; break; // GPIO26 = ADC0
    case 27: channel = 1; break; // GPIO27 = ADC1
    case 28: channel = 2; break; // GPIO28 = ADC2
    case 29: channel = 3; break; // GPIO29 = ADC3
    default: return 0;
  }

  adc_select_input(channel);
  return adc_read();
}

// Inicialização do joystick
void init_joystick(void) {
  // Configura pino conectado o botão do joystick como entrada com pull-up
  gpio_init(JOYBTN);
  gpio_set_dir(JOYBTN, GPIO_IN);
  gpio_pull_up(JOYBTN);

  // Inicializa dados do joystick
  joystickData.last_press = 0;
  joystickData.last_joybtn = false;
  LOG("INFO", "O joystick foi inicializado com sucesso.");
  sleep_ms(1000);
}

// Leitura dos dados do joystick
void read_joystick_data(JoystickData *data) {
  // Lê os valores analógicos dos eixos X e Y
  data->x = read_adc(JOY_X);
  data->y = read_adc(JOY_Y);
  LOG("DEBUG", "Joystick X: %d, Y: %d", data->x, data->y);
  sleep_ms(1000);

  // Atualiza o estado anterior do botão antes de ler o novo
  data->last_joybtn = data->joy_btn;
  LOG("DEBUG", "Botão do joystick: %s", data->joy_btn ? "Pressionado" : "Liberado");
  // Lê o estado do botão (invertido devido ao pull-up)
  data->joy_btn = !gpio_get(JOYBTN);
}

// Inicialização dos servos usando PWM
void init_servos(void) {
  // Configura os pinos conectados aos servos como PWM
  gpio_set_function(SERVO_OMBRO, GPIO_FUNC_PWM);
  gpio_set_function(SERVO_COTOVELO, GPIO_FUNC_PWM);
  gpio_set_function(SERVO_GARRA, GPIO_FUNC_PWM);

  // Obtém os slices e canais PWM para cada pino
  slice_ombro = pwm_gpio_to_slice_num(SERVO_OMBRO);
  channel_ombro = pwm_gpio_to_channel(SERVO_OMBRO);

  slice_cotovelo = pwm_gpio_to_slice_num(SERVO_COTOVELO);
  channel_cotovelo = pwm_gpio_to_channel(SERVO_COTOVELO);

  slice_garra = pwm_gpio_to_slice_num(SERVO_GARRA);
  channel_garra = pwm_gpio_to_channel(SERVO_GARRA);

  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 50.0f);     // 125MHz / 50 = 2.5MHz
  pwm_config_set_wrap(&config, 50000);       // 2.5MHz / 50000 = 50Hz

  // Aplica a configuração para cada slice e inicia o PWM
  pwm_init(slice_ombro, &config, true);
  pwm_init(slice_cotovelo, &config, true);
  pwm_init(slice_garra, &config, true);

  // Posição inicial dos servos
  set_servo_position(SERVO_OMBRO, servoPos.ombro);
  set_servo_position(SERVO_COTOVELO, servoPos.cotovelo);
  set_servo_position(SERVO_GARRA, servoPos.garra);
  LOG("INFO", "Os servos foram inicializados com sucesso!");
  sleep_ms(1000);
}

// Função para definir posição do servo (0-180 graus)
void set_servo_position(uint8_t gpio, uint8_t angle) {
  // Limita o ângulo a 0-180
  if (angle > 180) angle = 180;

  // Calcula o valor PWM baseado no ângulo
  // 1ms (1000μs) = 0° -> 2ms (2000μs) = 180°
  // Com wrap de 50000, isso corresponde a valores entre 2500 e 5000
  uint16_t pwm_value = 2500 + ((angle * 2500) / 180);

  // Define o valor PWM para o pino correspondente
  uint slice, channel;

  if (gpio == SERVO_OMBRO) {
    slice = slice_ombro;
    channel = channel_ombro;
  } else if (gpio == SERVO_COTOVELO) {
    slice = slice_cotovelo;
    channel = channel_cotovelo;
  } else if (gpio == SERVO_GARRA) {
    slice = slice_garra;
    channel = channel_garra;
  } else {
    return; // GPIO inválido
  }

  pwm_set_chan_level(slice, channel, pwm_value);
}

// Mapeia valor do joystick para ângulo do servo
uint8_t map_joystick_to_servo(uint16_t joystick_value, uint16_t joystick_center,
                              uint16_t joystick_min, uint16_t joystick_max,
                              uint8_t servo_min, uint8_t servo_max) {
  // Define uma zona morta em torno do centro para estabilidade
  const uint16_t dead_zone = 200; // Valor maior para o Pico devido à maior resolução ADC

  // Se o valor estiver na zona morta, retorna o valor central
  if (abs((int)joystick_value - (int)joystick_center) < dead_zone) {
    return (servo_min + servo_max) / 2;
  }

  // Limita o valor do joystick
  if (joystick_value < joystick_min) joystick_value = joystick_min;
  if (joystick_value > joystick_max) joystick_value = joystick_max;
  uint8_t angle = 0;
  // Mapeia o valor do joystick para o ângulo do servo
  if (joystick_value < joystick_center) {
    return servo_min + ((servo_max - servo_min) * (joystick_value - joystick_min)) /
           (joystick_center - joystick_min);
  } else {
    return servo_min + ((servo_max - servo_min) * (joystick_value - joystick_center)) /
           (joystick_max - joystick_center);
  }
  LOG("DEBUG", "Mapeado valor do joystick (%d) para ângulo do servo: %d", joystick_value, angle);
}

// Atualiza posição do braço robótico com base nos dados do joystick
void update_robot_arm(void) {
  // Lê dados do joystick
  read_joystick_data(&joystickData);

  // Mapeia dados do joystick para posições dos servos
  // O ADC do Pico tem 12 bits (0-4095)
  uint8_t new_ombro_angle = map_joystick_to_servo(joystickData.x, joystickCenter.x,
                            0, 4095, SERVO_OMBRO_MIN, SERVO_OMBRO_MAX);
  uint8_t new_cotovelo_angle = map_joystick_to_servo(joystickData.y, joystickCenter.y,
                               0, 4095, SERVO_COTOVELO_MIN, SERVO_COTOVELO_MAX);

  // Atualiza posição dos servos com filtro para suavizar movimento
  // Conversão para evitar underflow em cálculos com uint8_t
  int temp_ombro = (int)(servoPos.ombro * 0.7 + new_ombro_angle * 0.3);
  int temp_cotovelo = (int)(servoPos.cotovelo * 0.7 + new_cotovelo_angle * 0.3);

  servoPos.ombro = (uint8_t)temp_ombro;
  servoPos.cotovelo = (uint8_t)temp_cotovelo;
  LOG("DEBUG", "Atualizando posição dos servos: Ombro=%d, Cotovelo=%d",
      servoPos.ombro, servoPos.cotovelo);
  // Controle da garra baseado no botão do joystick com detecção de borda de subida
  uint32_t current_time = to_ms_since_boot(get_absolute_time());

  // Verifica se o botão foi pressionado (borda de subida) e se passou tempo suficiente
  if (joystickData.joy_btn && !joystickData.last_joybtn &&
      (current_time - joystickData.last_press > 100)) { // tempo de debounce

    // Alterna o estado da garra
    servoPos.garra_estado = !servoPos.garra_estado;
    servoPos.garra = servoPos.garra_estado ? SERVO_GARRA_MAX : SERVO_GARRA_MIN;

    // Atualiza o timestamp do último pressionamento
    joystickData.last_press = current_time;
    LOG("DEBUG", "Estado da garra alterado: %s",
        servoPos.garra_estado ? "Fechada" : "Aberta");
  }

  // Aplica as posições aos servos
  set_servo_position(SERVO_OMBRO, servoPos.ombro);
  set_servo_position(SERVO_COTOVELO, servoPos.cotovelo);
  set_servo_position(SERVO_GARRA, servoPos.garra);
}

// Calibração do joystick
void calibrate_joystick(void) {
  LOG("INFO", "Iniciando calibração do joystick..."); // DEBUG #6
  sleep_ms(1000);
  LOG("INFO", "Mantenha o joystick em posição central e não pressione o botão!!\n"); // DEBUG #7
  sleep_ms(1000); // Tempo necessário para o sistema "respirar"

  // Toma várias leituras e calcula a média para maior precisão
  uint32_t sum_x = 0, sum_y = 0;
  const int num_samples = 10;

  for (int i = 0; i < num_samples; i++) {
    joystickCenter.x = read_adc(JOY_X);
    joystickCenter.y = read_adc(JOY_Y);
    sum_x += joystickCenter.x;
    sum_y += joystickCenter.y;
    sleep_ms(100);
  }

  // Calcula a média
  joystickCenter.x = sum_x / num_samples;
  joystickCenter.y = sum_y / num_samples;
  LOG("INFO", "Calibração foi concluída!\n :)\n"); //DEBUG #4
  sleep_ms(1000);
}

int main() {

  stdio_init_all(); // Inicialização do sistema
  sleep_ms(1000);  // Tempo necessário para console serial inicializar
  LOG("INFO", "Iniciando sistema de braço robótico com Raspberry Pi Pico W...\n"); //DEBUG #6
  sleep_ms(1000);
  // Inicializa hardware
  init_adc();
  init_joystick();
  init_servos();

  // Calibra o joystick
  calibrate_joystick();
  LOG("INFO", "Sistema pronto!\n:)"); // DEBUG #7
  sleep_ms(1000);
  LOG("INFO", "Já mover o braço por meio do joystick. \nGLHF\n"); // DEBUG #8
  sleep_ms(1000);


  while (1) {
    update_robot_arm();
    sleep_ms(20); // Atualiza a cada 20ms para movimento suave (50Hz)
  }
  sleep_ms(50);
  return 0;
}

