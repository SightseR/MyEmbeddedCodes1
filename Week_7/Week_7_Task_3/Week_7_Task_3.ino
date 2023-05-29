  #include <LiquidCrystal.h>

  LiquidCrystal lcd_1(37, 36, 35, 34, 33, 32);


  int buttonState = 0;
  int pulseCount = 0;
  int timer = 0;

  void setup()
  {
    pinMode(2, INPUT);
    //pinMode(13, OUTPUT);
  }

  void loop()
  {

    

    // read the state of the pushbutton
    buttonState = digitalRead(2);
    // check if pushbutton is pressed. if it is, the
    // button state is HIGH
    if (buttonState == HIGH) {
     attachInterrupt(digitalPinToInterrupt(2), pulse_interrupt, RISING);
    }
  
    delay(10); // Delay a little bit to improve simulation performance

      lcd_1.clear();
      lcd_1.setCursor(0, 0);
      lcd_1.print("Pulse count: ");
      lcd_1.setCursor(13, 1);
      lcd_1.print(pulseCount);
  }


  void pulse_interrupt(void){
    pulseCount++;
  }