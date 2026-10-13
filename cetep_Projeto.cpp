#include <Wire.h>                 // Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h>    // Biblioteca para o Display LCD I2C

// Configuração do Display I2C (Endereço comum: 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16 colunas e 2 linhas

// Pinos dos Sensores e Atuadores
#define TRIG_PIN 5              // Pino de disparo do sensor ultrassônico
#define ECHO_PIN 18             // Pino de recepção do sensor ultrassônico

#define LED_VERDE 19            // Pino do LED verde
#define LED_AMARELO 4           // Pino do LED amarelo
#define LED_VERMELHO 2          // Pino do LED vermelho
#define BUZZER 15               // Pino do buzzer

// Altura total do reservatório em centímetros (ajuste para seu recipiente)
const float ALTURA_TOTAL_CM = 20.0;

// Variáveis de controle do buzzer (bip intermitente)
unsigned long ultimoBip = 0;
const unsigned long INTERVALO_BIP = 500; // 500 ms
bool estadoBuzzer = false;

void setup() {
  Serial.begin(115200); // Inicializa a comunicação serial para depuração

  pinMode(TRIG_PIN, OUTPUT);// Configura o pino TRIG como saída
  pinMode(ECHO_PIN, INPUT);// Configura o pino ECHO como entrada

  pinMode(LED_VERDE, OUTPUT);// Configura o pino do LED verde como saída
  pinMode(LED_AMARELO, OUTPUT);// Configura o pino do LED amarelo como saída
  pinMode(LED_VERMELHO, OUTPUT);// Configura o pino do LED vermelho como saída
  pinMode(BUZZER, OUTPUT);// Configura o pino do buzzer como saída

  // Inicialização do Display
  lcd.init(); // Inicializa o display LCD
  lcd.backlight();// Ativa a luz de fundo do display
  lcd.setCursor(0, 0);// Define a posição do cursor (coluna 0, linha 0)
  lcd.print("Sistema Nivel");// Exibe mensagem inicial
  lcd.setCursor(0, 1);// Define a posição do cursor (coluna 0, linha 1)
  lcd.print("Iniciando...");// Exibe mensagem inicial
  delay(2000);// Aguarda 2 segundos para leitura inicial
  lcd.clear();// Limpa o display para iniciar a leitura
}

void loop() {
  // ===== Leitura do sensor ultrassônico =====
  digitalWrite(TRIG_PIN, LOW);// Garante que o pino TRIG esteja baixo
  delayMicroseconds(2);// Aguarda 2 microssegundos
  digitalWrite(TRIG_PIN, HIGH);// Dispara o pulso ultrassônico
  delayMicroseconds(10);// Mantém o pino TRIG alto por 10 microssegundos
  digitalWrite(TRIG_PIN, LOW);// Retorna o pino TRIG para baixo

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);// Aguarda o pulso de retorno (timeout de 30 ms)
  float distancia_cm = duracao * 0.0343 / 2.0;// Calcula a distância em centímetros (velocidade do som: 343 m/s)

  // ===== Cálculo do nível da água =====
  float nivel_cm = ALTURA_TOTAL_CM - distancia_cm;// Calcula o nível da água com base na altura total do reservatório
  if (nivel_cm < 0) nivel_cm = 0;// Garante que o nível não seja negativo
  if (nivel_cm > ALTURA_TOTAL_CM) nivel_cm = ALTURA_TOTAL_CM;// Garante que o nível não ultrapasse a altura total do reservatório 

  float porcentagem = (nivel_cm / ALTURA_TOTAL_CM) * 100.0;// Calcula a porcentagem do nível da água em relação à altura total do reservatório  

  // ===== Atualiza Display LCD =====
  lcd.setCursor(0, 0);// Define a posição do cursor (coluna 0, linha 0)
  lcd.print("Nivel: ");// Exibe a palavra "Nivel: "
  lcd.print((int)porcentagem);// Exibe a porcentagem do nível da água (convertida para inteiro)
  lcd.print("%    ");// Exibe o símbolo de porcentagem e limpa o restante da linha

  lcd.setCursor(0, 1);// Define a posição do cursor (coluna 0, linha 1)

  // ===== Lógica dos níveis e alertas =====
  if (porcentagem >= 70.0) {
    // Nível Alto
    digitalWrite(LED_VERDE, HIGH);// Acende o LED verde
    digitalWrite(LED_AMARELO, LOW);// Apaga o LED amarelo
    digitalWrite(LED_VERMELHO, LOW);// Apaga o LED vermelho
    digitalWrite(BUZZER, LOW);// Desliga o buzzer
    estadoBuzzer = false;// Garante que o estado do buzzer esteja desligado
    lcd.print("Status: ALTO   ");// Exibe o status "ALTO" no display

  } else if (porcentagem >= 30.0) {
    // Nível Médio
    digitalWrite(LED_VERDE, LOW);// Apaga o LED verde   
    digitalWrite(LED_AMARELO, HIGH);// Acende o LED amarelo
    digitalWrite(LED_VERMELHO, LOW);// Apaga o LED vermelho
    digitalWrite(BUZZER, LOW);// Desliga o buzzer
    estadoBuzzer = false;// Garante que o estado do buzzer esteja desligado
    lcd.print("Status: MEDIO  ");// Exibe o status "MEDIO" no display

  } else {
    // Nível Baixo
    digitalWrite(LED_VERDE, LOW); // Apaga o LED verde
    digitalWrite(LED_AMARELO, LOW); // Apaga o LED amarelo
    digitalWrite(LED_VERMELHO, HIGH); // Acende o LED vermelho
    lcd.print("Status: BAIXO  "); // Exibe o status "BAIXO" no display

    // Bip intermitente a cada 500 ms (sem bloquear o loop)
    unsigned long agora = millis(); // Obtém o tempo atual em milissegundos
    if (agora - ultimoBip >= INTERVALO_BIP) {
      ultimoBip = agora; // Atualiza o tempo do último bip
      estadoBuzzer = !estadoBuzzer; // Alterna o estado do buzzer
      digitalWrite(BUZZER, estadoBuzzer); // Liga ou desliga o buzzer com base no estado atual
    }
  }

  delay(100); // Pequena pausa para estabilidade das leituras
}