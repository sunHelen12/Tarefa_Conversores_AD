#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/time.h"

// Configurando fontes
#include "inc/ssd1306.h"
#include "inc/font.h"

// Configurando o display
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Configurando o joystick
#define JOYSTICK_X_PIN 26 // GPIO para eixo x
#define JOYSTICK_Y_PIN 27 // GPIO para eixo y
#define JOYSTICK_BUTTON_PIN 22 // GPIO para botão do Joystick

// Configurando do botão
#define BUTTON_A_PIN 5 // GPIO para botão

// Configurando LEDs
#define LED_VERDE_PIN 11 // GPIO para LED verde
#define LED_AZUL_PIN 12 // GPIO para LED azul
#define LED_VERMELHO_PIN 13 // GPIO para LED vermelho

// Valores iniciais e constantes
#define ADC_MAX_VALUE 4095
#define JOYSTICK_CENTER_VALUE 2048
#define BLACK 0
#define WHITE 1

// Variáveis globais
static volatile uint32_t last_time = 0; // Armazena o último evento de tempo (microssegundos)
static volatile bool leds_ativos = true; // Armazena o estado dos LEDs
static volatile bool borda_display = false; // Alterna a borda do display
ssd1306_t ssd; // Para armazenar o estado do display

// Prototipos das rotinas
void init_hardware(void);
void update_display(int x, int y);
int map(int x, int in_min, int in_max, int out_min, int out_max);
void gpio_irq_handler(uint gpio, uint32_t events);

// Rotina principal
int main() {
    init_hardware();

    while (true) {
        // Leitura do ADC do eixo X e Y do joystick
        adc_select_input(0); // Escolhe o canal do eixo x
        uint16_t adc_x = adc_read(); // Leitura do eixo x
        adc_select_input(1); // Escolhe o canal do eixo y
        uint16_t adc_y = adc_read(); // Leitura do eixo y

        // Mapeamos os valores do joystick para o display
        int pos_x = map(adc_y, 0, ADC_MAX_VALUE, 0, WIDTH - 8);
        int pos_y = map(adc_x, 0, ADC_MAX_VALUE, HEIGHT - 8, 0);

        update_display(pos_x, pos_y);

        sleep_ms(50); // Aguarda 50ms para atualizar o display
    }
}

// Inicializa o hardware
void init_hardware() {
    stdio_init_all();

    // Inicializa o ADC e o joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);
    // Configura a interrupção do botão do joystick
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Inicializa o botão A
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    // Configura a interrupção do botão A
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Inicializa os LEDs
    gpio_init(LED_VERDE_PIN);
    gpio_set_dir(LED_VERDE_PIN, GPIO_OUT);
    gpio_put(LED_VERDE_PIN, 0); // Desliga o LED verde

    gpio_init(LED_AZUL_PIN);
    gpio_set_dir(LED_AZUL_PIN, GPIO_OUT);
    gpio_put(LED_AZUL_PIN, 0); // Desliga o LED azul

    gpio_init(LED_VERMELHO_PIN);
    gpio_set_dir(LED_VERMELHO_PIN, GPIO_OUT);
    gpio_put(LED_VERMELHO_PIN, 0); // Desliga o LED vermelho

    // Configura o I2C para o SSD1306
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // Inicializa o display OLED
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Atualiza o display com a posição do joystick
void update_display(int x, int y) {
    ssd1306_fill(&ssd, BLACK); // Preenche o display com a cor preta
    
    // Pressiona o botão do joystick
    if (borda_display) {
        ssd1306_fill(&ssd, !WHITE); // Preenche o display com a cor inversa
        ssd1306_rect(&ssd, 3, 3, 122, 60, WHITE, BLACK); // Desenha a borda do display
    }

    
    ssd1306_rect(&ssd, y, x, 8, 8, WHITE, WHITE); // Desenha o quadrado
    ssd1306_send_data(&ssd); // Envia os dados para o display
}

// Função para mapear valores
int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Rotina de interrupção
void gpio_irq_handler(uint gpio, uint32_t events) {
    // Armazena o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Verifica se passou tempo suficiente desde o último evento (debouncing)
    if (current_time - last_time > 200000) { // 200ms de debounce
        last_time = current_time; // Atualiza o tempo do último evento
        if (gpio == BUTTON_A_PIN) { // Botão A pressionado
            leds_ativos = !leds_ativos; // Inverte o estado dos LEDs

            if (!leds_ativos) { // Se os LEDs foram desativados, apaga todos
                gpio_put(LED_AZUL_PIN, 0); // Desliga o LED azul
                gpio_put(LED_VERMELHO_PIN, 0); // Desliga o LED vermelho
                gpio_put(LED_VERDE_PIN, 0); // Desliga o LED verde
            } else {
                gpio_put(LED_AZUL_PIN, 1); // Liga o LED azul
                gpio_put(LED_VERMELHO_PIN, 1); // Liga o LED vermelho
            }
        }

        if (gpio == JOYSTICK_BUTTON_PIN) { // Botão do joystick pressionado
            borda_display = !borda_display; // Alterna a borda do display
            gpio_put(LED_VERDE_PIN, !gpio_get(LED_VERDE_PIN)); // Alterna o estado do LED verde
        }
    }
}