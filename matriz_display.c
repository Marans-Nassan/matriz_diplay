#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "font.h"
#include "ssd1306.h"
#include "ws2812.pio.h"

#define botao_a 5
#define botao_b 6
#define matriz_led 7
#define green_led 11
#define blue_led 12
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_PORT i2c0
#define matriz_led_pins 25
//Criando um macro para a interrupção por questão de clareza.
#define int_irq(gpio_pin) gpio_set_irq_enabled_with_callback(gpio_pin, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
uint8_t i;
static volatile uint64_t last_time;
static volatile uint8_t tela = 0;
static volatile bool atualiza_display;
static volatile bool on_off = 0;
static volatile bool on_off2 = 0;

typedef struct pixeis {
    uint8_t R, G, B;
}pixeis;

pixeis leds [matriz_led_pins];

PIO pio;
uint sm;

void ledinit(){ //inicialização dos leds básicos.
    for(i = 11 ; i < 13; i++){
        gpio_init(i);
        gpio_set_dir(i, 1);
        gpio_put(i, 0);
    }
}

void botinit(){ //inicialização dos botões.
    for(i = 5; i < 7; i++){
        gpio_init(i);
        gpio_set_dir(i, 0);
        gpio_pull_up(i);
    }
}

void minit(uint pin){ //inicialização da matriz de led.

uint offset = pio_add_program(pio0, &ws2812_program);
pio = pio0;

sm = pio_claim_unused_sm(pio, false);
    if(sm < 0){
        pio = pio1;
        sm = pio_claim_unused_sm(pio, true);
    }

ws2812_program_init(pio, sm, offset, pin, 800000.f);
}
//configuração para permitir o uso da função display. (configuração da matriz de led)
void setled(const uint index, const uint8_t r, const uint8_t g, const uint8_t b){
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}
//esta é a função responsável por permitir ditar qual led vai acender.
void display(){
    for (uint i = 0; i < matriz_led_pins; ++i) {
        pio_sm_put_blocking(pio, sm, leds[i].R);
        pio_sm_put_blocking(pio, sm, leds[i].G);
        pio_sm_put_blocking(pio, sm, leds[i].B);
    }
sleep_us(100); 
}

//Parte da configuração dos símbolos.
void digit_complement(const uint8_t *digit_leds, uint16_t count){
    for (size_t i = 0; i < count; ++i) {
        setled(digit_leds[i], 0, 0, 1);
    }
        display();    
}

void digito0(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 15, 19, 14, 10, 5, 9, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito1(){
const uint8_t digit_leds[] = {22, 17, 12, 7, 2};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito2(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 18, 12, 6, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito3(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 19, 14, 13, 12, 11, 10, 9, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito4(){
const uint8_t digit_leds[] = {24, 20, 16, 18, 12, 7, 2};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito5(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 15, 14, 13, 12, 11, 10, 9, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito6(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 15, 14, 13, 12, 11, 10, 9, 5, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito7(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 18, 12, 6, 4};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito8(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 15, 19, 14, 13, 12, 11, 10, 5, 9, 4, 3, 2, 1, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

void digito9(){
const uint8_t digit_leds[] = {24, 23, 22, 21, 20, 15, 19, 14, 13, 12, 11, 10, 9, 0};
digit_complement(digit_leds, sizeof(digit_leds) / sizeof(digit_leds[0]));
}

static void (*digitos[10])() = { //função ponteiro para o que ser chamado para a matriz.
    digito0, digito1, digito2, digito3, digito4,
    digito5, digito6, digito7, digito8, digito9
};

void led_clear(){ // limpar o led para possibilitar a adição do próximo número sem colidir com o anterior.
    for(i = 0; i < matriz_led_pins; i++){
        setled(i, 0, 0, 0);
    }
}

const void digito_matriz(){
    led_clear();
    char entrada;
    scanf(" %c", &entrada);
    if(entrada >= '0' && entrada <= '9'){
        uint8_t indice = entrada - '0';
        digitos[indice]();
    }
    else {
        led_clear();
        display();
    }
}

void i2cinit(){
i2c_init(I2C_PORT, 400*1000);

    for(i = 14 ; i < 16; i++){
        gpio_set_function(i, GPIO_FUNC_I2C);
        gpio_pull_up(i);
    }
}

bool check(){
    (on_off == 0) ? printf("Desligado\n"): printf("Ligado\n");
    return true;
}

void gpio_irq_handler (uint gpio, uint32_t events){
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if((gpio == botao_a || gpio == botao_b) && (current_time - last_time > 300000)){ //debounce em microsegundos (0.3s)
    (gpio == botao_a) ? (gpio_put(11, !gpio_get(11))), printf("Led verde\n"): (void)0; //Ligando||Desligando led verde
    (gpio == botao_b) ? (gpio_put(12, !gpio_get(12))), printf("Led Azul\n"): (void)0; //Ligando||Desligando led azul
    on_off = !on_off;
    check();
    last_time = current_time;
    }
}

int main(){
    
stdio_init_all();
ledinit();
botinit();
int_irq(botao_a);
int_irq(botao_b);
i2cinit();
minit(matriz_led);

    while (true) {
    digito_matriz();
    
    }
}