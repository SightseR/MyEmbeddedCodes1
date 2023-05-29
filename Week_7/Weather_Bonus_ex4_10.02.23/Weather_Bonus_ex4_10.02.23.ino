#include <LiquidCrystal.h>
LiquidCrystal lcd(37, 36, 35, 34, 33, 32);
int pulseCount = 0;
int buttonPin= 2; 
unsigned long previousTime = 0;
float frequency = 0;

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin),checkSwitch,FALLING); //calling unterrupt function here
  lcd.setCursor(0,0);
  lcd.print("Frequ: ");
  lcd.setCursor(0,1);
  lcd.print("Count: ");
  lcd.setCursor(21,1);
  lcd.print("ES Course Group 2");
}

void loop(){
}

void checkSwitch() {
  unsigned long currentTime = millis();
  if (digitalRead(buttonPin) == LOW) {
      frequency = 1000.0/(currentTime - previousTime);//time difference between two falling edges
      previousTime = currentTime;
      lcd.setCursor(7,0);
      lcd.print(frequency);
      Serial.println(frequency);
    
      pulseCount++;//keep count of button press 
      lcd.setCursor(7,1);
      lcd.print(pulseCount);
  }
}