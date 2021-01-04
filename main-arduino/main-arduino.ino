#include <Adafruit_SH1106.h>

// EasyDriver connections
#define step_pin 10  // Pin 10 connected to Steps pin on EasyDriver
#define dir_pin 11   // Pin 11 connected to Direction pin
#define MS1 8       // Pin 8 connected to MS1 pin
#define MS2 9      // Pin 9 connected to MS2 pin
#define SLEEP 12     // Pin 12 connected to SLEEP pin
#define maxItemSize 5

// Rotary Encoder Module connections
#define pin_CLK 2
#define pin_DT  3
#define pin_Btn 4

unsigned long CurrentTime, LastTime;
enum eEncoderState {eNone, eLeft, eRight, eButton};
uint8_t EncoderA, EncoderB, EncoderAPrev;
bool ButtonPrev;

#define OLED_RESET -1
Adafruit_SH1106 display(OLED_RESET);

unsigned long DELAY_TIME = 1000; // 1 sec
unsigned long delayStart = 0; // the time the delay started
bool delayRunning = false; // true if still waiting for delay to finish

int StepperPosition = 0;    // To store Stepper Motor Position
int StepsToTake = 20;      // Controls the speed of the Stepper per Rotary click

int positionX1;
int positionX2;
int positionY1;
int positionY2;

int currentMenuItem = 0;
char menu[maxItemSize][10] = {"SET X1", "SET X2", "SET Y1", "SET Y2", "SET SPEED"}; //, "SET 5", "SET 6", "SET 7", "SET 8", "SET 9"};

//// SWITCH ////
int buttonStatePrevious = LOW;                      // previousstate of the switch

unsigned long minButtonLongPressDuration = 1500;    // Time we wait before we see the press as a long press
unsigned long buttonLongPressMillis;                // Time in ms when we the button was pressed
bool buttonStateLongPress = false;                  // True if it is a long press
const int intervalButton = 50;                      // Time between two readings of the button state
unsigned long previousButtonMillis;                 // Timestamp of the latest reading
unsigned long buttonPressDuration;                  // Time the button is pressed in ms
unsigned long currentMillis;          // Variabele to store the number of milleseconds since the Arduino has started
boolean ButtonStateShort = false;
boolean ButtonStateLong = false;
//------------------------------

eEncoderState GetEncoderState() {
  // Read encoder state
  eEncoderState Result = eNone;
  CurrentTime = millis();
  if (CurrentTime - LastTime >= 10) {
    // read every 5msec
    LastTime = CurrentTime;
    if (digitalRead(pin_Btn) == LOW ) {
      if (ButtonPrev) {
        Result = eButton; // Нажата кнопка
        ButtonPrev = 0;
      }
    }
    else {
      ButtonPrev = 1;
      EncoderA = digitalRead(pin_DT);
      EncoderB = digitalRead(pin_CLK);
      if ((!EncoderA) && (EncoderAPrev)) { // Signal A changed from 1 to 0
        if (EncoderB) Result = eRight;     // B=1 => CW
        else          Result = eLeft;      // B=0 => CCW
      }
      EncoderAPrev = EncoderA; // save current state
    }
  }
  return Result;
}

