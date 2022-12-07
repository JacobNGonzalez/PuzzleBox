//// ##### Library Imports #####
#include <Servo.h>
#include <Keypad.h>
//// ###########################

//// ##### Initial Constants/Variables #####
  //// |||||||| Hardware Specific |||||||
  //// LED variables
    const int ledPin =  13;      // the number of the LED pin

  //// Button Variables
    const int buttonPin = 12;     // the number of the pushbutton pin

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
    Servo srv_rightLock; // declare servo for the right flap locking mechanism
    Servo srv_drawerLatch; // declare servo for the drawer latch mechanism

  //// |||||||| Puzzle Specific |||||||
  //// Keypad Puzzle
    const String password = "12AB"; // primary password for the keypad puzzle
    String input_password; // actual password that the user input on the pad
    bool correct_pw = false; // whether the correct pw has been entered and confirmed



int buttonState = 0; //pushbutton status   
// setup function running at the beginning of the sketch
void setup() {
  Serial.begin(9600);

  input_password.reserve(10);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  Serial.print("sketch running now");
  myservo.attach(11);
  myservo.write(90);

  delay(5000);
}

// main loop function
void loop() {

  myservo.write(180);
  Serial.println("spinning servo forward - move to 180");
  delay(1000);
  myservo.write(90);
  Serial.println("stopping servo - move to to 90");
  delay(5000);
  myservo.write(0);
  Serial.println("spinning servo in reverse - move to to 0");
  delay(1000);
  myservo.write(90);
  Serial.println("stopping servo - move to to 90");
  delay(5000);
  //  char key = keypad.getKey();
  // if (key){
  //   Serial.println(key);

  //     if(key == '*') {
  //       input_password = ""; // clear input password
  //     } else if(key == '#') {
  //     if(password == input_password) {
  //       Serial.println("password is correct");
  //       // turn LED on:
  //       digitalWrite(ledPin, HIGH);
  //       for (pos = 0; pos <= 90; pos += 1) {
  //         myservo.write(pos);
  //         delay(15);
  //       }
  //       delay(30);
  //       for (pos = 90; pos >= 0; pos -= 1) {
  //         myservo.write(pos);
  //         delay(15);
  //       }
  //       digitalWrite(ledPin, LOW);
  //       delay(100);
  //     } else {
  //       Serial.println("password is incorrect, try again");
  //       // turn LED off:
  //       digitalWrite(ledPin, LOW);
  //       delay(100);
  //     }

  //     input_password = ""; // clear input password
  //     } else {
  //        input_password += key; // append new character to input password string
  //     }
  // }


//_____________________________________________________________________________


      // // read the state of the pushbutton value:
      // buttonState = digitalRead(buttonPin);
      // // Show the state of pushbutton on serial monitor
      // Serial.println(buttonState);
      // // check if the pushbutton is pressed.
      // // if it is, the buttonState is HIGH:
      // // if (buttonState == HIGH) {
      //   if (correct_pw == true){
      //   // turn LED on:
      //   digitalWrite(ledPin, HIGH);
      //   for (pos = 0; pos <= 90; pos += 1) {
      //     myservo.write(pos);
      //     delay(15);
      //   }
      //   delay(30);
      //   for (pos = 90; pos >= 0; pos -= 1) {
      //     myservo.write(pos);
      //     delay(15);
      //   }
      //   digitalWrite(ledPin, LOW);
      //   correct_pw = false;
      // } else {
      //   // turn LED off:
      //   digitalWrite(ledPin, LOW);
      // }
      // // Added the delay so that we can see the output of button
      // delay(100);
} 

// ################################################
// all puzzle interaction functions below

void simonSays() {
  // puzzle to match RGBY button-press to pattern shown on LEDs

}

// #################################################

// #################################################
// all box mechanical functions below

void liftKeypod() {
  // move the continuous servo to raise/lower the keypod cylinder

}

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