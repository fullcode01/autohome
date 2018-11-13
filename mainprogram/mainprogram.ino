// Include used Library
#include "IRLed.h"
#include <IOManager.h>
#include <IRremote.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// Create LiquidCrystal instance
const int rs = 12, en = 11, d4 = 8, d5 = 7, d6 = 4, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Create Servo instances
Servo frontDoorServo;
Servo bedRoomAServo;
Servo bedRoomBServo;
Servo toiletServo;
Servo garageServo;

// Create IRrecv by using pin number 10
// and decode_results instance
int RECV_PIN = 10;
IRrecv irrecv(RECV_PIN);
decode_results results;

// Setup goes here
void setup() {
  // Start the serial communication on 9600 baud rate;
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver

  // Attach Servos
  frontDoorServo.attach(3);
  bedRoomAServo.attach(5);
  bedRoomBServo.attach(6);
  toiletServo.attach(9);
  garageServo.attach(13);
}



// Main program
void loop() {
  String passwordSet = "04072225";    // Password saved
  String passwordEntered = "";        // String to track the password entered by user
  String mainMenu[] = {               // Main menus
    "1: Lamps      <<", 
    "2: Doors      >>",
    "3: Lock       <<",
    "              >>"
    };
  String lampMenu[] = {               // Lamp menus
    "1: Guest Room <<",
    "2: Bed Room 1 >>",
    "3: Bed Room 2 <<",
    "4: Family Room>>",
    "4: Dining Room<<",
    "4: Toilet     >>",
    "4: Garage     <<",
    "              >>"
  };
  String doorMenu[] = {             // Door menus
    "1: Front Door <<",
    "2: Bed 1      >>",
    "3: Bed 2      <<",
    "4: Toilet     >>",
    "5: Garage     <<",
    "              >>"
  };

  // Variable declaration

  // Variable to track showed menu index
  int mainMenuIndex = 0;        
  int doorMenuIndex = 0;        
  int lampMenuIndex = 0;

  // Variables to track servo position
  int frontDoorPosition = 50;
  int bedroomADoorPosition = 50;
  int bedroomBDoorPosition = 50;
  int toiletDoorPosition = 50;
  int garageDoorPosition = 50;

  // Variables to mark if the associated menu is shown or not
  bool doorMenuFlag = false;    
  bool lampMenuFlag = false;    
  bool mainMenuFlag = false;

  // Variable to mark
  bool lockStatusFlag = true;

  // Variable to mark if a fire has broke out or not
  bool fireFlag = false;

  // Main menu has been printed or not
  bool mainMenuPrintFlag = false;

  // LCD related variables
  int xCursorPos = 0;
  int currentPos = 0;

  // LEDs
  IOManager LDRin(A0, INPUT);
  IRLed guestRoomLED(A2, OUTPUT);
  IRLed bedRoomALED(A3, OUTPUT);
  IRLed bedRoomBLED(A4, OUTPUT);
  IRLed kitchenLED(A5, OUTPUT);
  IRLed *ledArray[] = {
    &guestRoomLED,
    &bedRoomALED,
    &bedRoomBLED,
    &kitchenLED,
  };

  // Move all servo to closed positon
  moveServo(garageServo, 70, 85, 8);
  moveServo(frontDoorServo, 90, 102, 8);
  moveServo(bedRoomAServo, 90, 102, 8);
  moveServo(bedRoomBServo, 90, 102, 8);
  moveServo(toiletServo, 90, 102, 8);

  // Start the LCD and print the string at 0, 0
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Enter Password :");

  // This will run forever
  while(1) {

    /* Thermal Sensor */
    // Variable which hold the value of the thermal sensor
    double sensorValue = thermistor(analogRead(A1));

    // If the value exceed 100 Celcius degrees, Blink all LEDs
    if (sensorValue > 100){
      long millisTiming = millis();
      while (sensorValue > 50) {
        sensorValue = thermistor(analogRead(A1));
        if (millis() - millisTiming > 1000) {
          lcd.clear();
          millisTiming = millis();
        }
        else {
          lcd.clear();
          lcd.print("Fire Fire Fire");
        }
        for (int i = 0; i < 5; i++) ledArray[i]->setHigh(true);
          delay(200);
       for (int i = 0; i < 5; i++) ledArray[i]->setLow(true);
          delay(200);
      }//*/
      delay(2000);
      mainMenuPrintFlag = false;
      doorMenuFlag = false;
      lampMenuFlag = false;
      mainMenuIndex = 0;
      fireFlag = true;
    }
    /* End of Thermal Sensor */

    /* LDR Sensor */
    // Check wether the LDR input value exceed 850 or not
    // If yes, the LEDs, will be turned on, otherwise they will 
    // be turned off
    if (LDRin.getAnalog() > 850) {
       for (int i = 0; i < 5; i++) ledArray[i]->setHigh();
    } else {
      for (int i = 0; i < 5; i++) {
        if (!ledArray[i]->isTurnedOnbyIR()) ledArray[i]->setLow();
      }
    }
    /* End of LDR Sensor */

    // Check if the system is locked or not
    if (!lockStatusFlag) {                          
      // If not locked and the main menu haven't printed yet,
      // the main menu will be shown on LCD
      if (mainMenuFlag && !mainMenuPrintFlag) {
        if (fireFlag) {
          lcd.clear();
          fireFlag = false;                        // Change fireFlag to false
        }
        printToLCD(mainMenu, &mainMenuIndex);
        mainMenuPrintFlag = true;                  // Changed to true so the main menu wouldn't be printed over and over
      }
    } else {                                       // Otherwise "Enter Password: " will still be prompted
      if (mainMenuFlag) {
        lcd.clear();
        mainMenuFlag = false;
      }
      printToLCD(0, 0, "Enter Password: ");
    }
  
    // If IR signal is detected
    if (irrecv.decode(&results)) {
      Serial.println(results.value);
      
      // Scroll to the next item(s) of the shown menu
      if (results.value == 16712445) {
        if (mainMenuFlag) printNextMenu(mainMenu, &mainMenuIndex, 3);
        else if(lampMenuFlag) printNextMenu(lampMenu, &lampMenuIndex, 7);
        else if (doorMenuFlag) printNextMenu(doorMenu, &doorMenuIndex, 5);
      }

      // Scroll to the previous item(s) of the shown menu
      if (results.value == 16720605) {
        if (mainMenuFlag) printPreviousMenu(mainMenu, &mainMenuIndex, 6);
        else if (lampMenuFlag) printPreviousMenu(lampMenu, &lampMenuIndex, 10);
        else if (doorMenuFlag) printPreviousMenu(doorMenu, &doorMenuIndex, 8);
      }

      // Check if the home is unlocked then print the main menu
       if (results.value == 16761405 && !lockStatusFlag) {
          lcd.clear();
          for (int i = 0; i < 2; i++) printToLCD(0, i, mainMenu[i]);
          mainMenuFlag = true;
          lampMenuFlag = false;
          lampMenuIndex = 0;
          doorMenuIndex = 0;
          doorMenuFlag = false;
        }

        // Delete
        if (results.value == 16769055) {
          if (lockStatusFlag && xCursorPos > 0) {
            passwordEntered = passwordEntered.substring(0, passwordEntered.length() - 1);
            printToLCD(--xCursorPos, 1, " ");
          }
        }

        // 0
        // If the system is still locked, "0" will be added to passwordEntered,
        // otherwise this button doesn't have any other function.
        if (results.value == 16738455) {
          if (lockStatusFlag) {
            printToLCD(xCursorPos++, 1, "0");
            passwordEntered = passwordEntered + "0";
          }
        }

        // 1
        if (results.value == 16724175) {
          // If the system is still locked, "1" will be added to passwordEntered
          if (lockStatusFlag) {
            printToLCD(xCursorPos++, 1, "1");
            passwordEntered = passwordEntered + "1";
          }
          // If its already unlocked and the main menu is shown,
          // print the lamp menu 
          else if (mainMenuFlag) {
            lcd.clear();
            printToLCD(lampMenu, &lampMenuIndex);
            mainMenuFlag = false;
            lampMenuFlag = true;
          }
          // If the lamp menu is shown, turn on the guest room LED
          else if (lampMenuFlag) 
              guestRoomLED.getStatus()?guestRoomLED.setLow(true):guestRoomLED.setHigh(true);
          // Or if the door menu is shown, open the front door or close it.
          else if (doorMenuFlag) {
              // Close the door
              if (!frontDoorPosition) {
                moveServo(frontDoorServo, &frontDoorPosition, 102, 5);
              }
              // Open the door 
              else {
                moveServo(frontDoorServo, &frontDoorPosition, 0, 5);
              }
            }
        }

        // 2
        if (results.value == 16718055) {
          // If the system is still locked, "2" will be added to passwordEntered
          if (lockStatusFlag) {
            printToLCD(xCursorPos++, 1, "2");
            passwordEntered = passwordEntered + "2";
          }
          // If its already unlocked and the main menu is shown,
          // print the door menu 
          else if (mainMenuFlag) {
            lcd.clear();
            printToLCD(doorMenu, &doorMenuIndex);
            mainMenuFlag = false;
            doorMenuFlag = true;
          }
          // If the menu shown is lamp menu, turn on or turn off bed room A LED 
          else if (lampMenuFlag) 
              bedRoomALED.getStatus()?bedRoomALED.setLow(true):bedRoomALED.setHigh(true);
          // If the menu shown is door menu, open or close the bed room A door
          else if (doorMenuFlag) 
              bedroomADoorPosition==0?moveServo(bedRoomAServo, &bedroomADoorPosition, 102, 8):
                      moveServo(bedRoomAServo, &bedroomADoorPosition, 0, 8);
            
        }

        // 3
        if (results.value == 16743045) {
          // If the system is still locked, "3" will be added to passwordEntered
          if (lockStatusFlag) {
            printToLCD(xCursorPos++, 1, "3");
            passwordEntered = passwordEntered + "3";
          }
          // If its already unlocked and the main menu is currently shown,
          // lock the system 
          else if (mainMenuFlag){
            mainMenuPrintFlag = false;
            lockStatusFlag = true;
          }
          // If the menu shown is lamp menu, turn on or turn off the bed room B LED
            else if (lampMenuFlag) 
              bedRoomBLED.getStatus()?bedRoomBLED.setLow(true):bedRoomBLED.setHigh(true);
          // If the menu shown is door menu, open or close the bed room B door
            else if (doorMenuFlag) 
              bedroomBDoorPosition==0?moveServo(bedRoomBServo, &bedroomBDoorPosition, 102, 8):
                      moveServo(bedRoomBServo, &bedroomBDoorPosition, 0, 8);
        }

        // 4
        if (results.value == 16716015) {
          // If the system is still locked, "4" will be added to passwordEntered
          if (lockStatusFlag) {
            printToLCD(xCursorPos++, 1, "4");
            passwordEntered = passwordEntered + "4";
          }
          // If the menu shown is lamp menu, turn on or turn off the kitchen, family room, toilet
          // and garage LEDs 
          else if (lampMenuFlag) 
              kitchenLED.getStatus()?kitchenLED.setLow(true):kitchenLED.setHigh(true);
           // If the menu shown is door menu, open or close the toilet door   
          else if (doorMenuFlag) 
              toiletDoorPosition==0?moveServo(toiletServo, &toiletDoorPosition, 102, 8):
                      moveServo(toiletServo, &toiletDoorPosition, 0, 8);
        }

        // 5
        if (results.value == 16726215) {
          // If the system is still locked, "5" will be added to passwordEntered
          if (lockStatusFlag) {
            printToLCD(xCursorPos++, 1, "5");
            passwordEntered = passwordEntered + "5";
          } 
          // If the door menu is shown, open or close the garage
          else if (doorMenuFlag) 
              garageDoorPosition==0?moveServo(garageServo, &garageDoorPosition, 85, 8):
                    moveServo(garageServo, &garageDoorPosition, 0, 8);
        }

        // 6
        if (results.value == 16734885) {
          // If the system is still locked, "6" will be added to passwordEntered
          if (lockStatusFlag) {
            printToLCD(xCursorPos++, 1, "6");
            passwordEntered = passwordEntered + "6";
          }
        }

        // 7
        if (results.value == 16728765) {
          // If the system is still locked, "7" will be added to passwordEntered
          if (lockStatusFlag) {
            printToLCD(xCursorPos++, 1, "7");
            passwordEntered = passwordEntered + "7";
          }
        }

        // 8
        if (results.value == 16730805) {
          // If the system is still locked, "8" will be added to passwordEntered
          if (lockStatusFlag) {
            printToLCD(xCursorPos++, 1, "8");
            passwordEntered = passwordEntered + "8";
          }
        }

        // 9
        if (results.value == 16732845) {
          // If the system is still locked, "9" will be added to passwordEntered
          if (lockStatusFlag) {
            printToLCD(xCursorPos++, 1, "9");
            passwordEntered = passwordEntered + "9";
          }
        }
        irrecv.resume();
      } // Receive the next value//*/
    delay(100);

    // First check if the user has entered 8 values
    if (passwordEntered.length() == 8) {
     // Check if the password is match
     // If matched, the system will be unlcoked and "Welcome!!" will be printed
     // and blink for 5 times.
     if (passwordEntered.equals(passwordSet)) {
      Serial.println("Unlocked");
      moveServo(frontDoorServo, &frontDoorPosition, 0, 8);        // Open the door
      for (int i = 0; i < 5; i++) {
        printToLCD(4, 0, "Welcome!!", true);
        delay(700);
        lcd.clear();
        delay(300);
      }
      moveServo(frontDoorServo, &frontDoorPosition, 102, 8);     // Close it again
      lockStatusFlag = false;                                    // Change lock status to false
      mainMenuFlag = true;                                       // Change to true so the main menu will be printed
      passwordEntered = "";                                      // Reset password Entered to empty so it could be used again next time the home is locked
     }
     // if not, print "Wrong password and blink it 5 times before
     // prompting "Enter Password: " again
     else {
      for (int i = 0; i < 5; i++) {
        printToLCD(0, 0, "Wrong Password!", true);
        delay(700);
        lcd.clear();
        delay(300);
      }
      printToLCD(0, 0, "Enter Password: ", true);
     }
     xCursorPos = 0;
     passwordEntered = "";                                      // Reset password Entered to empty so it could be used again
    }
  }
}

