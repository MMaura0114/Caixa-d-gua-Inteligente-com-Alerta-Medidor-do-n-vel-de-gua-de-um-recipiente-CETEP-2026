#include <Wire.h>                 // Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h>    // Biblioteca para o Display LCD I2C

// Configuração do Display I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); 

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
unsigned long intervaloBip = 500; // Tempo de espera entre os bips (muda no código)
int frequenciaSom = 1000;          // Tom do som em Hz (muda no código)
int duracaoSom = 100;              // Duração de cada bipe individual (muda no código)

void setup() {
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

  // ===== Lógica dos níveis, LEDs e Configuração do Buzzer =====
  if (porcentagem >= 70.0) {
    // NÍVEL ALTO: Som super agudo, bips longos e muito rápidos (Urgência de transbordo)
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, LOW);
    lcd.print("Status: ALTO   ");

    frequenciaSom = 2500;  // Som bem agudo (super alto)
    duracaoSom = 400;     // Som duradouro (400ms ligado)
    intervaloBip = 500;   // Intervalo curto entre bips (quase contínuo)

  } else if (porcentagem >= 30.0) {
    // NÍVEL MÉDIO: Som moderado, bips normais e mais espaçados
    digitalWrite(LED_VERDE, LOW);   
    digitalWrite(LED_AMARELO, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
    lcd.print("Status: MEDIO  ");

    frequenciaSom = 1200;  // Tom médio padrão
    duracaoSom = 150;     // Duração moderada
    intervaloBip = 1000;  // Espaçamento calmo (bipa a cada 1 segundo)

  } else {
    // NÍVEL BAIXO: Frequência razoável de alerta (Alerta lento, tom grave)
    digitalWrite(LED_VERDE, LOW); 
    digitalWrite(LED_AMARELO, LOW); 
    digitalWrite(LED_VERMELHO, HIGH); 
    lcd.print("Status: BAIXO  ");

    frequenciaSom = 600;   // Tom mais grave/razoável
    duracaoSom = 200;     // Duração curta
    intervaloBip = 1500;  // Bem espaçado (bipa a cada 1,5 segundos)
  }

  // ===== Execução do Buzzer (Intermitente sem travar o código) =====
  unsigned long agora = millis();
  if (agora - ultimoBip >= intervaloBip) {
    ultimoBip = agora;
    tone(BUZZER, frequenciaSom, duracaoSom); // Toca o som com a frequência e duração definidas
  }

  delay(100); 
}
