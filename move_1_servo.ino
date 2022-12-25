
//// ##### Library Imports #####
#include <Servo.h>
#include <Keypad.h>
#include <Wire.h>
#include <Key.h>
//#include <PCF8574.h>
//#include <I2CKeyPad.h>
#include <pcf8574.h>
#include <Keypad_I2C.h>
//// ###########################

//// ##### Initial Variables #####
  //// |||||||| Hardware Specific |||||||
  //// I2C Expansion Boards
    PCF8574 brd_1(0x20);  // board with simon hardware
    //PCF8574 brd_2(0x21);  // board with keypad hardware

  //// LED variables
    const int led_green =  13; // pin number of green LED
    int ledArray[] = {4, 5, 6, 7}; // {yellow, blue, red, green}.  ledArray[0] is yellow

  //// Button Variables
    const int btn_green = 12; // pin number of green pushbutton
    int buttonArray[] = {0, 1, 2, 3}; // {yellow, blue, red, green}. buttonArray[0] is yellow
    
  //// Output Variables
    const int buzzer = 8;
    int toneArray[] = {261, 293, 329, 392}; // {C, D, E, G}. all octave 4

  //// Keypad Variables
    /// Below is for Keypad on KeyPad_I2C (Joe Young's)
    int brd_keypad = 0x21;
    const byte ROWS = 4; // num of rows rows on keypad
    const byte COLS = 4; // num of columns on the keypad
    char keys[ROWS][COLS] = {
      {'1','2','3', 'A'},
      {'4','5','6', 'B'},
      {'7','8','9', 'C'},
      {'*','0','#', 'D'}
    }; // matrix of key locations
    byte rowPins[ROWS] = {0, 1, 2, 3}; //connect to the row pinouts of the keypad
    byte colPins[COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad
    // Call the keypad constructor. Normally the last value would be the expansion
    //    board type (PCF8574 in this case). Replacing it with 1 per the library code
    //    because using the name interferes with other library used.
    Keypad_I2C keypad (makeKeymap (keys), rowPins, colPins, ROWS, COLS, brd_keypad, 1);


  //// Servo Variables
    int loc_neutral = 90;  // neutral postion for continuous servos 
    int loc_forward = 180; // forward postion for continuous servos 
    int loc_reverse = 0; // reverse postion for continuous servos 
    Servo srv_podLift; // declare servo for the pod lift mechanism
    int pin_srv_podLift = 11; // pin that pod lift servo will attach to
    Servo srv_rightLock; // declare servo for the right flap locking mechanism
    Servo srv_drawerLatch; // declare servo for the drawer latch mechanism
    int pin_srv_drawerLatch = 12;
  //// |||||||| Puzzle Specific |||||||
  //// Keypad Puzzle
    const String password = "3920B"; // primary password for the keypad puzzle
    String pad_input; // password the user has input on the pad

  //// Simon Puzzle
   #define simonRounds 7 // number of rounds in the Simon game
    int pinCount = 4; // number of pins used (same for btns and leds)
    int ledState = 0;
    int gameState_simon = 0;    
    int gameValues_simon[simonRounds];
    int roundNum = 0; // current round in simon

    unsigned long previousMillis = 0;
    unsigned long interval1 = 500; // time inbetween LED blinks

    //declare functions
    void setLed(int ledNum);
    void displayClue(int roundNum);
    boolean waitForInput(int buttonNum);

  //// |||||||| Puzzle Completion Bools |||||||

    bool won_simon = false;
    bool won_password = false;
    //bool won_jingle = false;

 
// setup function running at the beginning of the sketch
void setup() {

  Wire.begin (); // Call the connection Wire
  keypad.begin (makeKeymap (keys)); // Call the connection for keypad
  // Remove Wire and Keypad begins above and only call kpd.begin() ?

  Serial.begin(9600); // start serial monitor and set baud rate
  Serial.print("setup running now");
  pad_input.reserve(10); // set memory reserve for password

  //pinMode(led_green, OUTPUT); // initialize the LED pin as an output:
  //pinMode(btn_green, INPUT); // initialize the pushbutton pin as an input

  srv_podLift.attach(pin_srv_podLift); // attach the servo to pin
  srv_podLift.write(loc_neutral); // move servo to neutral
  srv_drawerLatch.attach(pin_srv_drawerLatch); // attach the servo to pin
  srv_drawerLatch.write(loc_neutral);
  delay(2000); // delay to let servos all go home

  // Set pin mode output for all the expansion board LEDs (Simon Says)
  // Turn off all LEDs to start

  ////TODO: Below can probably be looped to clean up a bit
  // pinMode(brd_1, 4, OUTPUT);
  // digitalWrite(brd_1, 4, LOW);
  // pinMode(brd_1, 5, OUTPUT); 
  // digitalWrite(brd_1, 5, LOW);
  // pinMode(brd_1, 6, OUTPUT);
  // digitalWrite(brd_1, 6, LOW);
  // pinMode(brd_1, 7, OUTPUT);
  // digitalWrite(brd_1, 7, LOW);
  
  // set all leds to output
  for(int Pin = 0; Pin < pinCount; Pin++) {
    pinMode(brd_1, ledArray[Pin], OUTPUT);
    digitalWrite(brd_1, ledArray[Pin], LOW);
  }

  // set all byttons to use pullup resistor
  for(int Pin = 0; Pin < pinCount; Pin++) {
    pinMode(brd_1, buttonArray[Pin], INPUT_PULLUP);
  }
  // pinMode(brd_1, 0, INPUT_PULLUP);
  // pinMode(brd_1, 1, INPUT_PULLUP);
  // pinMode(brd_1, 2, INPUT_PULLUP);
  // pinMode(brd_1, 3, INPUT_PULLUP);

  randomSeed(analogRead(0)); //shuffle the random functions with analog noise as seed

  // Blink the LEDs in a row
  blinkLightRow();
  //blinkLightRow();
  //blinkLightRow();
  



}

// main loop function
void loop() {
  //testSimonButtons();
  //testKeypad();
  if (!won_simon) {
    simonSays();
  }
  else if (won_simon && !won_password) {
    passwordMatch();
  }
  else if (won_simon && won_password) {
    openingDrawerSound();
    delay(500);
    openGiftDrawer();
    delay(200);
  }
  else {
    Serial.println("condition combo error");

  }

  //testContinuousServo(srv_drawerLatch);

 }

// ################################################

//// ##### PUZZLE FUNCTIONS #####
  // functions for puzzle interactions go below

  void preGame_simon() {
    // check if enough time has elapsed between leds lighting up
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > interval1) {
      //if enough time has elapsed update lights
      if (ledState == 0) {
        Serial.println("pre green");
        setLed(0);
        ledState = 1;
      }
      else if (ledState == 1) {
        Serial.println("pre red");
        setLed(1);
        ledState = 2;
      }
      else if (ledState == 2) {
        Serial.println("pre yellow");
        setLed(2);
        ledState = 3;
      }
      else if (ledState == 3) {
        Serial.println("pre blue");
        setLed(3);
        ledState = 0;
      }
      //reset time 
      previousMillis = currentMillis;
    }
  }

  void gamePlay_simon() {
    Serial.println("gamePlay - simon");
    //pre-load array with random numbers for game
    for ( int i=0; i<simonRounds; i++) {
      //use floor to round number down to nearest integer and random to choose random numbers from 0 to 3 to put in the gameValues_simon array.
      gameValues_simon[i] = floor(random(0,4));
      Serial.println(gameValues_simon[i], DEC);
    }

    
    Serial.print("Starting Round ");
    Serial.println(roundNum, DEC);
    for (int r=0; r<simonRounds; r++) {
      displayClue(roundNum);
    
      for (int i=0; i<=roundNum; i++) {      
        Serial.print("Wainting for input ");
        Serial.print(i+1, DEC);
        Serial.print(" of ");
        Serial.print(roundNum+1, DEC);
        Serial.println("");
    
        //wait for user input and see if they're correct, if correct wait for next input etc.. if not correct, fail
        if (waitForInput(gameValues_simon[i])) {
          //correct input
          if (i==roundNum) {
            Serial.println("Correct");
            //check if final round 
            if (roundNum == simonRounds-1) {
              Serial.println("You Win!");
              // set gameState to 2 to run winning fuction
              gameState_simon = 2;
              return;
            }
            else {
              Serial.println("Continue");
            }
          }
        }
        else {
          //incorrect input
          Serial.println("You Lose!");
          // set gameState to 3 to run losing function
          gameState_simon = 3;
          return;
        }
      }
      //User made it through the round, increment round number to continue
      roundNum++;
      //Delay briefly so user has time to see start of next clue
      delay(2000);
    }
  }

    void setLed(int ledNum) {
      //if the led number is less than zero turn all the leds off
      if (ledNum<0) {
        for (int i = 0; i<pinCount; i++) {
          digitalWrite(brd_1, ledArray[i], LOW);
        }
      }
      else {
        //turn on the specified led, turn off the other leds
        //turn one and only one led on - if i is the same as the number passed in from pre-game then turn on that light, if it is not the same turn off that light 
        // play corresponding sound
        for (int i = 0; i<pinCount; i++) {
          if (i == ledNum) {
            digitalWrite(brd_1, ledArray[i], HIGH);
            tone(buzzer, toneArray[i], 500);
          }
          else {
            digitalWrite(brd_1, ledArray[i], LOW);
          }
        }
      }
    }

  void displayClue(int roundNum) {
    //turn all lights off
    setLed(-1);
    for (int i=0; i<=roundNum; i++)
    {
      //turn on led that matches current round
      setLed(gameValues_simon[i]);
      delay(500);
      setLed(-1);
      delay(250);
    }
  }

  void simonSays() {
    // puzzle to match RGBY button-press to pattern shown on LEDs
    if (gameState_simon == 0){
      int button0 = digitalRead(brd_1, buttonArray[0]);
      int button1 = digitalRead(brd_1, buttonArray[1]);  
      int button2 = digitalRead(brd_1, buttonArray[2]); 
      int button3 = digitalRead(brd_1, buttonArray[3]);

      if (button0 && button1 && button2 && button3) {
        preGame_simon();
      }
      else {
        setLed(-1);
        roundNum = 0; //set the round number to zero so you start on the first round
        delay (1000);
        gameState_simon = 1;
      }
    }

    if (gameState_simon == 1) {
      gamePlay_simon();
    }

    if (gameState_simon == 2) {
      // win scenario - play winning sound and scroll through leds followed by lose display once for good transition
      // delay to give player time to release button press before winning sound plays
      delay(500);
      winning_sound();
      winDisplay();
      winDisplay();
      winDisplay();
      loseDisplay();
      liftKeypod();
      Serial.println("winDisplay - simon says");
      // delay briefly before running pre_game
      delay (1000);
      // reset gameState to zero to call pre_game
      won_simon = true;
      gameState_simon = 0;
    }

    if (gameState_simon == 3) {
      // lose scenario - play losing sound and flash leds for good transition.
      // delay to give player time to release button press before losing sound plays
      delay(500);
      losing_sound();
      loseDisplay();
      loseDisplay();
      loseDisplay();
      Serial.println("loseDisplay - simon says");
      // delay briefly before running pre_game
      delay (1000);
      // reset gameState to zero to call pre_game
      gameState_simon = 0;
    } 
  }

  boolean waitForInput(int buttonNum) {
    //read all the buttons continuously until a button is pressed, if a button is pressed check if it's correct and if so return true. if not, return false. 
    while(true) 
    {
    int button0 = digitalRead(brd_1, buttonArray[0]);
    int button1 = digitalRead(brd_1, buttonArray[1]);  
    int button2 = digitalRead(brd_1, buttonArray[2]); 
    int button3 = digitalRead(brd_1, buttonArray[3]); 
    
    //because we are using input_pullup ! indicted a button is pressed --> if any button is pressed go to the rest of the code
    if (!button0 || !button1 || !button2 || !button3)
    {
        //Delay briefly to avoid double counting button press
        delay(250);

        //light up the corresponding led when a button is pressed
        //play the corresponding sound when a button is pressed
        if (buttonNum == 0)
        {
          tone(buzzer, toneArray[0], 500);       // C note - octave 4
          digitalWrite(brd_1, ledArray[0], HIGH);
          delay(250);
          digitalWrite(brd_1, ledArray[0], LOW);
        }
        else if (buttonNum == 1)
        {
          tone(buzzer, toneArray[1], 500);       // D note - octave 4
          digitalWrite(brd_1, ledArray[1], HIGH);
          delay(250);
          digitalWrite(brd_1, ledArray[1], LOW);
        }
        else if (buttonNum == 2)
        {
          tone(buzzer, toneArray[2], 500);       // E note - octave 4
          digitalWrite(brd_1, ledArray[2], HIGH);
          delay(250);
          digitalWrite(brd_1, ledArray[2], LOW);
        }
        else if (buttonNum == 3)
        {
          tone(buzzer, toneArray[3], 500);       // G note - octave 4
          digitalWrite(brd_1, ledArray[3], HIGH);
          delay(250);
          digitalWrite(brd_1, ledArray[3], LOW);
        }
        
        //check if correct button was pressed
        if (buttonNum == 0 && button0 == 0)
        {
          Serial.println("0 Pressed");
          return true;
        }
        else if (buttonNum == 1 && button1 == 0)
        {
          Serial.println("1 Pressed");
          return true;
        }
        else if (buttonNum == 2 && button2 == 0)
        {
          Serial.println("2 Pressed");
          return true;
        }
        else if (buttonNum == 3 && button3 == 0)
        {
          Serial.println("3 Pressed");
          return true;
        }
        else
        {
          return false;
        }
      }
    }
  }








  void passwordMatch() {
    // Puzzle for entering the correct password into keypad
    char key = keypad.getKey();
    if (key){
      Serial.println(key);
      tone(buzzer, 1108, 100);
      if(key == '*') {
        pad_input = ""; // clear input password
      } else if(key == '#') {
      if(password == pad_input) {
        Serial.println("password is correct!");
        tone(buzzer, 277, 300);
        delay(200);
        tone(buzzer, 277, 300);
        won_password = true; // mark this puzzle as 'won'
        //openGiftDrawer();
        delay(100);
      } else {
        Serial.println("password is incorrect, try again");
        tone(buzzer, 24, 500);
        digitalWrite(brd_1, 6, HIGH);
        delay(200);
        tone(buzzer, 24, 500);
        delay(200);
        tone(buzzer, 24, 500);
        delay(200);
        digitalWrite(brd_1, 6, LOW);
        //// turn LED off:
        //digitalWrite(led_green, LOW);
        delay(100);
      }

      pad_input = ""; // clear input password
      } else {
        pad_input += key; // append new character to input password string
      }
    }

  }


