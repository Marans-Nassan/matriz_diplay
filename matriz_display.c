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

void ledinit(){
    for(i = 11 ; i < 13; i++){
        gpio_init(i);
        gpio_set_dir(i, 1);
        gpio_put(i, 0);
    }
}

void botinit(){
    for(i = 5; i < 7; i++){
        gpio_init(i);
        gpio_set_dir(i, 0);
        gpio_pull_up(i);
    }
}

bool check(){
    (on_off == 0) ? printf("Desligado\n"): printf("Ligado\n");
    return true;
}

void minit(uint pin){

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
//esta é a função responsável por permitir ditar qual led vai acender e apagar.
void display(){
    for (uint i = 0; i < matriz_led; ++i) {
        pio_sm_put_blocking(pio, sm, leds[i].R);
        pio_sm_put_blocking(pio, sm, leds[i].G);
        pio_sm_put_blocking(pio, sm, leds[i].B);
    }
sleep_us(100); 
}

void i2cinit(){
i2c_init(I2C_PORT, 400*1000);

    for(i = 14 ; i < 16; i++){
        gpio_set_function(i, GPIO_FUNC_I2C);
        gpio_pull_up(i);
    }

}

void gpio_irq_handler (uint gpio, uint32_t events){
    uint64_t current_time = to_us_since_boot(get_absolute_time());
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
i2cinit();

gpio_set_irq_enabled_with_callback(botao_a, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
gpio_set_irq_enabled_with_callback(botao_b, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(10000);
    }
}
