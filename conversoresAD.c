#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/time.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

// Definições de pinos e constantes
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_BUTTON_PIN 22
#define BUTTON_A_PIN 5

#define LED_VERDE_PIN 11
#define LED_AZUL_PIN 12
#define LED_VERMELHO_PIN 13

#define ADC_MAX_VALUE 4095
#define JOYSTICK_CENTER_VALUE 2048
#define BLACK 0
#define WHITE 1

// Variáveis globais
static volatile uint32_t last_time = 0;
static volatile bool leds_ativos = true;
static volatile bool borda_display = false;
ssd1306_t ssd;

// Protótipos de funções
void init_hardware(void);
void update_display(int x, int y);
int map(int x, int in_min, int in_max, int out_min, int out_max);
void gpio_irq_handler(uint gpio, uint32_t events);
void set_led_brightness(uint gpio, uint16_t value);
void init_pwm(uint gpio);
uint16_t leitura_joystick(uint canal);
void atualizar_pwm_leds(void);

// Função principal
int main() {
    init_hardware();    
    while (true) {
        // Leitura dos valores do joystick
        adc_select_input(0);
        uint16_t adc_x = adc_read();
        adc_select_input(1);
        uint16_t adc_y = adc_read();

        // Mapeamento dos valores do joystick para o display
        int pos_y = map(adc_y, 0, ADC_MAX_VALUE, 0, WIDTH - 8);
        int pos_x = map(adc_x, 0, ADC_MAX_VALUE, HEIGHT - 8, 0);
        update_display(pos_y, pos_x);

        // Atualização dos LEDs
        atualizar_pwm_leds();

        sleep_ms(100);
    }
    return 0;
}

// Inicialização do hardware
void init_hardware() {
    stdio_init_all(); // Inicializa a entrada e saída padrão (UART)
    
    adc_init(); // Inicializa o ADC (Conversor Analógico-Digital)
    adc_gpio_init(JOYSTICK_X_PIN); // Configura o pino do eixo X do joystick para ADC
    adc_gpio_init(JOYSTICK_Y_PIN); // Configura o pino do eixo Y do joystick para ADC
    
    gpio_init(JOYSTICK_BUTTON_PIN); // Inicializa o pino do botão do joystick
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN); // Configura o pino do botão do joystick como entrada
    gpio_pull_up(JOYSTICK_BUTTON_PIN); // Habilita o pull-up interno no pino do botão do joystick
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Configura a interrupção para o botão do joystick
    
    gpio_init(BUTTON_A_PIN); // Inicializa o pino do botão A
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN); // Configura o pino do botão A como entrada
    gpio_pull_up(BUTTON_A_PIN); // Habilita o pull-up interno no pino do botão A
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Configura a interrupção para o botão A

    gpio_init(LED_VERDE_PIN); // Inicializa o pino do LED verde
    gpio_set_dir(LED_VERDE_PIN, GPIO_OUT); // Configura o pino do LED verde como saída
    gpio_put(LED_VERDE_PIN, 0); // Desliga o LED verde

    init_pwm(LED_AZUL_PIN); // Inicializa o PWM para o LED azul
    init_pwm(LED_VERMELHO_PIN); // Inicializa o PWM para o LED vermelho

    i2c_init(I2C_PORT, 400 * 1000); // Inicializa o I2C com velocidade de 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura o pino SDA para função I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura o pino SCL para função I2C
    gpio_pull_up(I2C_SDA); // Habilita o pull-up interno no pino SDA
    gpio_pull_up(I2C_SCL); // Habilita o pull-up interno no pino SCL
    
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display OLED
    ssd1306_config(&ssd); // Configura o display OLED
    ssd1306_send_data(&ssd); // Envia dados para o display OLED
    ssd1306_fill(&ssd, false); // Preenche o display com a cor preta
    ssd1306_send_data(&ssd); // Envia dados para o display OLED
}

// Atualização dos LEDs com base nos valores do joystick
void atualizar_pwm_leds(void) {
    if (!leds_ativos) {
        set_led_brightness(LED_VERMELHO_PIN, 0);
        set_led_brightness(LED_AZUL_PIN, 0);
        return;
    }

    uint16_t y_valor = leitura_joystick(0);
    uint16_t x_valor = leitura_joystick(1);

    uint8_t iluminacao_x = abs(x_valor - 1893) * 16 / 1893;
    uint8_t iluminacao_y = abs(y_valor - 2099) * 16 / 2099;

    uint16_t brilho_vermelho = iluminacao_x * 4095;
    uint16_t brilho_azul = iluminacao_y * 4095;

    set_led_brightness(LED_VERMELHO_PIN, brilho_vermelho);
    set_led_brightness(LED_AZUL_PIN, brilho_azul);
}

// Leitura do valor do joystick
uint16_t leitura_joystick(uint canal) {
    adc_select_input(canal);
    return adc_read();
}

// Atualização do display com a posição do joystick
void update_display(int y, int x) {
    ssd1306_fill(&ssd, BLACK);
    if (borda_display) {
        ssd1306_fill(&ssd, !WHITE);
        ssd1306_rect(&ssd, 3, 3, 122, 60, WHITE, BLACK);
    }
    ssd1306_rect(&ssd, x, y, 8, 8, WHITE, WHITE);
    ssd1306_send_data(&ssd);
}

// Função para mapear valores
int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Manipulador de interrupção do GPIO
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if (current_time - last_time > 200000) {
        last_time = current_time;
        if (gpio == BUTTON_A_PIN) {
            leds_ativos = !leds_ativos;
        }
        if (gpio == JOYSTICK_BUTTON_PIN) {
            borda_display = !borda_display;
            gpio_put(LED_VERDE_PIN, !gpio_get(LED_VERDE_PIN));
        }
    }
}

// Inicialização do PWM
void init_pwm(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 65535);
    pwm_init(slice, &config, true);
    pwm_set_gpio_level(gpio, 0);
}

// Ajuste da intensidade dos LEDs
void set_led_brightness(uint gpio, uint16_t value) {
    pwm_set_gpio_level(gpio, value);
}
