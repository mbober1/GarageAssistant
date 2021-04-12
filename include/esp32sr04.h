#define PWM_CHANNEL_TRIG 0
#define ECHO1_PIN 12
#define ECHO2_PIN 14
#define TRIG_PIN 33

struct EspSr04 {

  unsigned long sensor1Time;
  unsigned long sensor2Time;
  float distance1_cm = 200;
  float distance2_cm = 200;
  
};

EspSr04 espSr04;

void IRAM_ATTR Echo1_Callback() {

    if(digitalRead(ECHO1_PIN)) {
      espSr04.distance1_cm = (micros() - espSr04.sensor1Time)/58.0;
      if(espSr04.distance1_cm < 200)
        Serial.println((int)espSr04.distance1_cm);
    }
    else espSr04.sensor1Time = micros();
}

void IRAM_ATTR Echo2_Callback() {

    if(digitalRead(ECHO2_PIN)) {
      espSr04.distance2_cm = (micros() - espSr04.sensor2Time)/58.0;
      if(espSr04.distance2_cm < 200)
        Serial.println((int)espSr04.distance2_cm);
    }
    else espSr04.sensor2Time = micros();
}

void initSensors() {

    pinMode(ECHO1_PIN, INPUT_PULLUP);
    pinMode(ECHO2_PIN, INPUT_PULLUP);

    attachInterrupt(ECHO1_PIN, Echo1_Callback, CHANGE);
    attachInterrupt(ECHO2_PIN, Echo2_Callback, CHANGE);

    ledcAttachPin(TRIG_PIN, PWM_CHANNEL_TRIG);
    ledcSetup(PWM_CHANNEL_TRIG, 5, 15);
    ledcWrite(PWM_CHANNEL_TRIG, 2);
}
