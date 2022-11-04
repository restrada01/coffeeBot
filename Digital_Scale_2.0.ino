//Viral Science www.viralsciencecreativity.com www.youtube.com/c/viralscience
//Arduino Digital Weight Scale HX711 Load Cell Module
#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // LiquidCrystal_I2C library
#include <ezButton.h> // pushbutton library
#include <Servo.h>
#define debounce 50

HX711_ADC LoadCell(4, 5); // dt pin, sck pin
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD HEX address 0x27
int tare = 6;
int potPin = A2;
ezButton tareButton(tare);
int a = 0;
float b = 0;
unsigned long currentTime = 0;
unsigned long setTime = 0;
float reference = 0;
int angle = 0;

Servo coffee;

void setup() {
  //pinMode (tare, INPUT_PULLUP);
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(1000); // load cells gets 1000ms of time to stabilize

  pinMode(potPin, INPUT);

  /////////////////////////////////////
  LoadCell.setCalFactor(480); // Calibarate your LOAD CELL with 100g weight, and change the value according to readings
  /////////////////////////////////////
  
  lcd.begin(); // begins connection to the LCD module
  lcd.backlight(); // turns on the backlight
  lcd.setCursor(1, 0); // set cursor to first row
  lcd.print("Digital Scale "); // print out to LCD
  lcd.setCursor(0, 1); // set cursor to first row
  lcd.print(" 5KG MAX LOAD "); // print out to LCD
  delay(3000);
  lcd.clear();

  tareButton.setDebounceTime(debounce);
  tareButton.setCountMode(COUNT_FALLING);

  coffee.attach(9);

}
void loop() { 
  tareButton.loop();
  lcd.setCursor(1, 0); // set cursor to first row
  lcd.print("Digital Scale "); // print out to LCD 
  LoadCell.update(); // retrieves data from the load cell
  float i = LoadCell.getData(); // get output value
 
 //coffee.write(map(int(reference*10)%int(i),0,reference,0,180));
 
 
 if (i<0)
 {
  i = i * (-1);
  lcd.setCursor(0, 1); // setCursor(column, row)
  lcd.print("-");
   lcd.setCursor(8, 1);
  lcd.print("-");
 }
 else
 {
   lcd.setCursor(0, 1);
  lcd.print(" ");
   lcd.setCursor(8, 1);
  lcd.print(" ");
 }
  
  lcd.setCursor(1, 1); // set cursor to secon row
  lcd.print(i, 1); // print out the retrieved value to the second row
  lcd.print("g ");
  float z = i/28.3495;  // convert to ounces
  lcd.setCursor(9, 1);
  lcd.print(z, 2);
  lcd.print("oz ");

  if (i>=5000)
  {
    i=0;
  lcd.setCursor(0, 0); // set cursor to secon row
  lcd.print("  Over Loaded   "); 
  delay(200);
  }

  if (tareButton.isPressed() == true){ //if the button is pressed...
    currentTime = millis(); //measure time
    setTime = millis();
  }
  if (tareButton.isReleased() == true){ //when the button is released
    setTime = millis(); //measure time again for second timer
  }

  if (setTime - currentTime > 3000){    // if the second timer minus the old time is >3000 then...
    lcd.clear(); //clear the display
    while(true){ //loop forever
       tareButton.loop(); //start a button loop
       lcd.setCursor(0, 0); 
       lcd.print("Set tgt weight:");
       lcd.setCursor(5, 1);

       reference = float(map(analogRead(potPin), 0, 1023,0,503))/10;  // for some reason it is off by 3 at max
       angle = map(int(reference)*10,0,503,0,180);
       coffee.write(angle);

       lcd.print(reference,1);
       lcd.print("g ");
       if (tareButton.isPressed() == true) {break;} //if the button is pressed, break out of loop
    } lcd.clear(); //clear display again

  }

  if (tareButton.isPressed() == true)
  {
    lcd.setCursor(0, 1); // set cursor to secon row
    lcd.print("   Taring...    ");
    LoadCell.start(1000);
    lcd.setCursor(0, 1);
    lcd.print("                ");
  } 
}