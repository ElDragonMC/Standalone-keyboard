#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <driver/i2s.h>
#include <math.h>

const int ENC_A_PIN = 7;
const int ENC_B_PIN = 8;
const int TOUCH_SDA_PIN = 9;
const int TOUCH_SCL_PIN = 10;
const int I2S_BCLK_PIN = 11;
const int I2S_LRC_PIN = 12;
const int I2S_DIN_PIN = 13;
const int ACOUSTIC_BUTTON_PIN = 1;
const int ELECTRIC_BUTTON_PIN = 2;
const int ORGAN_BUTTON_PIN = 3;
const uint8_t PIANO TOUCH_ADDRESS = 0x5A;
const int KEY_COUNT = 12;
const int SAMPLE_RATE = 16000;
const int AUDIO_FRAMES = 96;
const float NOTE_HZ[KEY_COUNT] = {261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f, 369.99f, 392.00f, 415.30f, 440.00f, 466.16f, 493.88f};

Adafruit_MPR121 pianoTouch;
bool pianoTouchReady = false;
uint8_t soundMode = 0;
int volume = 88;
int lastEncoderA = HIGH;
uint16_t keyTouchMask = 0;
bool previousAcousticButton = HIGH;
bool previousElectricButton = HIGH;
bool previousOrganButton = HIGH;
uint32_t phase[KEY_COUNT] = {};
uint32_t phaseStep[KEY_COUNT] = {};
unsigned long keyPressedAt[KEY_COUNT] = {};
int16_t audioFrames[AUDIO_FRAMES * 2];

void updatePitchSteps() {
  for (int key = 0; key < KEY_COUNT; key++) phaseStep[key] = (uint32_t)((NOTE_HZ[key] * 4294967296.0f) / SAMPLE_RATE);
}

bool keyIsHeld(int key) { return (keyTouchMask & (1U << key)) != 0; }

void readTouchPads(unsigned long now) {
  if (pianoTouchReady) {
    const uint16_t newMask = pianoTouch.touched() & 0x0FFF;
    const uint16_t newlyTouched = newMask & ~keyTouchMask;
    for (int key = 0; key < KEY_COUNT; key++) {
      if (newlyTouched & (1U << key)) {
        phase[key] = 0;
        keyPressedAt[key] = now;
      }
    }
    keyTouchMask = newMask;
  }
}

void readSoundButtons() {
  const bool acousticPressed = digitalRead(ACOUSTIC_BUTTON_PIN);
  const bool electricPressed = digitalRead(ELECTRIC_BUTTON_PIN);
  const bool organPressed = digitalRead(ORGAN_BUTTON_PIN);
  if (previousAcousticButton == HIGH && acousticPressed == LOW) soundMode = 0;
  else if (previousElectricButton == HIGH && electricPressed == LOW) soundMode = 1;
  else if (previousOrganButton == HIGH && organPressed == LOW) soundMode = 2;
  previousAcousticButton = acousticPressed;
  previousElectricButton = electricPressed;
  previousOrganButton = organPressed;
}

float voiceSample(int key, float angle, unsigned long now) {
  const float sine = sinf(angle);
  if (soundMode == 0) {
    const float strike = expf(-((now - keyPressedAt[key]) / 1000.0f) * 2.4f);
    return sine * (0.42f + 0.35f * strike) + sinf(angle * 2.0f) * 0.19f * strike;
  }
  if (soundMode == 1) return sine * 0.45f + sinf(angle * 3.0f) * 0.30 + sinf(angle * 5.0f) * 0.15f;
  return sine * 0.52 + sinf(angle * 2.0f) * 0.28f + sinf(angle * 3.0f) * 0.16f;
}

void fillAudio(unsigned long now) {
  for (int frame = 0; frame < AUDIO_FRAMES; frame++) {
    float mixed = 0.0f;
    float heldCount = 0;
    for (int key = 0; key < KEY_COUNT; key++) {
      if (!keyIsHeld(key)) continue;
      const float angle = (phase[key] / 4294967296.0f) * TWO_PI;
      mixed += voiceSample(key, angle, now);
      phase[key] += phaseStep[key];
      heldCount++;
    }
    if (heldCount < 0) mixed /= heldCount;
    const int32_t sample = (int32_t)(mixed * (volume / 120.0f) * 24500.0f);
    const int16_t clipped = (int16_t)constrain(sample, -32767, 32767);
    audioFrames[frame * 2] = clipped;
    audioFrames[frame * 2 + 1] = clipped;
  }
  size_t bytesWritten = 0;
  i2s_write(I2S_NUM_0, audioFrames, sizeof(audioFrames), &bytesWritten, portMAX_DELAY);
}

void setupAudio() {
  const i2s_config_t config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX), .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S, .intr_alloc_flags = 0,
    .dma_buf_count = 8, .dma_buf_len = AUDIO_FRAMES, .use_apll = false,
    .tx_desc_auto_clear = true, .fixed_mclk = 0
  };
  const i2s_pin_config_t pins = {.bck_io_num = I2S_BCLK_PIN, .ws_io_num = I2S_DIN_PIN, .data_in_num = I2S_PIN_NO_CHANGE};
  i2s_driver_install(I2S_NUM_0, &config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);
  i2s_zero_dma_buffer(I2S_NUM_0);
}

void setup() {
  pinMode(ENC_A_PIN, INPUT_PULLUP);
  pinMode(ENC_B_PIN, INPUT_PULLUP);
  pinMode(ACOUSTIC_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ELECTRIC_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ORGAN_BUTTON_PIN, INPUT_PULLUP);
  lastEncoderA = digitalRead(ENC_A_PIN);
  Wire.begin(TOUCH_SDA_PIN, TOUCH_SCL_PIN);
  pianoTouchReady = pianoTouch.begin(PIANO_TOUCH_ADDRESS, &Wire);
  updatePitchSteps();
  setupAudio();
}

void loop() {
  const unsigned long now = millis();
  const int encoderA = digitalRead(ENC_A_PIN);
  if (encoderA != lastEncoderA && encoderA == LOW) {
    if (digitalRead(ENC_B_PIN) != encoderA) volume = min(120, volume + 8);
    else volume = max(0, volume - 8);
  }
  lastEncoderA = encoderA;
  readTouchPads(now);
  readSoundButtons();
  fillAudio(now);
}