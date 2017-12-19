#include <Bounce2.h>
#include <LiquidCrystal.h>

// Variables for LCD pins:
const int D7 = 13;
const int D6 = 12;
const int D5 = 11;
const int D4 = 10;
const int EN = 8;
const int RS = 9;   
const int V0 = 6;

// Variables for display text size:
const int columns = 16;
const int rows = 2;

// Variable for contrast changing:
int contrast = 0;

// Variables for button pins:
const int button1 = 2;
const int button2 = 3;

// Variable that tracks what the current screen is:
// 1 = Welcome, 2 = Input1, 3 = Input2, 4 = Result, 5 = Contrast
int currentScreen = 1;
int lastScreen = 1;

// Variables for contrast:
int minimum = 0;
int maximum = 60;
int rate = 5;

// Variables for storing user input:
int firstByte[8] = {0,0,0,0,0,0,0,0};
int secondByte[8] = {0,0,0,0,0,0,0,0};

// Variable to track cursor location when using inputting a byte:
int currentLoc = 0; 

// Instantiate the display: 
LiquidCrystal lcd(RS,EN,D4, D5, D6, D7);

// Instantiate the debouncers for each button: 
Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();

// Function that clears everything to the right of the column/row entered:
void partialClear(int col, int row){
    lcd.setCursor(col,row);
    lcd.write("                ");
    lcd.setCursor(col,row);
}

// Function that prints text to the display, based on the current screen number:
void displayScreen(int screenNum) {
    if (screenNum == 1) {
        lcd.print("My Binary");
        lcd.setCursor(0,2);
        lcd.print("Calculator:");
    } else if (screenNum == 2) {
        lcd.clear();
        lcd.print("Enter 1st byte:");
        displayArray(firstByte);
        lcd.setCursor(currentLoc,2);
        lcd.cursor();
    } else if (screenNum == 3) {
        lcd.clear();
        lcd.print("Enter 2nd byte:");
        displayArray(secondByte);
        lcd.setCursor(currentLoc,2);
        lcd.cursor();
    } else if (screenNum == 4) {
        lcd.noCursor();
        lcd.clear();
        lcd.print("Results:");
        lcd.setCursor(0,2);
        
        // Sum of the 2 input bytes converted from binary to int, 
        //    then the sum is converted back to binary:
        lcd.print(intToBin(binToInt(firstByte) + binToInt(secondByte)));
    } else if (screenNum == 5) {
        lcd.noCursor();
        lcd.clear();
        displayContrast();
        lcd.setCursor(0,2);
        if (contrast == minimum) {
            lcd.print("Minimum");
        } else if (contrast == maximum){
            lcd.print("Maximum");
        }
    }
}

// Function used to display the contrast number:
void displayContrast() {
    partialClear(0,0);
    lcd.print("Contrast: ");
    lcd.print(contrast);
}

// Function used to display the byte arrays:
void displayArray(int Array[]){
  for (int i = 0; i < 8; i++){
    lcd.setCursor(i,2);
    lcd.print(Array[i]);
  }
}

// Function that resets the 2 bytes for input:
void resetBytes(){
  for (int i = 0; i < 8; i++){
    firstByte[i] = 0;
    secondByte[i] = 0;
  }
}

// Function that takes in an array (of bits) converts it to an int, returning that number
int binToInt(int Array[]) {
  int result = 0;
  int bitArray[8] {128,64,32,16,8,4,2,1};
  for (int i = 0; i < 8; i++){
    bitArray[i] = bitArray[i] * Array[i];
    result += bitArray[i];    
  }
  return result;
}

// Function that converts an int to a binary String:
String intToBin(int decimal) {
  String resultString;
  if (String(decimal,BIN).length()<9) {
    int zeros = 8 - String(decimal,BIN).length();
    for (int i=0; i<zeros; i++) {
      resultString = resultString + "0";
    } 
  }
  resultString = resultString + String(decimal,BIN);
  return resultString;
}

// Function that checks if both buttons are being pressed, it returns true if 
// .  both buttons are pressed and adjusts for a button being pressed first:
bool bothButtonsPressed(int button1State, int button2State, int button1Pressed, int button2Pressed){
    if ((button1State == HIGH && button2State == HIGH) && (button1Pressed || button2Pressed)) {
        return true;
    } else {
      return false;
    }
}

