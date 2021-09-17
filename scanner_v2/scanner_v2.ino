/*
 * CHANGELOG:
 * - code cleanup
 * - remove unused stuff: oled display
 * - optimise delay loop
 */
#include <SimpleKalmanFilter.h>

// Pin definition
#define ECHO_PIN 10
#define TRIG_PIN 11

#define PIR_PIN 12

#define LED_PIN 13

#define SDA_PIN 3
#define SCL_PIN 2

#define SOUND_SPEED 0.034

float distance, prev_distance, filtered_distance;
long duration, ontime;
int blink_delay;
bool state;

SimpleKalmanFilter simpleKalmanFilter(0.2, 0.1, 0.001);

void setup() {
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

float scan() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  return duration * SOUND_SPEED/2;
}

void blinker(int delay) {
  if (millis() - ontime > delay) {
    state = !state;
    ontime = millis();
  }
  digitalWrite(LED_PIN, state);
}

void loop() {
  distance = scan();
  if (distance - prev_distance < 1000) {
    filtered_distance = simpleKalmanFilter.updateEstimate(distance);
    Serial.print(distance); Serial.print("\t"); Serial.print(filtered_distance); Serial.print("\t"); Serial.println(digitalRead(PIR_PIN));
    prev_distance = distance;
  }
  blinker(filtered_distance * 1.5);
}
