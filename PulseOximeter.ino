#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;

#define BUZZER_PIN 8
#define FINGER_THRESHOLD 1000
#define CONFIRMATION_DURATION 60000 // 1 minute
#define ALARM_PULSE_DURATION 500

uint16_t irBuffer[100]; 
uint16_t redBuffer[100];
int32_t bufferLength = 100;
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;

unsigned long abnormalStartTime = 0;
bool alarmTriggered = false;

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("MAX30105 was not found. Check wiring."));
    while (1);
  }

  byte ledBrightness = 60;
  byte sampleAverage = 4;
  byte ledMode = 2;
  byte sampleRate = 100;
  int pulseWidth = 411; 
  int adcRange = 4096;

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void loop() {
  uint32_t currentIR = particleSensor.getIR();

  if (currentIR < FINGER_THRESHOLD) {
    Serial.println(F("Finger not detected."));
    abnormalStartTime = 0;
    alarmTriggered = false;
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
    return;
  }

  // Initial Buffer Fill
  for (byte i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false) particleSensor.check();
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  while (1) {
    // Sliding window: Shift samples
    for (byte i = 25; i < 100; i++) {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    // Get 25 new samples
    for (byte i = 75; i < 100; i++) {
      while (particleSensor.available() == false) particleSensor.check();
      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();
    }

    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

    // Alarm Logic
    bool isAbnormal = (validSPO2 && spo2 < 90) || (validHeartRate == 0);

    if (isAbnormal) {
      if (abnormalStartTime == 0) abnormalStartTime = millis();
      else if (millis() - abnormalStartTime >= CONFIRMATION_DURATION && !alarmTriggered) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(ALARM_PULSE_DURATION);
        digitalWrite(BUZZER_PIN, LOW);
        alarmTriggered = true;
      }
    } else {
      abnormalStartTime = 0;
      alarmTriggered = false;
    }

    if (particleSensor.getIR() < FINGER_THRESHOLD) break;
  }
}