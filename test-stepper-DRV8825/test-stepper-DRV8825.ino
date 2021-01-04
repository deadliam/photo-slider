// EasyDriver connections
#define step_pin 10  // Pin 10 connected to Steps pin on EasyDriver
#define dir_pin 11   // Pin 11 connected to Direction pin
#define MS1 8       // Pin 8 connected to MS1 pin
#define MS2 9      // Pin 9 connected to MS2 pin
#define SLEEP 12     // Pin 12 connected to SLEEP pin

volatile boolean TurnDetected;  // need volatile for Interrupts
volatile boolean rotationdirection;  // CW or CCW rotation

// Rotary Encoder Module connections
const int PinCLK = 2;   // Generating interrupts using CLK signal
const int PinDT = 3;    // Reading DT signal
const int PinSW = 4;    // Reading Push Button switch

int StepperPosition = 0;    // To store Stepper Motor Position
int StepsToTake = 20;      // Controls the speed of the Stepper per Rotary click

int8_t counter = 0;
int8_t tempCount;

int direction;   // Variable to set Rotation (CW-CCW) of stepper

// Interrupt routine runs if CLK goes from HIGH to LOW
void rotarydetect() {
  delay(15);  // delay for Debouncing
  if (digitalRead(PinCLK))
  rotationdirection = digitalRead(PinDT);
  else
  rotationdirection = !digitalRead(PinDT);
  TurnDetected = true;
}


void setup ()  {
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  pinMode(step_pin, OUTPUT);
  pinMode(SLEEP, OUTPUT);   
  digitalWrite(SLEEP, HIGH);  // Wake up EasyDriver
  delay(5);  // Wait for EasyDriver wake up
   
 /* Configure type of Steps on EasyDriver:
 // MS1 MS2
 //
 // LOW LOW = Full Step //
 // HIGH LOW = Half Step //
 // LOW HIGH = A quarter of Step //
 // HIGH HIGH = An eighth of Step //
 */ 
  digitalWrite(MS1, LOW);      // Configures to Full Steps
  digitalWrite(MS2, LOW);    // Configures to Full Steps

  pinMode(PinCLK,INPUT);  // Set Pin to Input
  pinMode(PinDT,INPUT);  
  pinMode(PinSW,INPUT);
  digitalWrite(PinSW, HIGH); // Pull-Up resistor for switch
  attachInterrupt(0, rotarydetect, FALLING); // interrupt 0 always connected to pin 2 on Arduino UNO
}


void loop ()  {

  if (!(digitalRead(PinSW))) {   // check if button is pressed
    if (StepperPosition == 0) {  // check if button was already pressed
    } else {
        if (StepperPosition > 0) {  // Stepper was moved CW
          while (StepperPosition != 0){  //  Do until Motor position is back to ZERO
            digitalWrite(dir_pin, HIGH);  // (HIGH = anti-clockwise / LOW = clockwise)
            for (int x = 1; x < StepsToTake; x++) {
              digitalWrite(step_pin, HIGH);
              delay(1);
              digitalWrite(step_pin, LOW);
              delay(1);            
            }
            StepperPosition = StepperPosition - StepsToTake;
          }
        } else {
          while (StepperPosition != 0) {
            digitalWrite(dir_pin, LOW);  // (HIGH = anti-clockwise / LOW = clockwise)
              for (int x = 1; x < StepsToTake; x++) {
              digitalWrite(step_pin, HIGH);
              delay(1);
              digitalWrite(step_pin, LOW);
              delay(1);            
            }
           StepperPosition = StepperPosition + StepsToTake;
        }
      }
      StepperPosition = 0; // Reset position to ZERO after moving motor back
      counter = 0;
    }
  }

// Runs if rotation was detected
  if (TurnDetected) {
    TurnDetected = false;  // do NOT repeat IF loop until new rotation detected

// Which direction to move Stepper motor
    if (rotationdirection) { // Move motor CCW
        digitalWrite(dir_pin, HIGH);  // (HIGH = anti-clockwise / LOW = clockwise)
        for (int x = 1; x < StepsToTake; x++) {
          digitalWrite(step_pin, HIGH);
          delay(1);
          digitalWrite(step_pin, LOW);
          delay(1);            
        }
        counter--;
        StepperPosition = StepperPosition - StepsToTake;
    }

    if (!rotationdirection) { // Move motor CW
        digitalWrite(dir_pin, LOW);  // (HIGH = anti-clockwise / LOW = clockwise)
        for (int x = 1; x < StepsToTake; x++) {
          digitalWrite(step_pin, HIGH);
          delay(1);
          digitalWrite(step_pin, LOW); 
          delay(1);         
        }
        counter++;
        StepperPosition = StepperPosition + StepsToTake;
    }
  }
}
