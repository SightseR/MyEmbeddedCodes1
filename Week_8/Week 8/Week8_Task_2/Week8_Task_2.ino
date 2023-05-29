#include <LiquidCrystal.h>

LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

const int buttonInputPin = 3;
volatile boolean buttonPressed;
String windDirection;
int buttonState = 0;
int previousButtonState = 0;
bool displayDirection = true;
int degree_Value;

void setup() {
  lcd.begin(16, 2);

  pinMode(A2, INPUT);
  pinMode(buttonInputPin, INPUT_PULLUP);
}

void loop() {
  buttonState = digitalRead(buttonInputPin);

  if (buttonState != previousButtonState) {
    if (buttonState == LOW) {
      displayDirection = !displayDirection;
      lcd.clear();
    }
  }
  previousButtonState = buttonState;

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

    lcd.clear();
    lcd.print(windDirection);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(degree_Value);
    lcd.print(" degrees");
  }

  lcd.setCursor(20, 1);
  lcd.print(voltage_Value);
   lcd.print("V");
  lcd.print(" : ");
  lcd.print(degree_Value);
   lcd.print("Deg.");

  delay(250);
}
