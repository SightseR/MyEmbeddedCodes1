#include <Keypad.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>

LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

const byte ROWS = 4;
const byte COLS = 4;

//wind direction variables

const int buttonInputPin = 3;
volatile boolean buttonPressedA;
volatile boolean buttonPressedB;
String windDirection;
int buttonState = 0;
int previousButtonState = 0;
bool displayDirection = true;
int degree_Value;


//wind speed variables

// Define variables
volatile unsigned long pulseCount;
volatile unsigned long windFrequency;
//volatile boolean buttonPressed;
float windSpeed;
unsigned long i_time;

// Define pin numbers
const int signalInputPin = 2;
//const int buttonInputPin = 3;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = { 42, 43, 44, 45 };
byte colPins[COLS] = { 46, 47, 48, 49 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);

  //wind direction void setup

  pinMode(A2, INPUT);
  pinMode(buttonInputPin, INPUT_PULLUP);

  //wind speed void setup

   pulseCount = 0;
  windFrequency = 0;
  buttonPressedA = false;
  buttonPressedB = false;

  windSpeed = 0.0;
  i_time = 0;

  // Attach interrupt function to signal input pin
  attachInterrupt(digitalPinToInterrupt(signalInputPin), pulseCountFunction, RISING);

  // Set up timer for 0.5 second intervals
  Timer1.initialize(500000);
  Timer1.attachInterrupt(timerInterruptFunction);

  // Set button pin mode
  pinMode(buttonInputPin, INPUT_PULLUP);

}

void loop(){

 lcd.setCursor(20, 1);
  lcd.print("A-WindDir B-WindSpd");

  //delay(1000);
  float analog_Value = analogRead(A2);
  float voltage_Value = 5 * analog_Value / 1023;

   if (voltage_Value < 1.2) {
      degree_Value = 0;
    } else{
      degree_Value = ((voltage_Value - 1.2) / 3.8) * 360;
    }

  if (displayDirection) {
    lcd.setCursor(0, 0);

    if (voltage_Value <= 1.19) {
      windDirection = "Wait....";
    } else if (degree_Value >= 337.5 || (degree_Value > -1 && degree_Value <= 22.4)) {
      windDirection = "North";
    } else if (degree_Value > 22.4 && degree_Value <= 67.5) {
      windDirection = "North East";
    } else if (degree_Value > 67.5 && degree_Value <= 112.5) {
      windDirection = "East";
    } else if (degree_Value > 112.5 && degree_Value <= 157.5) {
      windDirection = "South East";
    } else if (degree_Value > 157.5 && degree_Value <= 202.5) {
      windDirection = "South";
    } else if (degree_Value > 202.5 && degree_Value <= 247.5) {
      windDirection = "South West";
    } else if (degree_Value > 247.5 && degree_Value <= 292.5) {
      windDirection = "West";
    } else {
      windDirection = "North West";
    }
  }
 
  delay(500);

  char key = keypad.getKey();

  if(key == 'A'){
    buttonPressedA = true;
    buttonPressedB = false;
  } else if(key == 'B'){
    buttonPressedB = true;
    buttonPressedA = false;
  }
 
  
  if(buttonPressedA == true && buttonPressedB == false){
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("Dir : ");
      lcd.print(windDirection);
      lcd.setCursor(0, 1);
      lcd.print(degree_Value);
      lcd.print(" degrees");
    } else if(buttonPressedB == true && buttonPressedA == false){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wind Speed: ");
      lcd.setCursor(13, 0);
      lcd.print(windSpeed);
      lcd.setCursor(0, 1);
      lcd.print("Freq: ");
      lcd.setCursor(7, 1);
      lcd.print(windFrequency);
      lcd.setCursor(13, 1);
      lcd.print("Hz");

  }

  
  

}

// Interrupt function to count pulses
void pulseCountFunction() {
  pulseCount++;
}

// Interrupt function to calculate wind frequency every 0.5 seconds
void timerInterruptFunction() {
  i_time++;
  if (i_time > 19) { // Perform wind speed calculation every 5 seconds
    windFrequency = pulseCount / 10; // Multiply pulse count by 2 to get frequency in Hz
    pulseCount = 0; // Reset pulse count
    windSpeed = -0.24 + windFrequency * 0.699; // Calculate wind speed using equation
    i_time = 0; // Reset time counter
  }
}