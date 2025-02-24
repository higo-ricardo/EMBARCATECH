cmake_minimum_required(VERSION 3.13)

# Definição do projeto
project(robotic_arm_joystick C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Inclusão do SDK do Raspberry Pi Pico
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)
pico_sdk_init()

# Arquivo fonte principal
add_executable(robotic_arm_joystick
    main.c
)

# Linka bibliotecas do Pico SDK
target_link_libraries(robotic_arm_joystick pico_stdlib hardware_pwm hardware_adc hardware_gpio)

# Habilita saída USB para depuração
pico_enable_stdio_usb(robotic_arm_joystick 1)
pico_enable_stdio_uart(robotic_arm_joystick 0)

# Criação de binário UF2
pico_add_extra_outputs(robotic_arm_joystick)
