#include <Wire.h> // Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h> // Biblioteca para o Display LCD I2C

// Configuração do Display I2C (Endereço comum: 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16 colunas e 2 linhas

// Pinos dos Sensores e Atuadores 
#define TRIG_PIN 5 // Pino de disparo do sensor ultrassônico
#define ECHO_PIN 18 // Pino de recepção do sensor ultrassônico

#define LED_VERDE 19 // Pino do LED verde
#define LED_AMARELO 4 // Pino do LED amarelo
#define LED_VERMELHO 2 // Pino do LED vermelho
#define BUZZER 15 // Pino do buzzer

// Altura total do reservatório em centímetros (ajuste para seu recipiente)
const float ALTURA_TOTAL_CM = 20.0; // Altura total do reservatório em centímetros

void setup() {
  Serial.begin(115200); // Inicializa a comunicação serial para depuração

  pinMode(TRIG_PIN, OUTPUT); // Configura o pino TRIG como saída
  pinMode(ECHO_PIN, INPUT); // Configura o pino ECHO como entrada

  pinMode(LED_VERDE, OUTPUT); // Configura o pino LED_VERDE como saída
  pinMode(LED_AMARELO, OUTPUT); // Configura o pino LED_AMARELO como saída
  pinMode(LED_VERMELHO, OUTPUT); // Configura o pino LED_VERMELHO como saída
  pinMode(BUZZER, OUTPUT); // Configura o pino BUZZER como saída

  // Inicialização do Display
  lcd.init(); // Inicializa o display LCD
  lcd.backlight(); // Ativa a luz de fundo do display
  lcd.setCursor(0, 0); // Define a posição do cursor (coluna 0, linha 0)
  lcd.print("Sistema Nivel"); // Exibe mensagem inicial no display
  lcd.setCursor(0, 1); // Define a posição do cursor (coluna 0, linha 1)
  lcd.print("Iniciando..."); // Exibe mensagem inicial no display
  delay(2000); // Aguarda 2 segundos para a mensagem inicial
  lcd.clear(); // Limpa o display após a mensagem inicial
}

void loop() {
  // Leitura do sensor ultrassônico
  digitalWrite(TRIG_PIN, LOW); // Garante que o pino TRIG esteja em LOW
  delayMicroseconds(2); // Aguarda 2 microssegundos para estabilizar o sensor 
  digitalWrite(TRIG_PIN, HIGH); // Envia um pulso HIGH para o pino TRIG
  delayMicroseconds(10); // Mantém o pino TRIG em HIGH por 10 microssegundos
  digitalWrite(TRIG_PIN, LOW); // Retorna o pino TRIG para LOW

  long duracao = pulseIn(ECHO_PIN, HIGH); // Mede o tempo que o pulso leva para retornar ao pino ECHO   
  float distancia_cm = duracao * 0.0343 / 2; // Converte o tempo em distância (cm)  

  // Cálculo do nível da água
  float nivel_cm = ALTURA_TOTAL_CM - distancia_cm; // Calcula o nível da água subtraindo a distância medida da altura total do reservatório 
  if (nivel_cm < 0) nivel_cm = 0; // Garante que o nível não seja negativo  
  if (nivel_cm > ALTURA_TOTAL_CM) nivel_cm = ALTURA_TOTAL_CM; // Garante que o nível não ultrapasse a altura total do reservatório  

  float porcentagem = (nivel_cm / ALTURA_TOTAL_CM) * 100; // Converte o nível em porcentagem  

  // Atualiza Display LCD
  lcd.setCursor(0, 0); // Define a posição do cursor (coluna 0, linha 0)
  lcd.print("Nivel: "); // Exibe a palavra "Nivel: " no display 
  lcd.print(porcentagem, 0); // Exibe a porcentagem do nível de água no display sem casas decimais  
  lcd.print("%    "); // Adiciona espaços para limpar qualquer valor anterior no display

  lcd.setCursor(0, 1); // Define a posição do cursor (coluna 0, linha 1)

  // Lógica dos níveis e alertas
  if (porcentagem >= 70.0) {
    // Nível Alto
    digitalWrite(LED_VERDE, HIGH); // Acende o LED verde
    digitalWrite(LED_AMARELO, LOW); // Apaga o LED amarelo
    digitalWrite(LED_VERMELHO, LOW); // Apaga o LED vermelho
    digitalWrite(BUZZER, LOW); // Desliga o buzzer
    lcd.print("Status: ALTO   "); // Exibe "Status: ALTO" no display
  } else if (porcentagem >= 30.0 && porcentagem < 70.0) {
    // Nível Médio
    digitalWrite(LED_VERDE, LOW); // Apaga o LED verde
    digitalWrite(LED_AMARELO, HIGH); // Acende o LED amarelo
    digitalWrite(LED_VERMELHO, LOW); // Apaga o LED vermelho
    digitalWrite(BUZZER, LOW); // Desliga o buzzer
    lcd.print("Status: MEDIO  "); // Exibe "Status: MEDIO" no display
  } else {
    // Nível Baixo
    digitalWrite(LED_VERDE, LOW); // Apaga o LED verde
    digitalWrite(LED_AMARELO, LOW); // Apaga o LED amarelo
    digitalWrite(LED_VERMELHO, HIGH); // Acende o LED vermelho
    lcd.print("Status: BAIXO  "); // Exibe "Status: BAIXO" no display

    // Bip intermitente no Buzzer
    digitalWrite(BUZZER, HIGH); // Liga o buzzer
    delay(100); // Aguarda 100 milissegundos  
    digitalWrite(BUZZER, LOW); // Desliga o buzzer
  }

  delay(500); // Aguarda 500 milissegundos antes da próxima leitura do sensor   
}
