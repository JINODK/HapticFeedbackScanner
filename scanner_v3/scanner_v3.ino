/*
 * CHANGELOG:
 * - added mode switch button
 * - added PIR human detection
 */

#include <SimpleKalmanFilter.h>

// Pin definition
#define ECHO_PIN 10
#define TRIG_PIN 11

#define PIR_PIN 12

#define VIBRATOR_PIN 13 //no not-safe-for-work pls

#define MODE_SW A0

#define SOUND_SPEED 0.034

float distance, prev_distance, filtered_distance;
long duration, ontime;
int blink_delay, button_duration;
bool state, mode;

SimpleKalmanFilter simpleKalmanFilter(0.2, 0.1, 0.001);

void setup() {
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(VIBRATOR_PIN, OUTPUT);
  pinMode(MODE_SW, INPUT_PULLUP);
}

void check_button() {
  if (!digitalRead(MODE_SW)) {
    if (button_duration > 100) {
      mode = !mode;
      button_duration = 0;
      digitalWrite(VIBRATOR_PIN, HIGH);
      delay(500);
      digitalWrite(VIBRATOR_PIN, LOW);
    } else {
      button_duration++;
    }
  } else {
    button_duration = 0;
  }
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
  digitalWrite(VIBRATOR_PIN, state);
}

void loop() {
  check_button();
  if (mode) {
    digitalWrite(VIBRATOR_PIN, digitalRead(PIR_PIN));
  } else {
    distance = scan();
    if (distance - prev_distance < 1000) {
      filtered_distance = simpleKalmanFilter.updateEstimate(distance);
      //Serial.print(distance); Serial.print("\t"); Serial.print(filtered_distance); Serial.print("\t"); Serial.println(digitalRead(PIR_PIN));
      //Serial.println(digitalRead(PIR_PIN));
      prev_distance = distance;
    }
    blinker(filtered_distance * 1.5);
  }
}
