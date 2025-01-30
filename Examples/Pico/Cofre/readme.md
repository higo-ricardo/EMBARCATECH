# COFRE DIGITAL

Esse projeto, de nível intermédiário, é ideal para makers, hobistas e amantes de programação embarcada porque possibilita a lógica básica de funcionamento de um cofre digital.
Este código exige conhecimentos de matrizes, funções, comunicação serial, entre outros.
Para facilitar a aprendizagem, a construção da parte mecãnica de abertura e fechamento da fechadura não será abordado.

## Recursos
```diff
# 01 placa de desenvolvimento Pi Pico
# 01 Teclado matricial 4x4
# 01 LED
# 01 resistor de 220ohms.
# 12 jumpers de ligação
# Visual Code IDE com Pico SDK e Maketools previamente instalados.
```
Esse projeto está dispnível no Wokwi.

*[Clique aqui](https://wokwi.com/projects/421144362535890945)*

---
## Requisitos de Software

### 1. Requisitos Funcionais

#### 1.1 Autenticação e Controle de Acesso ####
- RF01: O sistema deve suportar autenticação mediante senha por teclado
- RF02: O sistema deve permitir redefinição de senha com 4 dígitos por teclado
- RF03: O sistema deve bloquear após 3 tentativas de acesso com senhas incorretas
- RF04: O sistema deve permitir o reset de senha por teclado

#### 1.2 Operações do Cofre ####
- RF06: O sistema deve permitir de abertura e fechamento do cofre
- RF07: O sistema deve suportar operação de abertura de emergência mediante chave

### 2. Requisitos Não Funcionais

##### 2.1 Segurança ####
- RNF01: O sistema deve operar em corrente contínua para evitar choques elétricos
- RNF02: O sistema deve implementar protocolos de segurança física e lógica

#### 2.2 Desempenho ####
- RNF03: O sistema deve responder a comandos em menos de 1 segundo
- RNF04: O sistema deve suportar operação 24/7 com disponibilidade de 99.99%
- RNF05: O sistema deve ter sistema próprio de energia com autonomia minima de 1440h

#### 2.3 Usabilidade ####
- RNF06: O sistema deve fornecer feedback visual dos estados por LED

#### 2.4 Manutenção ####
- RNF18: O sistema deve ter módulos independentes para facilitar manutenção
- RNF19: O sistema deve ter documentação técnica completa

---

## Contribuição
Sinta-se à vontade para abrir issues e enviar pull requests para melhorias e sugestões.

## Licença
Este projeto está sob a licença MIT. Consulte o arquivo `LICENSE` para mais informações.


