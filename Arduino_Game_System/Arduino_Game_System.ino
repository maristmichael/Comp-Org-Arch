#include <Bounce2.h>
#include <LiquidCrystal.h>

// Constant variables for LCD pins:
const int D7 = 13;
const int D6 = 12;
const int D5 = 11;
const int D4 = 10;
const int EN = 8;
const int RS = 9;   
const int V0 = 6;

// Constant variables for display text size:
const int columns = 16;
const int rows = 2;

// Variable for current contrast:
int contrast = 0;

// Constant variable for contrast changing:
int minimum = 0;
int maximum = 60;
int rate = 5;

// Constant variables for button pins:
const int button1 = 2;
const int button2 = 3;


// Variable that tracks what the current screen is:
// 1 = Home, 2 = Game Choice 1, 3 = Game Choice 2, 4 = Coming Soon, 5 = Contrast Prompt, 6 = Contrast Adjustment
int currentScreen = 1;
int lastScreen = 1;

// Instantiate the display: 
LiquidCrystal lcd(RS,EN,D4, D5, D6, D7);

// Instantiate the debouncers for each button: 
Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();

// Variables for custom LCD characters
byte leftArrowByte[] = {
  B00010,
  B00100,
  B01000,
  B11111,
  B11111,
  B01000,
  B00100,
  B00010
};

byte rightArrowByte[] = {
  B01000,
  B00100,
  B00010,
  B11111,
  B11111,
  B00010,
  B00100,
  B01000
};

const int leftArrow = 0;
const int rightArrow = 1;

// Variable for scoring
int score = 0;
int highScore = 0;

// Variable for attempts remaining
int attempts = 3;

// Variable for current player
int currentPlayer = 1;

// Arrays to hold Friend Fued input for Player1 and 2:
int player1[] = {0,0,0,0};
int player2[] = {0,0,0,0};

// Variable to keep tracks of players' array postion
int arrayPosition = 0;

// Variables for random number generator
int num1;
int num2;

// Array for storing Math Master user input:
int mathByte[8] = {0,0,0,0,0,0,0,0};

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

// Variable to track cursor location when it is active:
int currentLoc = 0;

// Function that resets the 2 arrays for input:
void resetArrays(int size){
  for (int i = 0; i < size; i++){
    player1[i] = 0;
    player2[i] = 0;
    mathByte[i] = 0;
  }
}

// Function used to display the input array:
void displayArray(int Array[]){
  for (int i = 0; i < 8; i++){
    lcd.setCursor(i,2);
    lcd.print(Array[i]);
  }
}

// Function that clears everything to the right of the column/row entered:
void partialClear(int col, int row){
    lcd.setCursor(col,row);
    lcd.write("                ");
    lcd.setCursor(col,row);
}

// Function used to display the contrast number:
void displayContrast() {
    partialClear(0,0);
    lcd.print("Contrast: ");
    lcd.print(contrast);
}

