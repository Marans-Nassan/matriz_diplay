#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware.pio"
#include "font.h"
#include "ssd1306.h"
#include "2812.pio"

#define botoa_a 5
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

typedef struct pixeis {
    uint8_t G, R, B
} pixeis;

pixeis led [matriz_led_pins];

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

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments


void i2cinit(){
i2c_init(I2C_PORT, 400*1000);

    for(i = 14 ; i < 16; i++){
        gpio_set_function(i, GPIO_FUNC_I2C);
        gpio_pull_up(i);
    }

}


int main(){
    
stdio_init_all();
ledinit();
botinit();
i2cinit();
    
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
