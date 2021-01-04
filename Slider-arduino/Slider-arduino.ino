#include <Stepper.h>
 
#define STEPS 64

int buttonPin = 7;
int potPin = 2;
int buttonState = 0;
int potVal = 0;
Stepper stepper(STEPS, 2, 3, 4, 5);
 
void setup()
{
  Serial.begin(9600);
  Serial.println("start");
  pinMode(buttonPin, INPUT);
  pinMode(potPin, INPUT);
  stepper.setSpeed(100);
}
 
void loop()
{
  potVal = analogRead(potPin);
  Serial.print(potVal);
  int s = map(potVal, 0, 1023, 1, 200);
  Serial.print(" | ");
  Serial.println(s);
  
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Serial.println("start");
//    stepper.setSpeed(s);
    stepper.step(1);
  } else {
    Serial.println("stop");
  }
  delay(500);
}
