#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/time.h"

//configurando fontes
#include "inc/ssd1306.h"
#include "inc/font.h"

//configurando o display
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

//configurando o joystick
#define JOYSTICK_X_PIN 26 //GPIO para eixo x
#define JOYSTICK_Y_PIN 27 //GPIO para eixo y
#define JOYSTICK_BUTTON_PIN 22 //GPIO para botão do Joystick

//configurando do botão
#define BUTTON_A_PIN 5 //GPIO para botão

//configurando LEDs
#define LED_VERDE_PIN 11 //GPIO para LED verde
#define LED_AZUL_PIN 12 //GPIO para LED azul
#define LED_VERMELHO_PIN 13 //GPIO para LED vermelho

//variáveis globais
static volatile uint32_t last_time = 0; //armazena o último evento de temo (microssegundos)

//rotina principal
int main(){
    //inicializa o sistema
    stdio_init_all();

    //inicializa o ADC
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    //inicializa o joystick
    gpio_init(JOYSTICK_X_PIN);    
    gpio_init(JOYSTICK_Y_PIN);
    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);
    //inicializa o botão
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    //inicializa os LEDs
    gpio_init(LED_VERDE_PIN);
    gpio_set_dir(LED_VERDE_PIN, GPIO_OUT);
    gpio_put(LED_VERDE_PIN, 0);
    
    gpio_init(LED_AZUL_PIN);
    gpio_set_dir(LED_AZUL_PIN, GPIO_OUT);
    gpio_put(LED_AZUL_PIN, 0);

    gpio_init(LED_VERMELHO_PIN);
    gpio_set_dir(LED_VERMELHO_PIN, GPIO_OUT);
    gpio_put(LED_VERMELHO_PIN, 0);

    // Configura SSD1306
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);


    
}

// rotina de interrupção
void gpio_irq_handler(uint gpio, uint32_t events) {
    // armazena o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // verifica se passou tempo suficiente desde o último evento (debouncing) 
    if (current_time - last_time > 200000) { // 200ms de debounce
        if (gpio == BUTTON_A_PIN) { // botão 0 pressionado
                           
        }
    }
}
