// etrike
// Author: MAD Walker
// YouTube Channel: https://www.youtube.com/channel/UCv2wknond0LmUBsUUq1JKIw
// This sketch is used in the DIY Arduino Controlled Electric Tricycle for kids. This project is created for personal and demo purpose.
// Part 1 (Demo): https://www.youtube.com/watch?v=KHLfpjzK5O8
// Part 2 (How it's made) : https://www.youtube.com/watch?v=eJtuKRBo77M

#include <NewPing.h>

#define F_TRIGGER_PIN   6
#define F_ECHO_PIN      7
#define R_TRIGGER_PIN   11
#define R_ECHO_PIN      12
#define MAX_DISTANCE    250

#define FW_R_SW         8

#define MOTOR_L_DIR     A4
#define MOTOR_L_BRK     A3
#define MOTOR_R_DIR     A2
#define MOTOR_R_BRK     13
#define MOTOR_R_PWM     3
#define MOTOR_L_PWM     5

#define THROTTLE        A0
#define THROTTLE_MIN    180
#define THROTTLE_MAX    800

#define HANDLE_BAR_POS  A5

NewPing f_sonar(F_TRIGGER_PIN, F_ECHO_PIN, MAX_DISTANCE);
NewPing r_sonar(R_TRIGGER_PIN, R_ECHO_PIN, MAX_DISTANCE);

int distanceCM = 0;
int throttleVal = 0;
int motorSpeed = 0;
int currentMotorSpeed = 0;
int prevMotorSpeed = 0;
int handleBarLPosition = 0;
int handleBarRPosition = 0;
int handleBarADC = 0;
int leftOffset = 0;
int rightOffset = 0;

int fwRSwState = LOW;
int forwardDir = HIGH;

//TIMERS
unsigned long sonicStartMillis;
unsigned long sonicCurrentMillis;
unsigned long sonicFreqMillis = 50;

unsigned long throttleStartMillis;
unsigned long throttleCurrentMillis;
unsigned long throttleFreqMillis = 10;

void setup() {
  pinMode(FW_R_SW, INPUT);

  pinMode(MOTOR_L_DIR, OUTPUT);
  pinMode(MOTOR_L_BRK, OUTPUT);
  pinMode(MOTOR_R_DIR, OUTPUT);
  pinMode(MOTOR_R_BRK, OUTPUT);
  pinMode(MOTOR_R_PWM, OUTPUT);
  pinMode(MOTOR_L_PWM, OUTPUT);

  pinMode(THROTTLE, INPUT);
  pinMode(HANDLE_BAR_POS, INPUT);

  digitalWrite(MOTOR_L_DIR, LOW);
  digitalWrite(MOTOR_L_BRK, LOW);
  digitalWrite(MOTOR_R_DIR, LOW);
  digitalWrite(MOTOR_R_BRK, LOW);
  digitalWrite(MOTOR_R_PWM, LOW);
  digitalWrite(MOTOR_L_PWM, LOW);

  sonicStartMillis = millis();
  throttleStartMillis = millis();
  delay(500);
}

void loop() {
  getDistance();
  getHandleBarPosition();
  getThrottle();
}

void getThrottle() {
  throttleCurrentMillis = millis();
  throttleVal = analogRead(THROTTLE);
  motorSpeed = map(throttleVal, THROTTLE_MIN, THROTTLE_MAX, 0, 255);

  if (prevMotorSpeed == 0) {
    digitalWrite(MOTOR_L_DIR, fwRSwState);
    digitalWrite(MOTOR_R_DIR, fwRSwState);
  }

  if (throttleCurrentMillis - throttleStartMillis >= throttleFreqMillis) {
    throttleStartMillis = throttleCurrentMillis;
    setMotorSpeed();
  }
}

void getHandleBarPosition() {
  handleBarADC = analogRead(HANDLE_BAR_POS);
  handleBarLPosition = map(handleBarADC, 501, 750, 0, prevMotorSpeed - 75);
  handleBarRPosition = map(handleBarADC, 500, 210, 0, prevMotorSpeed - 75);
}

void setMotorSpeed() {
  leftOffset = 0;
  rightOffset = 0;
  if (prevMotorSpeed == 0) {
    fwRSwState = digitalRead(FW_R_SW);
  }

  if (motorSpeed > 100 && fwRSwState == HIGH) {
    motorSpeed = 100;
  }

  if (prevMotorSpeed <= motorSpeed && prevMotorSpeed < 255) {
    currentMotorSpeed++;
  }
  else if (prevMotorSpeed > 0) {
    currentMotorSpeed = currentMotorSpeed - 2;
  }

  if (handleBarLPosition > 0) {
    leftOffset = handleBarLPosition;
  }

  if (handleBarRPosition > 0) {
    rightOffset = handleBarRPosition;
  }

  if (prevMotorSpeed - leftOffset < 0) {
    leftOffset = 0;
  }

  if (prevMotorSpeed - rightOffset < 0) {
    rightOffset = 0;
  }

  if (distanceCM!=0 && distanceCM < 75) {
    currentMotorSpeed = currentMotorSpeed - 5;
  }

  if (currentMotorSpeed < 0) {
    currentMotorSpeed = 0;
  }
  else if (currentMotorSpeed > 255) {
    currentMotorSpeed = 255;
  }

  digitalWrite(MOTOR_L_BRK, LOW);
  digitalWrite(MOTOR_R_BRK, LOW);
  analogWrite(MOTOR_L_PWM, prevMotorSpeed - leftOffset);
  analogWrite(MOTOR_R_PWM, prevMotorSpeed - rightOffset);

  prevMotorSpeed = currentMotorSpeed;
}

void getDistance() {
  int currentCM = 0;
  sonicCurrentMillis = millis();
  if (sonicCurrentMillis - sonicStartMillis >= sonicFreqMillis) {
    sonicStartMillis = sonicCurrentMillis;
    if (fwRSwState == HIGH) {
      currentCM = r_sonar.ping_cm();
    }
    else {
      currentCM = f_sonar.ping_cm();
    }
    if (currentCM != 0) {
      distanceCM = currentCM;
    }
  }
}
