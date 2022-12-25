// Include all libraries for sensors and actuators
#include <HX711_ADC.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>
#include <Servo.h>
#define DEBOUNCE 50 // pushbutton debounce time for repeat presses

HX711_ADC LoadCell(4, 5); // D4 is data pin, D5 is SCLK pin
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD HEX address 0x27
int potPin = A2;  // potentiometer for target weight control
ezButton tareButton(6); // pushbutton for taring scale pin D6

unsigned long currentTime = 0;
unsigned long setTime = 0;
Servo coffeeGate;

float loadCellValue_g;
float setPoint = 0;
float error = 0;
int angle = 0;

// set proportional and integral constants
const float Kp = 1;
const float Ki = 0.1;
float integral = 0;
float output = 0;

void setup() {
  //pinMode (tare, INPUT_PULLUP);
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(1000); // load cell gets 1000ms to stabilize

  pinMode(potPin, INPUT);
  LoadCell.setCalFactor(480); // Calibarate load cell with 100g weight, and change the value according to readings
  
  lcd.begin(); // begins connection to the LCD module
  lcd.backlight(); // turns on the backlight
  lcd.setCursor(1, 0); // set cursor to first row
  lcd.print("Digital Scale "); // print out to LCD
  lcd.setCursor(0, 1); // set cursor to first row
  lcd.print(" 5KG MAX LOAD "); // print out to LCD
  delay(3000);
  lcd.clear();

  tareButton.setDebounceTime(DEBOUNCE);
  tareButton.setCountMode(COUNT_FALLING);

  coffeeGate.attach(9); // attach servo to pin D9 for PWM
}

void printValueToLCD(float loadCellValue_g){
  lcd.setCursor(1, 0); // set cursor to first row
  lcd.print("Digital Scale "); // print out to LCD 

  if (loadCellValue_g < 0){
    loadCellValue_g = loadCellValue_g * (-1);
    lcd.setCursor(0, 1); // setCursor(column, row)
    lcd.print("-");
    lcd.setCursor(8, 1);
    lcd.print("-");}

  else{
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(8, 1);
    lcd.print(" ");}
  
  lcd.setCursor(1, 1); // set cursor to secon row
  lcd.print(loadCellValue_g, 1); // print out the retrieved value to the second row
  lcd.print("g ");
  float loadCellValue_oz = loadCellValue_g/28.3495;  // convert to ounces
  lcd.setCursor(9, 1);
  lcd.print(loadCellValue_oz, 2);
  lcd.print("oz ");

  if (loadCellValue_g>=5000)
  {
    loadCellValue_g = 0;
    lcd.setCursor(0, 0); // set cursor to second row
    lcd.print("  Over Loaded   "); 
    delay(200);
  }
}

void retrieveSetpoint(){
  if (tareButton.isPressed() == true){ //if the button is pressed...
    currentTime = millis(); //measure time
    setTime = millis();
  }
  if (tareButton.isReleased() == true){ //when the button is released
    setTime = millis(); //measure time again for second timer
  }

  if (setTime - currentTime > 3000){    // if button was pressed for 3s
    lcd.clear(); //clear the display
    while(true){ //loop forever
       tareButton.loop(); //start a button loop
       lcd.setCursor(0, 0); 
       lcd.print("Set tgt weight:");
       lcd.setCursor(5, 1);

       setPoint = float(map(analogRead(potPin), 0, 1023,0,503))/10;  // map potentiometer to set target weight

       lcd.print(setPoint,1);
       lcd.print("g ");
       if (tareButton.isPressed() == true) {break;} //if the button is pressed, break out of loop
    } lcd.clear(); //clear display again
  }
}

void loop() { 

  tareButton.loop();  // must be called for button to debounce and read value 

  LoadCell.update(); // retrieves data from the load cell
  loadCellValue_g = LoadCell.getData(); // get output value
  
  printValueToLCD(loadCellValue_g); // print value to LCD display

  retrieveSetpoint();

  // calculate the error between the read value and setpoint
  error = setPoint - loadCellValue_g;

  // calculate the control signal
  integral += error;
  output = Kp * error + Ki * integral;
  
  // map the output to a servo angle
  angle = map(int(output)*10, 0, 503, 0, 180);
  coffeeGate.write(angle);

  // reset LoadCell to zero if tare button is pressed
  if (tareButton.isPressed() == true)
  {
    lcd.setCursor(0, 1); // set cursor to secon row
    lcd.print("   Taring...    ");
    LoadCell.start(1000);
    lcd.setCursor(0, 1);
    lcd.print("                ");
  } 
}