// Simple print to LCD function
void printToLCD(int x, int y, String message) {
 lcd.setCursor(x, y);
 lcd.print(message); 
}

// Clear and then print to the LCD
void printToLCD(int x, int y, String message, bool clearFlag) {
 lcd.clear();
 printToLCD(x, y, message);
}

// Print an array of String to LCD
void printToLCD(String message[], int *index) {
  for(int i = 0; i < 2; i++) {
    printToLCD(0, i, message[(*index)++]);
  }
}

// Print a message on LCD with blinking animation
void printToLCD(int x, int y, int onDelay, int offDelay, int loopCount, String message) {
  for (int i = 0; i <= loopCount; i++) {
    printToLCD(x, y, message, true);
    delay(onDelay);
    lcd.clear();
    delay(offDelay);
  }
}

// Move servo from current position to target position with a delay duration for each iteration
void moveServo(Servo servo, int currentPos, int targetPos, int duration) {
  while (currentPos != targetPos) {
    servo.write(currentPos);
    currentPos > targetPos?currentPos--:currentPos++;
    delay(duration);
  }
}

// Move servo from current position to target position with a delay for each iteration and
// assign value of targetPos to currentPos
void moveServo(Servo servo, int *currentPos, int targetPos, int duration) {
  moveServo(servo, (*currentPos), targetPos, duration);
  (*currentPos) = targetPos;
}

// Convert ADC reading from thermistor to celcius degrees
double thermistor(int rawADC) {
  double temp;
  temp = log(10000.0 * ((1024.0 / rawADC - 1)));
  temp = 1 / (0.001129148 + (0.000234125 + (0.000000087674 * temp * temp)) * temp);
  temp = temp - 273.15;
  return temp;  
}

// Print next menu function
void printNextMenu(String menu[], int *menuIndex, int bound) {
  (*menuIndex) > bound?(*menuIndex) = 0:*menuIndex;
  lcd.clear();
  printToLCD(menu, menuIndex);
}

// Print previous menu function
void printPreviousMenu(String menu[], int *menuIndex, int bound) {
  (*menuIndex) == 2?(*menuIndex) = bound:*menuIndex;
  lcd.clear();
  (*menuIndex) -= 4;
  printToLCD(menu, menuIndex);
}


