/*
 * CAIXA MÁGICA - DIA DOS NAMORADOS
 * ESP32-C3 + Buzzer Passivo + LED RGB + Sensor LDR
 * Melodia: "Enchanted" - Taylor Swift
 */

// ============ PINOS =============
#define LDR_PIN 4          // Sensor LDR (entrada analógica)
#define BUZZER_PIN 8       // Buzzer passivo (PWM)
#define LED_R 6            // LED RGB - vermelho
#define LED_G 7            // LED RGB - verde
#define LED_B 5            // LED RGB - azul

// ============ CONFIGURAÇÕES =============
#define PWM_FREQ 5000      // Frequência PWM para buzzer (Hz)
#define LDR_THRESHOLD 2000 // Limiar de luz (ajuste conforme necessário)
#define DEBOUNCE_TIME 1000 // Tempo de espera entre ativações (ms)

// ============ VARIÁVEIS GLOBAIS =============
unsigned long lastActivation = 0;
bool boxOpened = false;

// Notas musicais em Hz - Melodia "Enchanted"
// Esta é uma simplificação da melodia principal
const int melody[] = {
  523,  // C5
  587,  // D5
  659,  // E5
  784,  // G5
  880,  // A5
  784,  // G5
  659,  // E5
  587,  // D5
  523,  // C5
  587,  // D5
  659,  // E5
  784,  // G5
  880,  // A5
  988,  // B5
  880,  // A5
  784,  // G5
  659,  // E5
  587,  // D5
  523,  // C5
};

// Durações das notas (em milissegundos)
const int noteDurations[] = {
  400, 400, 400, 400, 400, 400, 400, 400, 400,
  400, 400, 400, 400, 400, 300, 300, 400, 400, 800,
};

// Cores para alternância (RGB mais claras/pastel)
struct Color {
  int r, g, b;
};

const Color colors[] = {
  {255, 200, 220},  // Rosa claro
  {255, 240, 200},  // Pêssego
  {200, 240, 255},  // Azul claro
  {240, 200, 255},  // Roxo pastel
  {200, 255, 240},  // Turquesa
  {255, 255, 200},  // Amarelo claro
};

const int NUM_COLORS = sizeof(colors) / sizeof(colors[0]);
const int MELODY_LENGTH = sizeof(melody) / sizeof(melody[0]);

void setup() {
  Serial.begin(115200);
  
  // Configurar pinos como saída
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Configurar pino LDR como entrada
  pinMode(LDR_PIN, INPUT);
  
  // LED RGB desligado inicialmente
  setColor(0, 0, 0);
  
  Serial.println("Caixa Mágica Iniciada!");
}

void loop() {
  // Leitura do sensor LDR
  int lightLevel = analogRead(LDR_PIN);
  
  Serial.print("Luz: ");
  Serial.println(lightLevel);
  
  // Detectar abertura da caixa (quando há mudança na luz)
  if (lightLevel > LDR_THRESHOLD && !boxOpened) {
    // Verificar debounce para evitar ativações múltiplas
    if (millis() - lastActivation > DEBOUNCE_TIME) {
      boxOpened = true;
      lastActivation = millis();
      
      Serial.println(">>> CAIXA ABERTA! <<<");
      
      // Ativar sequência mágica
      playMagicSequence();
      
      boxOpened = false;
    }
  }
  
  delay(100);
}

// ============ FUNÇÃO: Toca a sequência mágica =============
void playMagicSequence() {
  int colorIndex = 0;
  
  // Toca a melodia enquanto alterna cores
  for (int i = 0; i < MELODY_LENGTH; i++) {
    // Toca a nota
    playNote(melody[i], noteDurations[i]);
    
    // Alterna a cor a cada 2 notas
    if (i % 2 == 0) {
      setColor(colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b);
      colorIndex = (colorIndex + 1) % NUM_COLORS;
    }
  }
  
  // Desligar tudo no final
  setColor(0, 0, 0);
  noTone(BUZZER_PIN);
  
  Serial.println("Sequência finalizada!");
}

// ============ FUNÇÃO: Toca uma nota no buzzer =============
void playNote(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
  delay(duration + 50); // Pequeno espaço entre notas
}

// ============ FUNÇÃO: Define cor do LED RGB =============
void setColor(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}

// ============ FUNÇÕES AUXILIARES =============
// Função tone() compatível com ESP32
void tone(int pin, int frequency, int duration) {
  ledcSetup(0, frequency, 8);           // Canal 0, frequência, resolução 8 bits
  ledcAttachPin(pin, 0);                // Anexar pino ao canal
  ledcWrite(0, 128);                    // 50% duty cycle
  delay(duration);
  ledcDetachPin(pin);
}

void noTone(int pin) {
  ledcDetachPin(pin);
  digitalWrite(pin, LOW);
}
