#include <SimpleKalmanFilter.h>
#include <ss_oled.h>

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
SSOLED ssoled;

void setup() {
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  //oledInit(&ssoled, MY_OLED    , OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, SDA_PIN, SCL_PIN, RESET_PIN, I2C_CLK);
  oledInit(  &ssoled, OLED_128x64, 0x3C     , 0      , 0     , 0         , SDA_PIN, SCL_PIN, -1       , 400000L); // use standard I2C bus at 400Khz
  oledFill(&ssoled, 0,1);
  //oledWriteString(SSOLED *pOLED, int iScrollX, int x, int y, char *szMsg, int iSize, int bInvert, int bRender);
  oledWriteString(&ssoled, 0, 0, 0, "-JINODK-", FONT_STRETCHED, 0, 1);
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
