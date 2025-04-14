//ECE 6372 - ADVANCED HARDWARE DESIGN - DR. YUHUA CHEN - FALL 2024
//VENI VIDI VICI TEAM - MY TRAN - UH ID 2095966
//DECEMBER 12, 204

#include <LiquidCrystal.h>
#include "SR04.h"

//Pin configuration for Ultrasonic Sensor HC-SR04
#define TRIG_PIN 14
#define ECHO_PIN 15
SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long distance; //value to read from the sensor

//Define the number of password digits
const int passwordLength = 5;

//pin configuration for 3 color LED
int redLED = 17;
int blueLED = 18;
int whiteLED = 19;

//pin configuration for 3 input buttons
int redButton = 6;
int blueButton = 5;
int whiteButton = 4;

//Pin configuration for LCD screen
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

//Create a array to hold password input from user
int userInput[passwordLength];
int inputIndex = 0;
bool unlocked = false;

//Create a array to hold new generated password
int password[passwordLength];
String colorNames[] = {"Red", "Blue", "White"};

void setup() {
  //Setting up the serial communication
   Serial.begin(9600);
   //Setting up the iddling state message on LCD screen
   lcd.begin(16, 2);
   lcd.print("Veni Vidi Vici");
   lcd.setCursor(0, 1);
   lcd.print("System Idling");
   //Delay 1 second
   delay(1000);
}

void loop() {
  //Sensor read analog value using SR04 library function
   distance = sr04.Distance();
   Serial.print(distance);
   Serial.println(" cm");

   //Making the LED pin as output
   pinMode(redLED, OUTPUT);
   pinMode(blueLED, OUTPUT);
   pinMode(whiteLED, OUTPUT);
   
   //Making the switch as input
   pinMode(redButton, INPUT_PULLUP);
   pinMode(blueButton, INPUT_PULLUP);
   pinMode(whiteButton, INPUT_PULLUP);

   // Turn off all LEDs until buttons are pressed
   digitalWrite(redLED, LOW);
   digitalWrite(blueLED, LOW);
   digitalWrite(whiteLED, LOW);

  //when sensor reads a distance smaller than 20cm, system will triggered
   if (distance < 20 && !unlocked) {  
      // Generate new password every time a human is detected
      generateRandomPassword();
      
      // Print the generated password to Serial Monitor
      Serial.print("Generated Password: ");
      for (int i = 0; i < passwordLength; i++) {
         Serial.print(colorNames[password[i]]);
         Serial.print(" ");
      }
      Serial.println();

      lcd.clear();
      lcd.print("HUMAN DETECTED: ");
      lcd.setCursor(0, 1);
      lcd.print("Enter password: ->");

      // Wait for password input
      while (inputIndex < passwordLength) {
         int buttonPressed = getButtonPress();
         if (buttonPressed != -1) {
            userInput[inputIndex] = mapButtonToColorIndex(buttonPressed); // Map button to color index
            inputIndex++;
            lightupLED(buttonPressed); 
            lcd.setCursor(0, 1);
            lcd.print("Entered: ");
            lcd.print(inputIndex);
         }
      }

      // Validate the entered password
      if (checkPassword()) {
         lcd.clear();
         lcd.print("Password Correct!"); //Case: Password correct
         lcd.setCursor(0, 1);
         lcd.print("Welcome home!");
         unlocked = true;
         delay(5000); //delay this state for 5 sec and exit
         // Perform a random action here for unlocking simulation
      } else {
         lcd.clear();
         lcd.print("Wrong Password!"); //Case: password incorrect
         lcd.setCursor(0, 1);
         lcd.print("Try again please");
         delay(3000); //delay this state for 3 sec and exit
      }

      // Return to idle state
      lcd.clear();
      lcd.print("Veni Vidi Vici");
      lcd.setCursor(0, 1);
      lcd.print("System Idling");
      delay(2000); //delay the idling state for 2 sec and repeat
      
      // Reset for the next password entry
      inputIndex = 0;
      unlocked = false;
   }
   delay(1000);
}

//Function to check if button is pressed
int getButtonPress() {
  if (digitalRead(redButton) == LOW) {
    delay(300);
    return redButton;  // Red button -> 6
  } else if (digitalRead(blueButton) == LOW) {
    delay(300);
    return blueButton; // Blue button -> 5
  } else if (digitalRead(whiteButton) == LOW) {
    delay(300);
    return whiteButton; // White button -> 4
  }
  return -1;
}

//Function to validate the input password to the generated password
bool checkPassword() {
  for (int i = 0; i < passwordLength; i++) {
    if (userInput[i] != password[i]) {
      return false;
    }
  }
  return true;
}

//Function to activate the LED whenever button is pressed
void lightupLED(int button) {
  if (button == redButton) {
    digitalWrite(redLED, HIGH);
    delay(200);
    digitalWrite(redLED, LOW);
  } else if (button == blueButton) {
    digitalWrite(blueLED, HIGH);
    delay(200);
    digitalWrite(blueLED, LOW);
  } else if (button == whiteButton) {
    digitalWrite(whiteLED, HIGH);
    delay(200);
    digitalWrite(whiteLED, LOW);
  }
}

//Function to randonmize new password
void generateRandomPassword() {
   for (int i = 0; i < passwordLength; i++) {
     password[i] = random(0, 3);  // Generate values between 0 and 2 (inclusive)
   }
}

// Function to map button pin numbers to the corresponding color index
//Since I randomize the new password with 0 1 2 3 4, but the getButtonPressed return the pin of the button
//I need another function to match them together for faster use
int mapButtonToColorIndex(int button) {
  if (button == redButton) {
    return 0; // Red button -> 0
  } else if (button == blueButton) {
    return 1; // Blue button -> 1
  } else if (button == whiteButton) {
    return 2; // White button -> 2
  }
  return -1; // Invalid button
}
