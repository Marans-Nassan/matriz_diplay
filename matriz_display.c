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
#define I2C_PORT i2c1
#define endereco 0x3c
#define matriz_led_pins 25

uint8_t i;
static volatile uint64_t last_time;
static volatile bool on_off_a = 0;
static volatile bool on_off_b = 0;
static volatile char entrada;

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

void i2cinit(){ //iniciando o i2c
i2c_init(I2C_PORT, 400*1000);

    for(i = 14 ; i < 16; i++){
        gpio_set_function(i, GPIO_FUNC_I2C);
        gpio_pull_up(i);
    }
}
ssd1306_t ssd; //Organizando as configurações do Oled junto a sua iniciação.
void oledinit(){
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}
// configuração do que vai aparecer no Oled
void oleddis(const char *valor){
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    ssd1306_draw_string(&ssd, valor, 58, 25);
    ssd1306_send_data(&ssd);
    }

void rup_dis(const char *msg){
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    ssd1306_draw_string(&ssd, msg, 0, 25);
    ssd1306_send_data(&ssd);
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
//Macro para definir o que vai aparecer no display da matriz de leds.
#define digits(num, ...) \
void digito##num() { \
    const uint8_t digit_leds[] = { __VA_ARGS__ }; \
    size_t count = sizeof(digit_leds) / sizeof(digit_leds[0]); \
    for (size_t i = 0; i < count; ++i) { \
        setled(digit_leds[i], 0, 1, 0); \
    } \
    display(); \
}

digits(0, 24, 23, 22, 21, 20, 18, 15, 19, 14, 12, 10, 5, 9, 4, 6, 3, 2, 1, 0)
digits(1, 22, 17, 12, 7, 2, 14, 13, 11, 10)
digits(2, 14, 13, 12, 11, 10)
digits(3, 20, 18, 12, 6, 4, 24, 0)
digits(4, 24, 16, 12, 8, 0, 4, 6, 18, 20)
digits(5, 23, 16, 13, 6, 3, 14, 21, 18, 11, 8, 1, 10)
digits(6, 24, 23, 15, 14, 5, 4, 3, 21, 20, 19, 10, 9, 0, 1)
digits(7, 23, 15, 14, 5, 3, 21, 19, 10, 9, 1)
digits(8, 15, 16, 17, 18, 19, 5, 6, 7, 8, 9)
digits(9, 20, 18, 12, 6, 4)

static void (*digitos[10])() = { //função ponteiro para o que ser chamado para a matriz.
    digito0, digito1, digito2, digito3, digito4,
    digito5, digito6, digito7, digito8, digito9
};

void led_clear(){ // limpar o led para possibilitar a adição do próximo número sem colidir com o anterior.
    for(i = 0; i < matriz_led_pins; i++){
        setled(i, 0, 0, 0);
    }
    display();
}

void digito_matriz(){ //organizando o que vai ser apresentado na matriz de led e no Oled caso use números.
    uint8_t indice = entrada - '0';
    digitos[indice]();
    display();
    char str[2] = { entrada, '\0' };
    oleddis(str);
}

void gpio_irq_handler (uint gpio, uint32_t events){
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if((gpio == botao_a || gpio == botao_b) && (current_time - last_time > 300000)){ //debounce em microsegundos (0.3s)
        if(gpio == botao_a){
            gpio_put(11, !gpio_get(11)); //Alterna o estado do pino.
            on_off_a = !on_off_a; //Alterna o estado da boleana para ajudar na identificação do estado.
                if(on_off_a == 1){
                    printf("Green Led On\n");
                    rup_dis("Green Led On");
                }
                else {
                    printf("Green Led Off\n");
                    rup_dis("Green Led Off");
                }
        }
        if(gpio == botao_b){
            gpio_put(12, !gpio_get(12)); 
            on_off_b = !on_off_b; 
                if(on_off_b == 1){
                    printf("Blue Led On\n");
                    rup_dis("Blue Led On");
                }
                else {
                    printf("Blue Led Off\n");
                    rup_dis("Blue Led Off");
                }
        }
        last_time = current_time;
    }
}
//Criando um macro para a interrupção por questão de clareza.
#define int_irq(gpio_pin) gpio_set_irq_enabled_with_callback(gpio_pin, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

int main(){
    
stdio_init_all();
ledinit();
botinit();
int_irq(botao_a);
int_irq(botao_b);
i2cinit();
oledinit();
minit(matriz_led);   
    
    while (true) {
    uint8_t ch = getchar_timeout_us(0);
    scanf(" %c", &entrada);
        if(entrada >= '0' && entrada <= '9'){
            led_clear();
            sleep_ms(1);
            digito_matriz();
        }
        else {
            led_clear();
            char str[2] = { entrada, '\0' };
            oleddis(str);
        }
    }
}