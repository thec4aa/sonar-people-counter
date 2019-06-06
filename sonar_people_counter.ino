/*
  Sonar People Counter with LCD Feedback
  v.0.2
  2018-07-10 4:07 PM

  GOAL: get all this code working together
  
  TODO:
 * 
 * 
 * 
  
  LiquidCrystal Using Sparkfun LCD Button shield
  https://www.sparkfun.com/products/13293
  Code via http://hackerspacetech.com/lcd-button-shield-v2-for-arduino-by-sparkfun.html
  NOTE:   This uses different hook up than what the examples in the arduino library use.  see The circuit:
 * LCD RS pin to digital pin 12  (8 for shield)
 * LCD Enable pin to digital pin 11 (9 for shield)
 * LCD D4 pin to digital pin 5   (D4)
 * LCD D5 pin to digital pin 4   (D5 for shield)
 * LCD D6 pin to digital pin 3   (D6 for shield)
 * LCD D7 pin to digital pin 2   (D7 for shield)
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 * 
 */

// include the LCD library:
#include <LiquidCrystal.h>
// include the NewPing sonar library:
#include <NewPing.h>

/////////////////
// SONAR STUFF //
/////////////////

// Sensor Yes: trigger pin, echo pin, maximum distance in cm
NewPing sonarYes(15, 16, 400); // 120cm = ~48in

// Ping frequency (in milliseconds), fastest we should ping is about 35ms per sensor
#define pingSpeed 125 
// onboard LED for feedback
#define LED_PIN 13

// stores when the next pingTimer will run
unsigned long pingTimer1; //, pingTimer2, pingTimer3; 

// Global Variables
int triggerDistance = 30; // This is the short distance, in inches
int IN1 = 0; // Global inches variable 1

int peopleCount = 0;
boolean walkpastSwitch = false;
boolean pwalkpastSwitch = true;
boolean leftButton = false;

//GPIO declarations
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//byte segmentClock = 9; // green
//byte segmentLatch = 8; // blue
//byte segmentData = 10; // yellow

byte segmentClock = A4; // green
byte segmentLatch = A3; // blue
byte segmentData = A5; // yellow
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// initialize the library with the numbers of the interface pins
// for the sparkfun shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup()
{
//    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Open serial monitor at 9600 baud to see ping results
    Serial.begin(9600);  //for debugging
    Serial.println("People Counter - setup begins!");
    lcd.print("BING BONG");

    // Sensor 1 fires after 100ms (or whatever pingSpeed is)
    pingTimer1 = millis() + pingSpeed;

    // set up pins an ardunio
    pinMode(LED_PIN, OUTPUT);
    pinMode(segmentClock, OUTPUT);
    pinMode(segmentData, OUTPUT);
    pinMode(segmentLatch, OUTPUT);
// not needed?    
//    pinMode(A1, OUTPUT); 
//    pinMode(A2, INPUT);

    //start with everything low
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, LOW);
    digitalWrite(segmentLatch, LOW);
  
    // start at 00
    postNumber('00', false);

  Serial.println("People Counter - setup complete");
}



// array setup for button feedback
char* buttons[] = {"Left", "Up", "Down", "Right", "Select", "Undefined"};

#define LEFT_BUTTON     0
#define UP_BUTTON       1
#define DN_BUTTON       2
#define RIGHT_BUTTON    3
#define SEL_BUTTON      4
#define UNDEFINED       5

int buttonindex;


//
// SHOW NUMBER FUNCTION
//
//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showNumber(float value)
{
  int number = abs(value); //Remove negative signs and any decimals

  //Serial.print("number: ");
  //Serial.println(number);

  for (byte x = 0 ; x < 2 ; x++)
  {
    int remainder = number % 10;

    postNumber(remainder, false);

    number = number / 10;
  }

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}






//
// POST NUMBER FUNCTION
//
//Given a number, or '-', shifts it out to the display
void postNumber(byte number, boolean decimal)
{
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

#define a  1<<0
#define b  1<<6
#define c  1<<5
#define d  1<<4
#define e  1<<3
#define f  1<<1
#define g  1<<2
#define dp 1<<7

  byte segments;

  switch (number)
  {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }

  if (decimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, segments & 1 << (7 - x));
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}

void leftButtonCheck() {

    int buttonValue = 1023;
    buttonValue = analogRead(A0);
      if (buttonValue > 845 && buttonValue < 865)  // left button
    {
      leftButton = !leftButton;  
      Serial.print("BUTTON STATE CHANGE: ");  
      Serial.println(leftButton);
    } 
}

void loop()
{
  // fire the ping
  if (millis() >= pingTimer1) {
      pingTimer1 += pingSpeed; // Make sensor 1 fire again 100ms later (pingSpeed)
      int in1 = sonarYes.ping_in();
      if (in1 != 0) { // throw out 0 readings
        IN1 = in1;
      }
    }

//  Serial.print("Ping1: ");
//  Serial.print(IN1);
//  Serial.println("in");
    lcd.clear(); // clear the screen
    lcd.home(); // move cursor home
    lcd.print("Dist: ");
    lcd.print(IN1);
    lcd.print("in");
 
//  // LCD BUTTON STUFF
////
//    int buttonValue = 1023;
//    buttonValue = analogRead(A0);
//    lcd.clear();
//    lcd.print("DOORWAY");
//
//    while (buttonValue > 1015) 
//    {
//        buttonValue = analogRead(A0);
//        Serial.println(buttonValue);
//    }
//

      leftButtonCheck();

      if(leftButton) {
      peopleCount = peopleCount - 1;
  } 
      
//        buttonindex = LEFT_BUTTON;
//    }
//    else if (buttonValue > 915 && buttonValue < 949)  // UP button
//    {
//        buttonindex = UP_BUTTON;
//    }
//    else if (buttonValue > 895 && buttonValue < 910) // down button
//    {
//        buttonindex = DN_BUTTON;
//    }
//    else if (buttonValue > 810 && buttonValue < 820) // right button
//    {
//        buttonindex = RIGHT_BUTTON;
//    }
//    else if (buttonValue > 605 && buttonValue < 620) // select button
//    {
//        buttonindex = SEL_BUTTON;
//    }
//    else buttonindex = UNDEFINED;
//    lcd.clear();
//    lcd.print(buttons[buttonindex]);
//    lcd.print("  pressed");
//    // wait for key to be releasted
//
//    while (buttonValue < 1000) 
//    {
//        buttonValue = analogRead(A0);
//    };  // sit in this loop till key unpressed

  // test the distance of the object
  
  if (IN1 < triggerDistance && IN1 > 1){
      /* If the sonar is triggered 
       - light up the LED
       - true - someone is walking past       
     */
       digitalWrite(LED_PIN, HIGH); 
       walkpastSwitch = true;
  } else {
      /* If the sonar is triggered
       *  turn off the LED
       *  false - someone is walking past
       */
       digitalWrite(LED_PIN, LOW); 
       walkpastSwitch = false;
  }

  // if someone wasn't walking past, and now someone is...
  if(pwalkpastSwitch == false && walkpastSwitch == true){
    peopleCount = peopleCount + 1;
  }

  // show the peopleCount
  showNumber(peopleCount); //Don't show decimal

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK

  // save the walkpastSwitch state to pwalkpastSwitch
  pwalkpastSwitch = walkpastSwitch;


}
