# Projeto: Controle de LEDs e Display OLED com Raspberry Pi Pico

## Descrição

Este projeto tem como objetivo controlar LEDs comuns, LEDs endereçáveis WS2812 e um display OLED SSD1306 utilizando um Raspberry Pi Pico. A comunicação entre os componentes é realizada via UART e I2C, e os botões de acionamento são gerenciados por interrupções com debounce.

## Objetivos

- Compreender o funcionamento e a aplicação de comunicação serial em microcontroladores.
- Aplicar os conhecimentos adquiridos sobre UART e I2C na prática.
- Manipular e controlar LEDs comuns e LEDs endereçáveis WS2812.
- Fixar o estudo do uso de botões de acionamento, interrupções e debounce.
- Desenvolver um projeto funcional que combine hardware e software.

## Componentes Utilizados

- Raspberry Pi Pico
- Display OLED SSD1306 (I2C)
- LEDs WS2812 endereçáveis
- LEDs comuns (Green e Blue)
- Botões de acionamento

## Configuração de Hardware

### **Pinos Utilizados**

- **Botões:**
  - Botão A: GPIO 5
  - Botão B: GPIO 6
- **LEDs:**
  - LED Verde: GPIO 11
  - LED Azul: GPIO 12
  - Matriz de LEDs WS2812: GPIO 7
- **Display OLED (I2C):**
  - SDA: GPIO 14
  - SCL: GPIO 15

## Instalação e Compilação

1. Configure o ambiente de desenvolvimento do Raspberry Pi Pico (SDK do Pico e CMake).
2. Clone este repositório.
3. Compile o código utilizando CMake e o SDK do Pico.
4. Carregue o arquivo .uf2 gerado para o Raspberry Pi Pico.

## Funcionalidades

- Controle de LEDs individuais através dos botões.
- Apresentação de números na matriz de LEDs.
- Atualização de mensagens no display OLED conforme interações.
- Detecção de entrada via UART para alteração dos números exibidos na matriz de LEDs.
- Uso de interrupções para otimização da leitura dos botões.

## Como Usar

1. **Iniciar o sistema:** Ao ligar, o display OLED será inicializado e os LEDs ficarão apagados.
2. **Pressionar um botão:**
   - Botão A alterna o estado do LED Verde.
   - Botão B alterna o estado do LED Azul.
   - O display OLED exibe o estado atual dos LEDs.
3. **Enviar um número via UART:**
   - O número correspondente será exibido na matriz de LEDs.
   - O mesmo número será mostrado no display OLED.

## Motivação

Desenvolvido para fins educacionais no estudo de microcontroladores e comunicação serial.

## Autor

Hugo Martins Santana (TIC370101267)