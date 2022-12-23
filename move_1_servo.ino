
//// ##### Library Imports #####
#include <Servo.h>
#include <Keypad.h>
#include <Wire.h>
//#include <PCF8574.h>
#include <I2CKeyPad.h>
#include <pcf8574.h>
//// ###########################

//// ##### Initial Variables #####
  //// |||||||| Hardware Specific |||||||
  //// I2C Expansion Boards
    PCF8574 brd_1(0x20);
  //// LED variables
    const int led_green =  13; // pin number of green LED

  //// Button Variables
    const int btn_green = 12; // pin number of green pushbutton

  //// Output Variables
    const int buzzer = 8;

  //// Keypad Variables
    const int ROW_NUM = 4; // num of rows rows on keypad
    const int COLUMN_NUM = 4; // num of columns on the keypad
    char keys[ROW_NUM][COLUMN_NUM] = {
      {'1','2','3', 'A'},
      {'4','5','6', 'B'},
      {'7','8','9', 'C'},
      {'*','0','#', 'D'}
    }; // matrix of key locations
    byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
    byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
    Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

  //// Servo Variables
    int loc_neutral = 90;  // neutral postion for continuous servos 
    int loc_forward = 180; // forward postion for continuous servos 
    int loc_reverse = 0; // reverse postion for continuous servos 
    Servo srv_podLift; // declare servo for the pod lift mechanism
    int pin_srv_podLift = 11; // pin that pod lift servo will attach to
    Servo srv_rightLock; // declare servo for the right flap locking mechanism
    Servo srv_drawerLatch; // declare servo for the drawer latch mechanism

  //// |||||||| Puzzle Specific |||||||
  //// Keypad Puzzle
    const String password = "12AB"; // primary password for the keypad puzzle
    String input_password; // actual password that the user input on the pad
    bool correct_pw = false; // whether the correct pw has been entered and confirmed

 
// setup function running at the beginning of the sketch
void setup() {
  Serial.begin(9600); // start serial monitor
  Serial.print("setup running now");
  input_password.reserve(10); // set memory reserve for password

  //pinMode(led_green, OUTPUT); // initialize the LED pin as an output:
  //pinMode(btn_green, INPUT); // initialize the pushbutton pin as an input

  srv_podLift.attach(pin_srv_podLift); // attach the servo to pin
  srv_podLift.write(loc_neutral); // move servo to neutral
  delay(3000);

  // set pin mode output for all the expansion board LEDs
  // turn off all LEDs to start
  pinMode(brd_1, 4, OUTPUT);
  digitalWrite(brd_1, 4, LOW);
  pinMode(brd_1, 5, OUTPUT); //set LED pin as output for testing
  digitalWrite(brd_1, 5, LOW);
  pinMode(brd_1, 6, OUTPUT);
  digitalWrite(brd_1, 6, LOW);
  pinMode(brd_1, 7, OUTPUT);
  digitalWrite(brd_1, 7, LOW);

  pinMode(brd_1, 0, INPUT_PULLUP);
  pinMode(brd_1, 1, INPUT_PULLUP);
  pinMode(brd_1, 2, INPUT_PULLUP);
  pinMode(brd_1, 3, INPUT_PULLUP);


  digitalWrite(brd_1, 4, HIGH);
  delay(250);
  digitalWrite(brd_1, 4, LOW);
  digitalWrite(brd_1, 5, HIGH);
  delay(250);
  digitalWrite(brd_1, 5, LOW);
  digitalWrite(brd_1, 6, HIGH);
  delay(250);
  digitalWrite(brd_1, 6, LOW);
  digitalWrite(brd_1, 7, HIGH);
  delay(250);
  digitalWrite(brd_1, 7, LOW);


}

// main loop function
void loop() {
  int btn_yellow = digitalRead(brd_1, 0);
  int btn_blue = digitalRead(brd_1, 1);
  int btn_red = digitalRead(brd_1, 2);
  int btn_green = digitalRead(brd_1, 3);

  if(!btn_yellow){
    delay(150);
    tone(buzzer, 261, 150);
    digitalWrite(brd_1, 4, HIGH);
    delay(250);
    digitalWrite(brd_1, 4, LOW);
  }

  if(!btn_blue){
    delay(150);
    tone(buzzer, 293, 150);
    digitalWrite(brd_1, 5, HIGH);
    delay(250);
    digitalWrite(brd_1, 5, LOW);
  }

  if(!btn_red){
    delay(150);
    tone(buzzer, 329, 150);
    digitalWrite(brd_1, 6, HIGH);
    delay(250);
    digitalWrite(brd_1, 6, LOW);
  }

  if(!btn_green){
    delay(150);
    tone(buzzer, 392, 150);
    digitalWrite(brd_1, 7, HIGH);
    delay(250);
    digitalWrite(brd_1, 7, LOW);
  }


  
 }

// ################################################

//// ##### PUZZLE FUNCTIONS #####
  // functions for puzzle interactions go below

  void simonSays() {
    // puzzle to match RGBY button-press to pattern shown on LEDs

  }


// #################################################



//// ##### MECHANICAL FUNCTIONS #####
  // functions for box mechanichal movements go below

  // move the continuous servo to raise/lower the keypod cylinder
  void liftKeypod() {
    char key = keypad.getKey();
    if (key){
      Serial.println(key);
      if(key == '*') {
        input_password = ""; // clear input password
      } else if(key == '#') {
      if(password == input_password) {
        Serial.println("password is correct");
        // turn LED on:
        digitalWrite(led_green, HIGH);
        delay(3000);
        digitalWrite(led_green, LOW);
        delay(100);
      } else {
        Serial.println("password is incorrect, try again");
        // turn LED off:
        digitalWrite(led_green, LOW);
        delay(100);
      }

      input_password = ""; // clear input password
      } else {
        input_password += key; // append new character to input password string
      }
    }
  }


// #################################################


//// ##### DEVELOPMENT FUNCTIONS #####
  // any functions below are for development, and can be removed to condense the sketch

  // function to test the movement of continuous servos
  // loop this when 0'ing out the potentiometer on one
  void testContinuousServo(Servo servoName) {
    servoName.write(180);
    Serial.println("spinning servo forward - move to 180");
    delay(1000);
    servoName.write(90);
    Serial.println("stopping servo - move to to 90");
    delay(5000);
    servoName.write(0);
    Serial.println("spinning servo in reverse - move to to 0");
    delay(1000);
    servoName.write(90);
    Serial.println("stopping servo - move to to 90");
    delay(5000);
  }
  
// #################################################