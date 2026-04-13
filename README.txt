Smart Pulse Oximeter with False-Alarm Mitigation
This project implements a high-sensitivity SpO2 and Heart Rate monitor using an Arduino and the MAX30105 optical sensor. It is designed to provide stable, real-time health metrics while filtering out the common "nuisance alarms" found in low-cost pulse oximetry devices.

 Hardware Architecture
The system relies on a high-precision optical path to measure blood oxygenation levels non-invasively.

Microcontroller: Arduino Uno/Nano (ATmega328P).

Sensor: MAX30105 — An integrated "Particle Sensor" that combines two LEDs (Red and IR) with a sensitive photodetector. It is significantly more precise than the standard MAX30102 for pulse oximetry.

Alert System: Active Piezo Buzzer (Pin 8) for auditory notifications.

Communication: I2C protocol at 400kHz (Fast Mode) for low-latency data transmission.

Software Logic & Signal Processing
1. The Maxim Integrated Algorithm
The project utilizes the maxim_heart_rate_and_oxygen_saturation algorithm. This is a robust signal processing method that:

Analyzes a 100-sample buffer of both Red and IR data.

Uses a sliding window approach: instead of waiting for a full 100-sample refresh, the system shifts the buffer by 25 samples every iteration. This provides near-continuous updates and a smoother user experience.

2. False-Alarm Mitigation (The 60s Rule)
Standard SpO2 sensors are prone to "motion artifacts"—momentary drops in oxygen readings caused by the finger shifting or the sensor losing contact. To address this, I implemented a Confirmation Timer:

Threshold: If SpO2 drops below 90% or the Heart Rate signal is lost, the code initiates a CONFIRMATION_DURATION (60,000ms).

The Alarm: An audible buzzer pulse triggers only if the abnormality persists for a full, continuous minute.

Safety Logic: If the reading stabilizes for even one sample during that minute, the timer is instantly reset. This ensures that the user is not disturbed by momentary noise, but is alerted to genuine, sustained physiological distress.

Setup & Pinout

MAX30105 Pin          Arduino Pin                       Description
VCC,                           3.3V                            Power Supply
GND                           GND                              Ground
SDA                            A4                                  I2C Data
SCL                             A5                                 I2C Clock
Buzzer (+)                   Pin 8                              Alert Trigger