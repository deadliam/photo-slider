
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <AccelStepper.h>

const char ssid[] = "Xiaomi_236D"; // WiFi name
const char password[] = "ellisfromlos"; // WiFi password

WiFiClient client;
char auth[] = "0pt04wgCJiLZm4xKFaP3Gg2nCdctG5zD"; // Blynk Authentication Token

#define BUTTON1 D1
#define BUTTON2 D2

#define HALFSTEP 4

#define motorPin1  D5     // IN1 on the ULN2003 driver 1
#define motorPin2  D6    // IN2 on the ULN2003 driver 1
#define motorPin3  D7    // IN3 on the ULN2003 driver 1
#define motorPin4  D8     // IN4 on the ULN2003 driver 1

boolean stepperMove = false;
int stepperDirection = 0;
int accel = 100;
int stepperSpeed = 200;
int maxSpeedStepper = 1000;

boolean button1Pressed = false;
boolean button2Pressed = false;

AccelStepper stepper(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

BLYNK_WRITE(V0) // Button Widget is writing to pin V0
{
  stepperDirection = param.asInt();
}

BLYNK_WRITE(V1) // Slider Widget is writing to pin V1
{
  stepperSpeed = param.asInt();
}

BLYNK_WRITE(V2) // Button Widget is writing to pin V2
{
  int stepperRun = param.asInt();
  
  if (stepperRun == true)
  {
    stepperMove = true;
  } else {
    stepperMove = false;
  }
}

void setup()
{
  stepper.setMaxSpeed(maxSpeedStepper);
  stepper.setSpeed(stepperSpeed);
  stepper.setAcceleration(accel);
  
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);

  Serial.begin(115200);
  Serial.print("Initialize Blynk.");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(auth, ssid, password);

  Blynk.virtualWrite(0, 0);
//  Blynk.virtualWrite(1, 1);
  Blynk.virtualWrite(2, 0);
}

void loop()
{
  Blynk.run();
  if (stepperMove == true)
  {
    if (stepperDirection == true) 
    {
      stepper.setSpeed(stepperSpeed);
      stepper.runSpeed();
    } else {
      stepper.setSpeed(-stepperSpeed);
      stepper.runSpeed();
    }  
  } else {
    stepperMove = false;
  }

  if (digitalRead(BUTTON1) == HIGH && stepperDirection == true) {
    stepperMove = false;
    stepperDirection = false;
    Blynk.virtualWrite(2, 0);
    Blynk.virtualWrite(0, 0);
  }
  if (digitalRead(BUTTON2) == HIGH && stepperDirection == false) {
    stepperMove = false;
    stepperDirection = true;
    Blynk.virtualWrite(2, 0);
    Blynk.virtualWrite(0, 1);
  }
}
