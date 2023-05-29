  #include <LiquidCrystal.h>

  LiquidCrystal lcd_1(37, 36, 35, 34, 33, 32);

  void setup()
  {
    lcd_1.begin(16, 2); // Set up the number of columns and rows on the LCD.

    /*pinMode(A2, INPUT);
    
    //lcd_1.print("Week-6 Task-B");

    pinMode(13,OUTPUT);
    pinMode(12,OUTPUT);
    pinMode(11,OUTPUT);
    pinMode(10,OUTPUT);
    pinMode(9,OUTPUT);
    pinMode(8,OUTPUT);
    pinMode(7,OUTPUT);
    pinMode(6,OUTPUT);*/
  }

  void loop()
  {
  int ch = 65; 
  int c1 = 0;
  int c2 = 12;
  int c3 = 0;
  int c4 = 1;

    for(int i = 65; i<=77; i++){
    lcd_1.clear();
    lcd_1.setCursor(c1, c3);
    lcd_1.print((char) ch);
    if (ch >= 90){
      ch = 65;
    } else {
    ch++;
    }

    if (c1 >= 12){
      c1 = 0;
    } else {
    c1++;
    }

    delay(200);
    }

  for(int j = 90; j >= 78; j--){
    lcd_1.clear();
    lcd_1.setCursor(c2, c4);
    lcd_1.print((char) ch);
    if (ch >= 90){
      ch = 65;
    } else {
    ch++;
    }
    
    if (c2 <= 0){
      c2 = 12;
    } else {
      c2--;
    }
    
    delay(200);
    }
  
    

  }