// Function that checks if button 1 or 2 is being pressed, it returns it's corresponding button number:
int buttonPressed(int button1Pressed, int button2Pressed) {
    if (button1Pressed) {
        return 1;
    }
    if (button2Pressed) {
        return 2;
    }
}

void setup() {
    Serial.begin(9600);

    // Set the buttons to take input:
    pinMode(button1,INPUT);
    pinMode(button2,INPUT);
  
    // Sets up debouncers on thebuttons for 5ms each:
    debouncer1.attach(button1);
    debouncer1.interval(5);
    debouncer2.attach(button2);
    debouncer2.interval(5);
  
    // Sets up the display with the size and contrast:
    lcd.begin(columns,rows);
    pinMode(V0, OUTPUT);
    analogWrite(V0, contrast);
  
    // Display the welcome screen:
    displayScreen(1);
}

void loop() {
    // Updates debouncers
    debouncer1.update();
    debouncer2.update();
  
    // Variables to capture debouncer updates
    int button1State = debouncer1.read();
    int button2State = debouncer2.read();
  
    // Variables to see if buttons went from LOW –> HIGH
    int button1Pressed = debouncer1.rose();
    int button2Pressed = debouncer2.rose();
    
    switch (currentScreen){
      // Welcome Screen:
      case 1:
        // Need to press both buttons to leave welcome screen and start the calculator:
        if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            displayScreen(2);
            currentScreen = 2;
            break;
        }
        break;
        
      // 1st Input Screen:
      case 2:
        // Can access brighntess screen from this screen
        if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            displayScreen(5);
            lastScreen = currentScreen;
            currentScreen = 5;
            break;

        // Flips the bit thats hovering overtop the cursor in the 1st byte array:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            firstByte[currentLoc] = !firstByte[currentLoc];
            displayArray(firstByte);
            lcd.setCursor(currentLoc,2);

        // Moves cursor to the right 1 bit:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            currentLoc++;
            lcd.setCursor(currentLoc,2);

            // If you move the cursor beyond the 8th bit, move to 2nd input screen:
            if (currentLoc > 7) {
                currentLoc = 0;
                displayScreen(3);
                currentScreen = 3;
            } 
        }
        break;
        
      // 2nd Input Screen:
      case 3:
        // Can access brighntess screen from this screen:
        if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            displayScreen(5);
            lastScreen = currentScreen;
            currentScreen = 5;
            break;
            
        // Flips the bit thats hovering overtop the cursor in the 2nd byte array:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            secondByte[currentLoc] = !secondByte[currentLoc];
            displayArray(secondByte);
            lcd.setCursor(currentLoc,2);

        // Moves cursor to the right 1 bit:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            currentLoc++;
            lcd.setCursor(currentLoc,2);

            // If you move the cursor beyond the 8th bit, move to result screen:
            if (currentLoc > 7) {
                currentLoc = 0;
                displayScreen(4);
                currentScreen = 4;
            }
        }
        break;
        
      // Result Screen:
      case 4:
        // Returns you to the 1st input screen:
        if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            resetBytes();
            displayScreen(2);
            currentScreen = 2;
            break;
        }     
        break;
        
       // Contrast Screen:
       case 5:
         // Returns you to previous screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentLoc = 0;
            displayScreen(lastScreen);
            currentScreen = lastScreen;
            break;
            
         // Lower contrast with 1st button:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            displayContrast();
            partialClear(0,2);
            if (contrast == minimum) {
                lcd.print("Minimum");
            } else {
                contrast -= rate;
                analogWrite(V0, contrast);
                lcd.print("-");
                lcd.print(rate);
                displayContrast();
            }
              
         // Raise contrast with 2nd button:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            displayContrast();
            partialClear(0,2);
            if (contrast == maximum) {
                lcd.print("Maximum");
            } else {
                // Display the current contrast on the top row:
                contrast += rate;
                analogWrite(V0, contrast);
                lcd.print("+");
                lcd.print(rate);
                displayContrast();
            }
         }
         break;
    }
}