void waitForButtonState() {
  
  // If the difference in time between the previous reading is larger than intervalButton
  if(currentMillis - previousButtonMillis > intervalButton) {
    
    // Read the digital value of the button (LOW/HIGH)
    int buttonState = digitalRead(pin_Btn);
    buttonState = !buttonState;
    
    // If the button has been pushed AND
    // If the button wasn't pressed before AND
    // IF there was not already a measurement running to determine how long the button has been pressed
    if (buttonState == HIGH && buttonStatePrevious == LOW && !buttonStateLongPress) {
      buttonLongPressMillis = currentMillis;
      buttonStatePrevious = HIGH;
      Serial.println("Button pressed");
    }

    // Calculate how long the button has been pressed
    buttonPressDuration = currentMillis - buttonLongPressMillis;

    // If the button is pressed AND
    // If there is no measurement running to determine how long the button is pressed AND
    // If the time the button has been pressed is larger or equal to the time needed for a long press
    if (buttonState == HIGH && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {
      buttonStateLongPress = true;
      Serial.println("Button long pressed");
      ButtonStateLong = !ButtonStateLong;
      returnHome();
    }
      
    // If the button is released AND
    // If the button was pressed before
    if (buttonState == LOW && buttonStatePrevious == HIGH) {
      buttonStatePrevious = LOW;
      buttonStateLongPress = false;
      Serial.println("Button released");

      // If there is no measurement running to determine how long the button was pressed AND
      // If the time the button has been pressed is smaller than the minimal time needed for a long press
      // Note: The video shows:
      //       if (!buttonStateLongPress && buttonPressDuration < minButtonLongPressDuration) {
      //       since buttonStateLongPress is set to FALSE on line 75, !buttonStateLongPress is always TRUE
      //       and can be removed.
      if (buttonPressDuration < minButtonLongPressDuration) {
        Serial.println("Button pressed shortly");        
        ButtonStateShort = !ButtonStateShort;
      }
    }
    // store the current timestamp in previousButtonMillis
    previousButtonMillis = currentMillis;
  }
}

void stepperMove(int directionStepper) {
  if(directionStepper == 1) {
    digitalWrite(dir_pin, HIGH);  // (HIGH = anti-clockwise / LOW = clockwise)  
  } else if(directionStepper == 2) {
    digitalWrite(dir_pin, LOW);  // (HIGH = anti-clockwise / LOW = clockwise)  
  }
  
  for (int x = 1; x < StepsToTake; x++) {
    digitalWrite(step_pin, HIGH);
    delay(1);
    digitalWrite(step_pin, LOW);
    delay(1);            
  }
  if(directionStepper == 1) {
    StepperPosition = StepperPosition - StepsToTake;
  } else if(directionStepper == 2) {
    StepperPosition = StepperPosition + StepsToTake;  
  }
//  Serial.print("Stepper position: ");
//  Serial.println(StepperPosition);
}

void returnHome() {
//    if (!(digitalRead(PinSW))) {   // check if button is pressed
    if(StepperPosition == 0) {  // check if button was already pressed
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
    }
//  }
}

void displayUpdate(char* str) {
  if (delayRunning && ((millis() - delayStart) >= DELAY_TIME)) {
    delayRunning = false; // finished delay -- single shot, once only
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(str);
    display.display();
  }
}

// Func for Display
void resetTimer() {
  delayStart = millis();
  delayRunning = true;
}

int readRotary() {
  switch (GetEncoderState()) {
    case eNone: return 0;
    case eLeft: {   // CW
      Serial.println("======= LEFT");
      return 1;
      break;
    }
    case eRight: {  // CCW
      Serial.println("======= RIGHT");
      return 2;
      break;
    }
    case eButton: { // Button
      currentMenuItem += 1;
      if(currentMenuItem > maxItemSize - 1) {
        currentMenuItem = 0;
    }
      resetTimer();
      Serial.println(menu[currentMenuItem]);
      return 3;
    }
  }
}

void setup()  {
  Serial.begin(9600);
  
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

  pinMode(pin_DT,  INPUT);
  pinMode(pin_CLK, INPUT);
  pinMode(pin_Btn, INPUT_PULLUP); // Button hasn't pull to +5, then we use pull-up mode

  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  delay(1000);
  resetTimer();
}

void loop () {
  currentMillis = millis();

  readRotary();
  waitForButtonState();
  switch (currentMenuItem) {
    case 0:
      Serial.println("Case 0");
      displayUpdate("==== SLIDER ====");
      break;
    
    case 1:
      Serial.println("Case 1");
      displayUpdate("SET X1");
      delay(1000);
      while(currentMenuItem == 1) {
        // 1 - LEFT, 2 - RIGHT
        int rotationDirection = readRotary();
        stepperMove(rotationDirection);
      }
      break;
    
    case 2:
      Serial.println("Case 2");
      displayUpdate("SET X2");
      break;

    case 3:
      Serial.println("Case 3");
      displayUpdate("SET Y1");
      break;

    case 4:
      Serial.println("Case 4");
      displayUpdate("SET Y2");
      break;
    default:
      Serial.println("Case DEFAULT");
  }
}
