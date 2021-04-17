#define PWM_CHANNEL_TRIG 0
#define ECHO1_PIN 12
#define ECHO2_PIN 14
#define TRIG_PIN 33

extern void IRAM_ATTR echoCallback();


class Esp32sr04
{
public:
  unsigned long sensorTime;
  int distance = -1;
  int echoPin, trigPin;

  Esp32sr04(const int &echoPin, const int &trigPin);
  ~Esp32sr04();
};


Esp32sr04::Esp32sr04(const int &echoPin, const int &trigPin) : echoPin(echoPin), trigPin(trigPin)
{
  pinMode(this->echoPin, INPUT_PULLUP);
  attachInterrupt(this->echoPin, echoCallback, CHANGE);
  ledcAttachPin(this->trigPin, PWM_CHANNEL_TRIG);
  ledcSetup(PWM_CHANNEL_TRIG, 5, 15);
  ledcWrite(PWM_CHANNEL_TRIG, 2);
}

Esp32sr04::~Esp32sr04() {}