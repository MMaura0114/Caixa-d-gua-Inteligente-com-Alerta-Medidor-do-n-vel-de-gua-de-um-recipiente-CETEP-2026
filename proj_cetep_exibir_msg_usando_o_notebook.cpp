#include <Wire.h>                 // Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h>    // Biblioteca para o Display LCD I2C

// Configuração do Display I2C (Endereço ajustado para o padrão do Tinkercad: 0x20)
LiquidCrystal_I2C lcd(0x20, 16, 2); 

// Pinos dos Sensores e Atuadores
#define TRIG_PIN 5              // Pino de disparo do sensor ultrassônico
#define ECHO_PIN 6              // Pino de recepção do sensor ultrassônico

#define LED_VERDE 2             // Pino do LED verde
#define LED_AMARELO 3           // Pino do LED amarelo
#define LED_VERMELHO 4          // Pino do LED vermelho
#define BUZZER 7                // Pino do buzzer

// Altura total do reservatório em centímetros
const float ALTURA_TOTAL_CM = 20.0;

// Variáveis de controle do buzzer (Bip intermitente dinâmico)
unsigned long ultimoBip = 0;
unsigned long intervaloBip = 500; 
int frequenciaSom = 1000;          
int duracaoSom = 100;              

void setup() {
  // Inicializa a comunicação serial com o computador.
  // 9600 é a velocidade de transmissão (Baud Rate) recomendada para o Nano e Uno.
  Serial.begin(9600); 

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Nivel");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  
  // Envia uma mensagem inicial para o computador
  Serial.println("--- SISTEMA DE MONITORAMENTO INICIADO ---");
  
  delay(2000);
  lcd.clear();
}

void loop() {
  // ===== Leitura do sensor ultrassônico =====
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
  float distancia_cm = duracao * 0.0343 / 2.0;

  // ===== Cálculo do nível da água =====
  float nivel_cm = ALTURA_TOTAL_CM - distancia_cm;
  if (nivel_cm < 0) nivel_cm = 0;
  if (nivel_cm > ALTURA_TOTAL_CM) nivel_cm = ALTURA_TOTAL_CM; 

  float porcentagem = (nivel_cm / ALTURA_TOTAL_CM) * 100.0;  

  // ===== Atualiza Display LCD =====
  lcd.setCursor(0, 0);
  lcd.print("Nivel: ");
  lcd.print((int)porcentagem);
  lcd.print("%    ");
  lcd.setCursor(0, 1);

  // Variável de texto para guardar o status atual e enviar para o PC
  String statusAtual = "";

  // ===== Lógica dos níveis, LEDs e Configuração do Buzzer =====
  if (porcentagem >= 70.0) {
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, LOW);
    lcd.print("Status: ALTO   ");
    
    statusAtual = "ALTO (Risco de transbordo!)";
    frequenciaSom = 2500;  
    duracaoSom = 400;     
    intervaloBip = 500;   

  } else if (porcentagem >= 30.0) {
    digitalWrite(LED_VERDE, LOW);   
    digitalWrite(LED_AMARELO, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
    lcd.print("Status: MEDIO  ");

    statusAtual = "MEDIO (Normal)";
    frequenciaSom = 1200;  
    duracaoSom = 150;     
    intervaloBip = 1000;  

  } else {
    digitalWrite(LED_VERDE, LOW); 
    digitalWrite(LED_AMARELO, LOW); 
    digitalWrite(LED_VERMELHO, HIGH); 
    lcd.print("Status: BAIXO  ");

    statusAtual = "BAIXO (Alerta de falta de agua)";
    frequenciaSom = 600;   
    duracaoSom = 200;     
    intervaloBip = 1500;  
  }

  // ===== ENVIO DE DADOS PARA A TELA DO COMPUTADOR =====
  // Imprime tudo em uma única linha organizada
  Serial.print("Nivel da Agua: ");
  Serial.print((int)porcentagem);
  Serial.print("% | Distancia do sensor: ");
  Serial.print(distancia_cm);
  Serial.print(" cm | Status: ");
  Serial.println(statusAtual); // O "println" quebra a linha para a próxima leitura

  // ===== Execução do Buzzer (Intermitente) =====
  unsigned long agora = millis();
  if (agora - ultimoBip >= intervaloBip) {
    ultimoBip = agora;
    tone(BUZZER, frequenciaSom, duracaoSom); 
  }

  delay(500); // Aumentado para 500ms para a leitura na tela do PC não rolar rápido demais
}
