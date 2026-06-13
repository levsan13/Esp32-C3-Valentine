/*
 * CAIXA MÁGICA - DIA DOS NAMORADOS (Versão Áudio Hi-Fi)
 * ESP32-C3 + PCM5102A (I2S) + LED RGB + Sensor LDR
 * Melodia: "Enchanted" - Taylor Swift
 */

#include <driver/i2s.h>
#include <math.h>

// ============ PINOS LDR E LED =============
#define LDR_PIN 32         // Sensor LDR (entrada analógica segura)
#define LED_B 25           // LED RGB - azul
#define LED_R 26           // LED RGB - vermelho
#define LED_G 27           // LED RGB - verde

// ============ PINOS I2S (Módulo PCM5102A) ============
#define I2S_DIN 18         // Dados de Áudio
#define I2S_BCK 19         // Bit Clock
#define I2S_WS  23         // Word Select (L/R Clock - Ajustado para evitar conflito com OLED)

// ============ CONFIGURAÇÕES DE ÁUDIO =============
#define SAMPLE_RATE 22050
#define I2S_PORT I2S_NUM_0
#define WAVETABLE_SIZE 1024

// ============ CONFIGURAÇÕES DA CAIXA =============
#define LDR_THRESHOLD 2000 // Limiar de luz (ajuste conforme necessário)
#define DEBOUNCE_TIME 1000 // Tempo de espera entre ativações (ms)

// ============ VARIÁVEIS GLOBAIS =============
unsigned long lastActivation = 0;
bool boxOpened = false;
int16_t sineTable[WAVETABLE_SIZE]; // Tabela de ondas na memória

// ============ MELODIA: "Enchanted" ============
const int melody[] = {
  // "This night is spar-kling, don't you let it go"
  494, 392, 494, 392, 294, 440, 494, 440, 392, 330, 
  0, // Pausa
  // "I'm won-der-struck, blush-ing all the way home"
  494, 392, 494, 392, 392, 392, 370, 392, 370, 293,
  0, // Pausa
  // "I was en-chan-ted to meet you"
  494, 494, 494, 440, 392, 588, 494, 440, 392, 370, 392
};

// ============ RITMO (Suave e Aveludado) ============
const int noteDurations[] = {
  // "This night is spar-kling, don't you let it go"
  1000, 1000, 1000, 1000, 1000, 500, 500, 500, 500, 500, // check
  1000, // Pausa
  // "I'm won-der-struck, blush-ing all the way home"
  1000, 1000, 1000, 1000, 500, 500, 700, 500, 500, 700, 
  1000, // Pausa
  // "I was en-chan-ted to meet you"
  700, 700, 700, 700, 500, 800, 700, 700, 700, 700, 700
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

// ============ SETUP ============
void setup() {
  Serial.begin(115200);
  
  // Configurar pinos LDR e LEDs
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  
  setColor(0, 0, 0); // LED RGB desligado inicialmente
  
  // Inicializa o Áudio
  initI2S();
  initWavetable();
  
  Serial.println("✨ Caixa Mágica Iniciada! Aguardando abertura...");
}

// ============ LOOP PRINCIPAL ============
void loop() {
  int lightLevel = analogRead(LDR_PIN);
  
  if (lightLevel > LDR_THRESHOLD && !boxOpened) {
    if (millis() - lastActivation > DEBOUNCE_TIME) {
      boxOpened = true;
      lastActivation = millis();
      
      Serial.println(">>> 🎁 CAIXA ABERTA! Iniciando magia... <<<");
      
      playMagicSequence();
      
      boxOpened = false;
    }
  }
  
  delay(100);
}

// ============ INICIALIZAR I2S E TABELA ============
void initI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };
  
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_DIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);
}

void initWavetable() {
  for(int i = 0; i < WAVETABLE_SIZE; i++) {
    sineTable[i] = (int16_t)(sin(2.0 * M_PI * i / WAVETABLE_SIZE) * 32767);
  }
}

// ============ FUNÇÃO: SEQUÊNCIA MÁGICA =============
void playMagicSequence() {
  int colorIndex = 0;
  
  for (int i = 0; i < MELODY_LENGTH; i++) {
    // Alterna a cor a cada 2 notas
    if (i % 2 == 0) {
      setColor(colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b);
      colorIndex = (colorIndex + 1) % NUM_COLORS;
    }
    
    // Toca a nota
    playNote(melody[i], noteDurations[i]);
  }
  
  // Desligar o LED no final (o áudio silencia automaticamente)
  setColor(0, 0, 0);
}

// ============ FUNÇÃO: TOCAR NOTA =============
void playNote(int frequency, int duration) {
  // Se a frequência for 0, é uma pausa
  if (frequency == 0) {
    delay(duration);
    return;
  }

  int totalSamples = (SAMPLE_RATE / 1000) * duration;
  const int BUFFER_SIZE = 512;
  int16_t sample_buffer[BUFFER_SIZE * 2]; 
  
  uint32_t step = (frequency * 67108864ULL) / SAMPLE_RATE;
  uint32_t phase = 0;
  
  int samples_processed = 0;

  // Volume máximo
  int32_t max_amplitude = 1600; 

  while (samples_processed < totalSamples) {
    int chunk_size = (totalSamples - samples_processed < BUFFER_SIZE) ? (totalSamples - samples_processed) : BUFFER_SIZE;
    
    for (int i = 0; i < chunk_size; i++) {
      int current_sample = samples_processed + i;
      
      int16_t val = sineTable[(phase >> 16) & 1023];
      
      // Decaimento suave (Fade-out)
      int32_t current_amplitude = (max_amplitude * (totalSamples - current_sample)) / totalSamples;
      
      int16_t mixed = (int16_t)(((int32_t)val * current_amplitude) >> 15);
      
      sample_buffer[i * 2]     = mixed; 
      sample_buffer[i * 2 + 1] = mixed; 
      
      phase += step;
    }
    
    size_t bytes_written;
    i2s_write(I2S_PORT, sample_buffer, chunk_size * 4, &bytes_written, portMAX_DELAY);
    samples_processed += chunk_size;
  }
}

// ============ FUNÇÃO: DEFINE COR DO LED RGB =============
void setColor(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}