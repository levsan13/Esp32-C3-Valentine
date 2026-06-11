/*
 * VERSÃO ALTERNATIVA - MAIS SIMPLES (sem LDR)
 * Se preferir usar um botão ao invés de detectar luz
 */

#define BOTAO_PIN 2         // Botão para ativar
#define BUZZER_PIN 8
#define LED_R 6
#define LED_G 7
#define LED_B 5

unsigned long lastPress = 0;

// Melodia Enchanted - Versão Simplificada (apenas a introdução famosa)
const int melodySimple[] = {
  587,  // D
  659,  // E
  784,  // G
  880,  // A
  784,  // G
  659,  // E
  587,  // D
  659,  // E
  784,  // G
  880,  // A
  988,  // B
  880,  // A
};

const int durationSimple[] = {
  300, 300, 300, 400, 200, 200, 300, 300, 300, 300, 400, 600
};

struct Color {
  int r, g, b;
};

const Color colors[] = {
  {255, 200, 220},  // Rosa
  {255, 240, 200},  // Pêssego
  {200, 240, 255},  // Azul claro
  {240, 200, 255},  // Roxo
};

const int NUM_COLORS = 4;
const int MELODY_LEN = 12;

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BOTAO_PIN, INPUT_PULLUP);
  
  setColor(0, 0, 0);
  Serial.println("Caixa Mágica com Botão - Pronta!");
}

void loop() {
  // Detectar pressão do botão
  if (digitalRead(BOTAO_PIN) == LOW) {
    if (millis() - lastPress > 1000) {
      lastPress = millis();
      Serial.println("Botão pressionado!");
      playSequence();
    }
  }
  delay(100);
}

void playSequence() {
  int colorIndex = 0;
  
  for (int i = 0; i < MELODY_LEN; i++) {
    // Toca nota
    tone(BUZZER_PIN, melodySimple[i], durationSimple[i]);
    
    // Alterna cor
    setColor(colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b);
    colorIndex = (colorIndex + 1) % NUM_COLORS;
    
    delay(durationSimple[i] + 100);
  }
  
  setColor(0, 0, 0);
  noTone(BUZZER_PIN);
}

void setColor(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}

void tone(int pin, int frequency, int duration) {
  ledcSetup(0, frequency, 8);
  ledcAttachPin(pin, 0);
  ledcWrite(0, 128);
  delay(duration);
  ledcDetachPin(pin);
}

void noTone(int pin) {
  ledcDetachPin(pin);
  digitalWrite(pin, LOW);
}

// ========================================================================
// VERSÃO COM EFEITOS EXTRAS
// Adicionar estas funções à versão principal para mais criatividade
// ========================================================================

/*
// EFEITO 1: Fade de cores (transição suave)
void fadeColor(int r1, int g1, int b1, int r2, int g2, int b2, int steps) {
  for (int i = 0; i <= steps; i++) {
    int r = r1 + (r2 - r1) * i / steps;
    int g = g1 + (g2 - g1) * i / steps;
    int b = b1 + (b2 - b1) * i / steps;
    setColor(r, g, b);
    delay(30);
  }
}

// EFEITO 2: Pulso de cor (brilho pulsante)
void pulseColor(int r, int g, int b, int times) {
  for (int t = 0; t < times; t++) {
    for (int brightness = 0; brightness < 255; brightness += 5) {
      setColor(r * brightness / 255, g * brightness / 255, b * brightness / 255);
      delay(10);
    }
    for (int brightness = 255; brightness > 0; brightness -= 5) {
      setColor(r * brightness / 255, g * brightness / 255, b * brightness / 255);
      delay(10);
    }
  }
  setColor(0, 0, 0);
}

// EFEITO 3: Sequência de cores com ritmo do buzzer
void rhythmicSequence() {
  Color[] beatColors = {{255, 100, 150}, {150, 200, 255}, {200, 150, 255}};
  int[] beatFreqs = {523, 659, 784, 880};
  
  for (int beat = 0; beat < 8; beat++) {
    setColor(beatColors[beat % 3].r, beatColors[beat % 3].g, beatColors[beat % 3].b);
    tone(BUZZER_PIN, beatFreqs[beat % 4], 150);
    delay(200);
  }
  setColor(0, 0, 0);
}

// EFEITO 4: Arco-íris suave
void softRainbow(int cycles) {
  Color rainbow[] = {
    {255, 100, 150},  // Rosa
    {255, 150, 100},  // Coral
    {255, 200, 100},  // Pêssego
    {200, 255, 100},  // Amarelo
    {100, 255, 150},  // Verde
    {100, 200, 255},  // Azul
    {150, 100, 255},  // Roxo
  };
  
  for (int c = 0; c < cycles; c++) {
    for (int i = 0; i < 7; i++) {
      setColor(rainbow[i].r, rainbow[i].g, rainbow[i].b);
      delay(400);
    }
  }
  setColor(0, 0, 0);
}
*/

// ========================================================================
// VERSÃO COM LDR AJUSTÁVEL
// Use este código para calibrar o LDR automaticamente
// ========================================================================

/*
#define LDR_PIN 4

void calibrateLDR() {
  Serial.println("=== CALIBRAÇÃO LDR ===");
  Serial.println("Feche a caixa (deixe no escuro) e pressione qualquer tecla");
  while(Serial.available() == 0);
  
  int darkValue = analogRead(LDR_PIN);
  Serial.print("Valor no escuro: ");
  Serial.println(darkValue);
  
  Serial.println("Agora abra a caixa na luz e pressione qualquer tecla");
  Serial.readString(); // limpar buffer
  while(Serial.available() == 0);
  
  int brightValue = analogRead(LDR_PIN);
  Serial.print("Valor na luz: ");
  Serial.println(brightValue);
  
  int threshold = (darkValue + brightValue) / 2;
  Serial.print("Use como LDR_THRESHOLD: ");
  Serial.println(threshold);
}

// Chamar em setup(): calibrateLDR();
*/

// ========================================================================
// DICAS DE OTIMIZAÇÃO
// ========================================================================

/*
1. ECONOMIA DE ENERGIA:
   - Use sleep mode entre ativações: esp_light_sleep_start()
   - Reduza o brilho do LED se usar bateria

2. MELODIA MAIS LONGA:
   - Adicione mais notas aos arrays
   - Considere armazenar em PROGMEM para economizar RAM:
     const int melody[] PROGMEM = {...};

3. MÚLTIPLOS EFEITOS:
   - Crie funções separadas para cada efeito
   - Use variáveis para escolher qual efeito rodar

4. SINCRONIZAÇÃO MÚSICA-COR:
   - Calcule a cor baseada na nota (cores diferentes = notas diferentes)
   - Crie um mapa de frequência para cor

5. SENSOR MAIS PRECISO:
   - Faça múltiplas leituras e tire média: analogReadAvg()
   - Use filtro de suavização: newValue = 0.8 * old + 0.2 * new
*/