// #################################################



//// ##### MECHANICAL FUNCTIONS #####
  // functions for box mechanichal movements go below

  // move the continuous servo to raise/lower the keypod cylinder
  void liftKeypod() {
    Serial.println("lifting key pod");
    Serial.println("spinning servo forward - move to 180");
    srv_podLift.write(180);
    delay(850);
    srv_podLift.write(90);
    Serial.println("stopping servo - move to to 90");
    delay(15000);
    Serial.println("spinning servo in reverse - move to to 0");
    srv_podLift.write(0);
    delay(850);
    srv_podLift.write(90);
    Serial.println("stopping servo - move to to 90");
    delay(200);
  }

  //move the servo controlling the gift drawer latch
  void openGiftDrawer(){
    Serial.println("opening gift drawer");
    Serial.println("spinning servo forward to open latch");
    srv_drawerLatch.write(180);
    delay(15000);
    srv_drawerLatch.write(90);
    Serial.println("stopping servo - move to to 90");
    delay(20000);
    Serial.println("spinning servo reverse to close latch");
    srv_drawerLatch.write(0);
    delay(15000);
  }

  //after win - have lights scroll quickly
  void winDisplay() {
    digitalWrite(brd_1, ledArray[0], HIGH);
    delay(75);
    digitalWrite(brd_1, ledArray[0], LOW);
    delay(75);
    digitalWrite(brd_1, ledArray[1], HIGH);
    delay(75);
    digitalWrite(brd_1, ledArray[1], LOW);
    delay(75);
    digitalWrite(brd_1, ledArray[2], HIGH);
    delay(75);
    digitalWrite(brd_1, ledArray[2], LOW);
    delay(75);
    digitalWrite(brd_1, ledArray[3], HIGH);
    delay(75);
    digitalWrite(brd_1, ledArray[3], LOW);
    delay(75);
    digitalWrite(brd_1, ledArray[2], HIGH);
    delay(75);
    digitalWrite(brd_1, ledArray[2], LOW);
    delay(75);
    digitalWrite(brd_1, ledArray[1], HIGH);
    delay(75);
    digitalWrite(brd_1, ledArray[1], LOW);
    delay(75);
    digitalWrite(brd_1, ledArray[0], HIGH);
    delay(75);
    digitalWrite(brd_1, ledArray[0], LOW);
    delay(75);
  }

  // blink all the colored simon LEDs in a row
  void blinkLightRow(){
    //TODO: This can also be looped when cleaning
    digitalWrite(brd_1, 4, HIGH);
    tone(buzzer, 36, 50);
    delay(100);
    digitalWrite(brd_1, 4, LOW);
    digitalWrite(brd_1, 5, HIGH);
    tone(buzzer, 36, 50);
    delay(100);
    digitalWrite(brd_1, 5, LOW);
    digitalWrite(brd_1, 6, HIGH);
    tone(buzzer, 36, 50);
    delay(100);
    digitalWrite(brd_1, 6, LOW);
    digitalWrite(brd_1, 7, HIGH);
    tone(buzzer, 36, 50);
    delay(100);
    digitalWrite(brd_1, 7, LOW);
  }
  
  void winning_sound(void) {
    Serial.println("playing winning sound");
    tone(buzzer, 294, 250);
    delay(200);
    tone(buzzer, 294, 250);
    delay(200);
    tone(buzzer, 294, 250);
    delay(200);
    tone(buzzer, 392, 500);
    delay(500);
    tone(buzzer, 392, 250);
    delay(200);
    tone(buzzer, 440, 250);
    delay(200);
    tone(buzzer, 392, 250);
    delay(200);
    tone(buzzer, 440, 250);
    delay(200);
    tone(buzzer, 494, 500);
  }

  void loseDisplay() {
    digitalWrite(brd_1, ledArray[0], HIGH);
    digitalWrite(brd_1, ledArray[1], HIGH);
    digitalWrite(brd_1, ledArray[2], HIGH);
    digitalWrite(brd_1, ledArray[3], HIGH);
    delay(200);
    digitalWrite(brd_1, ledArray[0], LOW);
    digitalWrite(brd_1, ledArray[1], LOW);
    digitalWrite(brd_1, ledArray[2], LOW);
    digitalWrite(brd_1, ledArray[3], LOW);
    delay(200);
  }

  void losing_sound(void) {
    Serial.println("losing_sound");
    tone(buzzer, 98, 250);
    delay(250);
    tone(buzzer, 93, 250);
    delay(250);
    tone(buzzer, 87, 250);
    delay(250);
  }

  void openingDrawerSound() {
    Serial.println("playing gift drawer sound");
    #define NOTE_B0  31
    #define NOTE_C1  33
    #define NOTE_CS1 35
    #define NOTE_D1  37
    #define NOTE_DS1 39
    #define NOTE_E1  41
    #define NOTE_F1  44
    #define NOTE_FS1 46
    #define NOTE_G1  49
    #define NOTE_GS1 52
    #define NOTE_A1  55
    #define NOTE_AS1 58
    #define NOTE_B1  62
    #define NOTE_C2  65
    #define NOTE_CS2 69
    #define NOTE_D2  73
    #define NOTE_DS2 78
    #define NOTE_E2  82
    #define NOTE_F2  87
    #define NOTE_FS2 93
    #define NOTE_G2  98
    #define NOTE_GS2 104
    #define NOTE_A2  110
    #define NOTE_AS2 117
    #define NOTE_B2  123
    #define NOTE_C3  131
    #define NOTE_CS3 139
    #define NOTE_D3  147
    #define NOTE_DS3 156
    #define NOTE_E3  165
    #define NOTE_F3  175
    #define NOTE_FS3 185
    #define NOTE_G3  196
    #define NOTE_GS3 208
    #define NOTE_A3  220
    #define NOTE_AS3 233
    #define NOTE_B3  247
    #define NOTE_C4  262
    #define NOTE_CS4 277
    #define NOTE_D4  294
    #define NOTE_DS4 311
    #define NOTE_E4  330
    #define NOTE_F4  349
    #define NOTE_FS4 370
    #define NOTE_G4  392
    #define NOTE_GS4 415
    #define NOTE_A4  440
    #define NOTE_AS4 466
    #define NOTE_B4  494
    #define NOTE_C5  523
    #define NOTE_CS5 554
    #define NOTE_D5  587
    #define NOTE_DS5 622
    #define NOTE_E5  659
    #define NOTE_F5  698
    #define NOTE_FS5 740
    #define NOTE_G5  784
    #define NOTE_GS5 831
    #define NOTE_A5  880
    #define NOTE_AS5 932
    #define NOTE_B5  988
    #define NOTE_C6  1047
    #define NOTE_CS6 1109
    #define NOTE_D6  1175
    #define NOTE_DS6 1245
    #define NOTE_E6  1319
    #define NOTE_F6  1397
    #define NOTE_FS6 1480
    #define NOTE_G6  1568
    #define NOTE_GS6 1661
    #define NOTE_A6  1760
    #define NOTE_AS6 1865
    #define NOTE_B6  1976
    #define NOTE_C7  2093
    #define NOTE_CS7 2217
    #define NOTE_D7  2349
    #define NOTE_DS7 2489
    #define NOTE_E7  2637
    #define NOTE_F7  2794
    #define NOTE_FS7 2960
    #define NOTE_G7  3136
    #define NOTE_GS7 3322
    #define NOTE_A7  3520
    #define NOTE_AS7 3729
    #define NOTE_B7  3951
    #define NOTE_C8  4186
    #define NOTE_CS8 4435
    #define NOTE_D8  4699
    #define NOTE_DS8 4978
    #define REST      0

    int tempo = 140;

    int melody[] = {

    // We Wish You a Merry Christmas
    // Score available at https://musescore.com/user/6208766/scores/1497501
    
    NOTE_C5,4, //1
    NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
    NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
    NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
    NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
    NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
    NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
    NOTE_D5,4, NOTE_G5,4, NOTE_E5,4

    // NOTE_F5,2, NOTE_C5,4, //8 
    // NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
    // NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
    // NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
    // NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
    // NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
    // NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
    // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
    // NOTE_F5,2, NOTE_C5,4

    // NOTE_F5,4, NOTE_F5,4, NOTE_F5,4,//17
    // NOTE_E5,2, NOTE_E5,4,
    // NOTE_F5,4, NOTE_E5,4, NOTE_D5,4,
    // NOTE_C5,2, NOTE_A5,4,
    // NOTE_AS5,4, NOTE_A5,4, NOTE_G5,4,
    // NOTE_C6,4, NOTE_C5,4, NOTE_C5,8, NOTE_C5,8,
    // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
    // NOTE_F5,2, NOTE_C5,4, 
    // NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
    // NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
    
    // NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8, //27
    // NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
    // NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
    // NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
    // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
    // NOTE_F5,2, NOTE_C5,4,
    // NOTE_F5,4, NOTE_F5,4, NOTE_F5,4,
    // NOTE_E5,2, NOTE_E5,4,
    // NOTE_F5,4, NOTE_E5,4, NOTE_D5,4,
    
    // NOTE_C5,2, NOTE_A5,4,//36
    // NOTE_AS5,4, NOTE_A5,4, NOTE_G5,4,
    // NOTE_C6,4, NOTE_C5,4, NOTE_C5,8, NOTE_C5,8,
    // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
    // NOTE_F5,2, NOTE_C5,4, 
    // NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
    // NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
    // NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8, 
    // NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
    
    // NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,//45
    // NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
    // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
    // NOTE_F5,2, NOTE_C5,4,
    // NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
    // NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
    // NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
    // NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
    
    // NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, //53
    // NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
    // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
    // NOTE_F5,2, REST,4
  };

  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;

  int divider = 0, noteDuration = 0;

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzer);
  }

    // coppied from https://github.com/robsoncouto/arduino-songs/blob/master/merrychristmas/merrychristmas.ino
}

