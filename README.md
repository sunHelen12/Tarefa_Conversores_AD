<h1 align="center">Tarefa Conversores Analógico-Digitais - Embarcatech 💻</h1> 
    <h2>Descrição</h2>
    <p>Este projeto utiliza um joystick analógico para controlar a intensidade dos LEDs RGB e exibir um quadrado móvel no display SSD1306. Os LEDs Azul e Vermelho ajustam seu brilho conforme os eixos Y e X, respectivamente, utilizando PWM para suavizar a variação luminosa. O botão do joystick alterna o estado do LED Verde e modifica a borda do display. O botão A ativa ou desativa os LEDs PWM. O conversor analógico-digital (ADC) do RP2040 é utilizado para capturar os valores dos eixos X e Y do joystick e convertê-los em sinais digitais para controle dos LEDs e da posição do quadrado no display.</p>
<h2>Importante⚠️</h2>
<p>Esse código foi produzido em um Sistema Operacional Linux. Certifique-se de importar o projeto adequadamente e verificar as configurações do <i>c_cpp_properties.json</i>.</p>      
<h2>Objetivos</h2>
    <ul>
        <li>Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040.</li>
        <li>Utilizar PWM para controlar a intensidade dos LEDs RGB com base nos valores do joystick.</li>
        <li>Exibir a posição do joystick no display SSD1306 através de um quadrado móvel.</li>
        <li>Aplicar o protocolo I2C para comunicação com o display.</li>
    </ul>
    <h2>Componentes Utilizados</h2>
    <ul>
        <li><strong>Joystick Analógico</strong> - GPIOs: 26 (X), 27 (Y), 22 (Botão)</li>
        <li><strong>LED RGB</strong> - GPIOs: 11 (Verde), 12 (Azul), 13 (Vermelho)</li>
        <li><strong>Botão A</strong> - GPIO: 5</li>
        <li><strong>Display OLED SSD1306</strong> - Conectado via I2C (GPIO 14 - SDA, GPIO 15 - SCL)</li>
    </ul>    
    <h2>Funcionalidades</h2>
    <ul>
        <li>O LED Azul ajusta seu brilho conforme o eixo Y do joystick.</li>
        <li>O LED Vermelho ajusta seu brilho conforme o eixo X do joystick.</li>
        <li>Os LEDs são controlados via PWM para variação suave da intensidade.</li>
        <li>O display exibe um quadrado móvel que acompanha os movimentos do joystick.</li>
        <li>O botão do joystick alterna o estado do LED Verde e modifica a borda do display.</li>
        <li>O botão A ativa ou desativa os LEDs PWM.</li>
    </ul> 
    <h2>Tecnologias Utilizadas</h2>
    <ul>
      <li>Linguagem C.</li>
      <li>IDE Visual Studio Code.</li>
      <li>Pico SDK.</li>
      <li>Placa BitDogLab.</li>
    </ul>
    <h2>Requisitos do Projeto</h2>
    <ul>
        <li><strong>Uso de interrupções</strong> - Todas as funcionalidades dos botões utilizam interrupções.</li>
        <li><strong>Debouncing</strong> - Implementado via software para evitar múltiplas leituras.</li>
        <li><strong>Display 128x64</strong> - Utilizado para exibir a posição do joystick.</li>
        <li><strong>Código estruturado e comentado</strong> - Para facilitar a compreensão e manutenção.</li>
    </ul>  
    <h2>Vídeo</h2>
      <p>Link do Vídeo: </p>
     <h2>Como Executar o Projeto</h2>
        <h2>Clone o Repositório</h2>
    <ol>
        <li>Abra o <strong>Prompt de Comando</strong> ou o terminal de sua preferência.</li>
        <li>Clone o repositório usando o Git:
            <pre><code>git clone https://github.com/seu-usuario/seu-repositorio.git</code></pre>
        </li>
        <li>Entre no diretório do projeto:
            <pre><code>cd seu-repositorio</code></pre>
        </li>
    </ol>
 <h2>Configuração do Ambiente Local</h2>
    <ol>
        <li>Baixe e instale o <a href="https://cmake.org/download/" target="_blank">CMake</a>.</li>
        <li>Configure o <strong>Pico SDK</strong> seguindo o guia oficial em 
            <a href="https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf" target="_blank">
                Raspberry Pi Pico SDK</a>.
        </li>
        <li>Crie um diretório de construção:
            <pre><code>mkdir build</code></pre>
            <pre><code>cd build</code></pre>
        </li>
        <li>Execute o CMake para gerar os arquivos de construção:
            <pre><code>cmake ..</code></pre>
        </li>
    </ol>
    
