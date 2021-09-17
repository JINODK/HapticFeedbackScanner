/*
 * CHANGELOG:
 * - initial development
 */
#include <SimpleKalmanFilter.h>
#include <ss_oled.h>

// Pin definition
#define ECHO_PIN 10
#define TRIG_PIN 11

#define PIR_PIN 12

#define LED_PIN 13

#define SDA_PIN 3
#define SCL_PIN 2
#define RESET_PIN -1

#define OLED_ADDR 0x3C
#define FLIP180 0
#define INVERT 0

#define SOUND_SPEED 0.034

long duration, ontime;
float distanceCm, prev_distanceCm;
int blink_delay;
bool state;

SimpleKalmanFilter simpleKalmanFilter(0.1, 0.1, 0.001);
SSOLED ssoled;

void setup() {
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  oledInit(&ssoled, OLED_128x64, OLED_ADDR, FLIP180, INVERT, 0, SDA_PIN, SCL_PIN, RESET_PIN, 400000L); // use standard I2C bus at 400Khz
  oledFill(&ssoled, 0,1);
  oledWriteString(&ssoled, 0, 0, 0, "-JINODK-", FONT_STRETCHED, 0, 1);
}

void scan() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  prev_distanceCm = distanceCm;
  distanceCm = duration * SOUND_SPEED/2;
}

void blinker() {
  if (millis() - ontime > blink_delay) {
    state = !state;
    ontime = millis();
  }
  digitalWrite(LED_PIN, state);
}

void loop() {
  scan();
  if (distanceCm - prev_distanceCm < 1000) {
    Serial.print(distanceCm); Serial.print("\t"); Serial.print(simpleKalmanFilter.updateEstimate(distanceCm)); Serial.print("\t"); Serial.println(digitalRead(PIR_PIN));
    blink_delay = distanceCm * 1.5;
    if (blink_delay < 0) blink_delay = 0;
  } else {
    distanceCm = prev_distanceCm;
  }
  blinker();
  delay(10);
}