// Function that checks if both buttons are being pressed, it returns true if 
// .  both buttons are pressed and adjusts for a button being pressed first:
boolean bothButtonsPressed(int button1State, int button2State, int button1Pressed, int button2Pressed){

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

// Function that prints text to the display, based on the currentScreen number:
void displayScreen() {
    if (currentScreen == 1) {
        lcd.clear();
        lcd.print("Arduino Game");
        lcd.setCursor(0,2);
        lcd.print("   System");
        lcd.setCursor(15,2);
        lcd.write(byte(rightArrow));
    } else if (currentScreen == 2) {
        currentLoc = 2;
        lcd.clear();
        lcd.print("Pick a game:");
        lcd.setCursor(0,2);
        lcd.print("Friend Fued!");
        lcd.setCursor(15,2);
        lcd.write(byte(rightArrow));
    } else if (currentScreen == 3) {
        partialClear(0,2);
        lcd.write(byte(leftArrow));
        lcd.setCursor(currentLoc,2);
        lcd.print("Math Master!");
        lcd.setCursor(0,2);
        lcd.setCursor(15,2);
        lcd.write(byte(rightArrow));
    } else if (currentScreen == 4) {
        partialClear(0,2);
        lcd.write(byte(leftArrow));
        lcd.setCursor(2,2);
        lcd.print("More to come!");
    } else if (currentScreen == 5) {
        lcd.clear();
        lcd.print("  Adj Contrast");
        lcd.setCursor(0,2);
        lcd.write(byte(leftArrow));
    } else if (currentScreen == 6) {
        lcd.noCursor();
        lcd.clear();
        displayContrast();
        lcd.setCursor(0,2);
        if (contrast == minimum) {
            lcd.print("Minimum");
        } else if (contrast == maximum){
            lcd.print("Maximum");
        }
    } else if (currentScreen == 7) {
        lcd.clear();
        if (currentPlayer==1) {
            lcd.print("Player 1 Set");
            lcd.setCursor(0,2);
            lcd.print("your answers");
        } else {
            lcd.print("Player 2 Guess");
            lcd.setCursor(0,2);
            lcd.print("correct answers");
        }
    } else if (currentScreen == 8) {
        lcd.clear();
        if (currentPlayer==1){
            lcd.print("Have I ever");
            lcd.setCursor(0,2);
            lcd.print("been on a plane?");
        } else {
            lcd.print("Has he/she ever");
            lcd.setCursor(0,2);
            lcd.print("been on a plane?");
        }
    } else if (currentScreen == 9) {
        lcd.clear();
        if (currentPlayer==1){
            lcd.print("Do I have");
            lcd.setCursor(0,2);
            lcd.print("any allergies?");
        } else {
            lcd.print("Does he/she have");
            lcd.setCursor(0,2);
            lcd.print("any allergies?");
        }
    } else if (currentScreen == 10) {
        lcd.clear();
        if (currentPlayer==1){
            lcd.print("Have I ever");
            lcd.setCursor(0,2);
            lcd.print("been camping?");
        } else {
            lcd.print("Has he/she ever");
            lcd.setCursor(0,2);
            lcd.print("been camping?");
        }
    } else if (currentScreen == 11) {
        lcd.clear();
        if (currentPlayer==1){
            lcd.print("Do I have");
            lcd.setCursor(0,2);
            lcd.print("a sibling?");
        } else {
            lcd.print("Does he/she have");
            lcd.setCursor(0,2);
            lcd.print("a sibling?");
        }
    } else if (currentScreen == 12) {
        lcd.clear();
        for(int i=0; i<4; i++) {
          if(player1[i] == player2[i]) {
              score += 1;
          }
        }
        if (score > 3){
          lcd.print("Player 2 is a...");
          lcd.setCursor(0,2);
          lcd.print("GREAT friend!!!");
        } else  if (score > 1){
          lcd.print("Player 2 is an...");
          lcd.setCursor(0,2);
          lcd.print("OKAY friend!!!");
        } else {
          lcd.print("Player 2 is a...");
          lcd.setCursor(0,2);
          lcd.print("BAD friend!!!");
        }
        score = 0;
    } else if (currentScreen == 13) {
        lcd.clear();
        lcd.print("  Math Master!");
        lcd.setCursor(0,2);
        lcd.write(byte(leftArrow));
        lcd.print(" easy");   
    } else if (currentScreen == 14) {
        lcd.clear();
        lcd.print(num1);
        lcd.print("+");
        lcd.print(num2);
        lcd.print(":");
        currentLoc = 0;
        displayArray(mathByte);
        lcd.setCursor(currentLoc,2);
        lcd.cursor(); 
    } else if (currentScreen == 15) {
        lcd.clear();
        lcd.noCursor();
        lcd.print("High Score:");
        lcd.print(highScore);
        lcd.setCursor(0,2);
        lcd.print("Your Score:");
        lcd.print(score);        
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

    // Sets up characters to be displayed:
    lcd.createChar(leftArrow, leftArrowByte);
    lcd.createChar(rightArrow, rightArrowByte);

    // Sets up randomizing numbers:
    randomSeed(analogRead(A0));
  
    // Sets up the display with the size and contrast:
    lcd.begin(columns,rows);
    pinMode(V0, OUTPUT);
    analogWrite(V0, contrast);
  
    // Display the welcome screen:
    displayScreen();
}

void loop() {

    // Updates debouncers:
    debouncer1.update();
    debouncer2.update();
  
    // Variables to capture debouncer updates:
    int button1State = debouncer1.read();
    int button2State = debouncer2.read();
  
    // Variables to see if buttons went from LOW –> HIGH:
    int button1Pressed = debouncer1.rose();
    int button2Pressed = debouncer2.rose();
    
    switch (currentScreen){
      // Home Screen:
      case 1:
        // Enters Game Selection:
        if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 2;
            displayScreen();
            break;
            
        // Button 1 does nothing:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            break;
        // Takes you to Contrast Prompt:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            currentScreen = 5;
            displayScreen();
        }
        break;
        
        
      // Game Choice 1: 'Friend Fued!':
      case 2:
        // Starts the Game:
        if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){ {
            currentScreen = 7;
            displayScreen();
            break;
        }

        // Button 1 does nothing:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
          
        // Takes you to next Game Option:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            currentScreen = 3;
            displayScreen();
        }
        break;
        
      // Game Choice: 'Math Master!':
      case 3:
        // Starts the Game:
        if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
          currentScreen = 13;
          displayScreen();
          break;
            
        // Takes you to previous Game Option:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            currentScreen = 2;
            displayScreen();

        // Takes you to next Game Option:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            currentScreen = 4;
            displayScreen();
        }
        break;
        
      // Game Choice: 'More to Come':
      case 4:
        // Button 1&2 does nothing:
        if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            
        // Takes you to previous Game Option:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            currentScreen = 3;
            displayScreen();

        // Button 2 does nothing:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
        }
        break;
        
       // Contrast Prompt Screen:
       case 5:
         // Takes you to Contrast Adjustment Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 6;
            displayScreen();
            break;
            
         // Returns you to the Home Screen:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            currentScreen = 1;
            displayScreen();
              
         // 2nd Button does nothing:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            
         }
         break;

         // Contrast Adjustment Screen
         case 6:
         // Returns you to the Contrast Prompt Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 5;
            displayScreen();
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

         // Friend Fued Start Screen:
         case 7:
         
         // Returns you to the Home Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 1;
            currentPlayer = 1;
            arrayPosition = 0;
            displayScreen();
            break;
         // Answers True and takes you to Family Fued Question 1:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            currentScreen = 8;
            displayScreen();
            break;
         // Answers False and takes you to Family Fued Question 1:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            currentScreen = 8;
            displayScreen();
            break;
         }
         break;

         // Friend Fued Question 1:
         case 8:
         // Returns you to the Home Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 1;
            currentPlayer = 1;
            arrayPosition = 0;
            displayScreen();
            break; 
         // Answers True and takes you to Family Fued Question 2:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            if (currentPlayer == 1) {
              player1[arrayPosition] = 1;
            } else {
              player2[arrayPosition] = 1;
            }
            arrayPosition++;
            currentScreen = 9;
            displayScreen();
            break;
         // Answers False and takes you to Family Fued Question 2:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            if (currentPlayer == 1) {
              player1[arrayPosition] = 0;
            } else {
              player2[arrayPosition] = 0;
            }
            arrayPosition++;
            currentScreen = 9;
            displayScreen();
            break;
         }
         break;

         // Friend Fued Question 2:
         case 9:
         // Returns you to the Home Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 1;
            currentPlayer = 1;
            arrayPosition = 0;
            displayScreen();
            break;
         // Answers True and takes you to Family Fued Question 3:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            if (currentPlayer == 1) {
              player1[arrayPosition] = 1;
            } else {
              player2[arrayPosition] = 1;
            }
            arrayPosition++;
            currentScreen = 10;
            displayScreen();
            break;
         // Answers False and takes you to Family Fued Question 3:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            if (currentPlayer == 1) {
              player1[arrayPosition] = 0;
            } else {
              player2[arrayPosition] = 0;
            }
            arrayPosition++;
            currentScreen = 10;
            displayScreen();
            break;
         }
         break;

         // Friend Fued Question 3:
         case 10:
         // Returns you to the Home Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 1;
            currentPlayer = 1;
            arrayPosition = 0;
            displayScreen();
            break;
         // Answers True and takes you to Family Fued Question 4:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            if (currentPlayer == 1) {
                player1[arrayPosition] = 1;
            } else {
                player2[arrayPosition] = 1;
            }
            arrayPosition++;
            currentScreen = 11;
            displayScreen();
            break;
         // Answers False and takes you to Family Fued Question 4:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            if (currentPlayer == 1) {
              player1[arrayPosition] = 0;
            } else {
              player2[arrayPosition] = 0;
            }
            arrayPosition++;
            currentScreen = 11;
            displayScreen();
            break;
         }
         break;

         // Friend Fued Question 4:
         case 11:
         // Returns you to the Home Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 1;
            currentPlayer = 1;
            arrayPosition = 0;
            displayScreen();
            
         // Answers True and takes you to either Player 2 input or result screens:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            if (currentPlayer == 1) {
                player1[arrayPosition] = 1;
             } else {
                player2[arrayPosition] = 1;
            }
            if (currentPlayer == 1){
              currentScreen = 7;
              currentPlayer = 2;
              arrayPosition = 0;
              displayScreen();
            } else if (currentPlayer == 2) {
              currentScreen = 12;
              displayScreen();
            }
            break;
         // Answers False and takes you to either Player 2 input or result screen:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            if (currentPlayer == 1) {
              player1[arrayPosition] = 0;
            } else {
              player2[arrayPosition] = 0;
            }
            if (currentPlayer == 1){
              currentScreen = 7;
              currentPlayer = 2;
              arrayPosition = 0;
              displayScreen();
            } else if (currentPlayer == 2) {
              currentScreen = 12;
              displayScreen();
            }
            break;
         }
         break;

         // Friend Fued Results:
         case 12:
         // Returns you to the Home Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 1;
            displayScreen(); 
         // Returns you to Family Fued Start Screen:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            currentScreen = 7;
            displayScreen();
         // Returns you to Family Fued Start Screen:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            currentScreen = 7;
            displayScreen();   
         }
         resetArrays(4);
         currentPlayer = 1;
         arrayPosition = 0;
         break;

         // Math Master Start Screen:
         case 13:
         // Returns you to the Home Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 1;
            displayScreen(); 
         // Returns you to Family Fued Start Screen:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            currentScreen = 14;
            num1 = random(127);
            num2 = random(128);
            displayScreen();
         // Returns you to Family Fued Start Screen:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
              
         }
         break;

         // Math Master Game:
         case 14:
         // Returns you to the Home Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            lcd.noCursor();
            currentScreen = 1;
            displayScreen();
             
         // Flips the bit thats hovering overtop the cursor in the 1st byte array:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            mathByte[currentLoc] = !mathByte[currentLoc];
            displayArray(mathByte);
            lcd.setCursor(currentLoc,2);

        // Moves cursor to the right 1 bit:
        } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            currentLoc++;
            lcd.setCursor(currentLoc,2);

            // If you move the cursor beyond the 8th bit, move to 2nd input screen:
            if (currentLoc > 7) {
                int input = binToInt(mathByte);
                if (input == (num1+num2)) {
                  score +=1;
                } else {
                  attempts--;
                  if (attempts == 0){
                      if(score > highScore) {
                        highScore = score;
                      }
                      currentScreen = 15;
                      displayScreen();
                      break;
                  }
                }
                resetArrays(8);
                num1 = random(128);
                num2 = random(127);
                currentScreen = 14;
                displayScreen();
            } 
        }
        break;

        // Math Master Results:
         case 15:
         // Returns you to the Home Screen:
         if (bothButtonsPressed(button1State, button2State, button1Pressed, button2Pressed) == true){
            currentScreen = 1;
            displayScreen(); 
         // Returns you to Family Fued Start Screen:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 1) {
            currentScreen = 13;
            displayScreen();
         // Returns you to Family Fued Start Screen:
         } else if (buttonPressed(button1Pressed,button2Pressed) == 2) {
            currentScreen = 13;
            displayScreen();   
         }
         resetArrays(8);
         score = 0;
         attempts = 3;
         break;
    }
    
}



