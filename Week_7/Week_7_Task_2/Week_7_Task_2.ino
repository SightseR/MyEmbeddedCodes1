  #include <LiquidCrystal.h>

  LiquidCrystal lcd_1(37, 36, 35, 34, 33, 32);

  int buttonState = 0;

  void setup()
  {
    pinMode(2, INPUT);
    //pinMode(13, OUTPUT);
  }

  void loop()
  {

    lcd_1.clear();
    lcd_1.setCursor(0, 0);

    // read the state of the pushbutton
    buttonState = digitalRead(2);
    // check if pushbutton is pressed. if it is, the
    // button state is HIGH
    if (buttonState == HIGH) {
      lcd_1.print("Button Pressed!");
    } else {
      lcd_1.print("Button Not Pressed!");
    }
    delay(10); // Delay a little bit to improve simulation performance

    
  }