// #################################################


//// ##### DEVELOPMENT FUNCTIONS #####
  // any functions below are for development, and can be removed to condense the sketch

  // function to test the movement of continuous servos
  // loop this when 0'ing out the potentiometer on one
  void testContinuousServo(Servo servoName) {
    servoName.write(180);
    Serial.println("spinning servo forward - move to 180");
    // forward opens latch to release
    delay(15000);
    servoName.write(90);
    Serial.println("stopping servo - move to to 90");
    delay(5000);
    servoName.write(0);
    Serial.println("spinning servo in reverse - move to to 0"); 
    //reverse closes latch
    delay(15000);
    servoName.write(90);
    Serial.println("stopping servo - move to to 90");
    delay(5000);
  }

  // function to test functionality of all the Simon Says buttons, their LEDs, and buzzer sounds
  void testSimonButtons() {
    // set the current state of each button 
    int btn_yellow = digitalRead(brd_1, 0);
    int btn_blue = digitalRead(brd_1, 1);
    int btn_red = digitalRead(brd_1, 2);
    int btn_green = digitalRead(brd_1, 3);

    //Check the state of each button. Delay to skip bumps, play sound, blink LED
    //TODO: Combine these into one loop
    if(!btn_yellow){
      delay(150);
      tone(buzzer, 261, 150);       // C note - octave 4
      digitalWrite(brd_1, 4, HIGH);
      delay(250);
      digitalWrite(brd_1, 4, LOW);
    }

    if(!btn_blue){
      delay(150);
      tone(buzzer, 293, 150);       // D note - octave 4
      digitalWrite(brd_1, 5, HIGH);
      delay(250);
      digitalWrite(brd_1, 5, LOW);
    }

    if(!btn_red){
      delay(150);
      tone(buzzer, 329, 150);       // E note - octave 4
      digitalWrite(brd_1, 6, HIGH);
      delay(250);
      digitalWrite(brd_1, 6, LOW);
    }

    if(!btn_green){
      delay(150);
      tone(buzzer, 392, 150);       // G note - octave 4
      digitalWrite(brd_1, 7, HIGH);
      delay(250);
      digitalWrite(brd_1, 7, LOW);
    }
  }

  // function to test the keypad buttons
  // Prints each press into the serial monitor
  void testKeypad() {
    char key = keypad.getKey (); // Create a variable named key of type char to hold the characters pressed
 
    if (key) {// if the key variable contains
      Serial.println (key); // output characters from Serial Monitor
    }
  }


  
// #################################################