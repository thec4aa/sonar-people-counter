/*
  Sonar People Counter with LCD Feedback
  v.0.6
  July 13, 2018 1:55 PM
  
  DONE:
 * consolidate and cleaned up code
 *    * moved things into functions
 * incorporate led strip flashing
 * displays 3 digits!
 * 4:55 PM
  
  
 * Pin assignments: 
 * 
 * D2    Sonar Trigger 
 * D3    Sonar Echo
 * D4-10 connected to LCD
 * D11   PWM for LED Strip
 * D12   pause button
 * D13   LED feedback
 * A3    Digits Latch (blue)
 * A4    Digits Clock (Green)
 * A5    Digits Data  (Yellow)
 * 
 */

// LIBRARIES
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// include LCD library
// LiquidCrystal Using Sparkfun LCD Button shield
// https://www.sparkfun.com/products/13293
#include <LiquidCrystal.h>
// include the NewPing sonar library:
#include <NewPing.h>

// SONAR STUFF 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define TRIGGER_PIN  2
#define ECHO_PIN     3
#define MAX_DISTANCE 400
// Ping frequency (in milliseconds), fastest we should ping is about 35ms per sensor
#define pingSpeed 125 

// Sensor Yes: trigger pin, echo pin, maximum distance in cm
NewPing sonarYes(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);// 120cm = ~48in
// stores when the next pingTimer will run
unsigned long pingTimer1; 

// Global variables
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define LED_PIN 13     // onboard LED for feedback
#define LED_STRIP_PIN 11     // onboard LED for feedback
#define PAUSE_PIN 12     // our Pause Button
int triggerDistance = 30;     // This is the short distance, in inches
int IN1 = 0;     // Global inches variable for sonar
int peopleCount = 0;     // count of people who have walked by
boolean walkpastSwitch = false;     // is someone walking by?
boolean pwalkpastSwitch = true;     // was someone just walking by?
#define FADESPEED 1     // LED Strip make this higher to slow down
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


// LCD BUTTON variables
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// array setup for button feedback
char* buttons[] = {"Left", "Up", "Down", "Right", "Select", "Undefined"};

#define LEFT_BUTTON     0
#define UP_BUTTON       1
#define DN_BUTTON       2
#define RIGHT_BUTTON    3
#define SEL_BUTTON      4
#define UNDEFINED       5

boolean downButton = false; // is the left button pressed?
boolean pauseButton = true; // is pause button NOT pressed?
int buttonindex; // what button is being pressed?
int pbuttonindex = 5; // what button just pressed?

// initialize the library with the numbers of the interface pins
// for the sparkfun LCD shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


// GPIO declarations for Large Digits
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
byte segmentClock = A4; // green
byte segmentLatch = A3; // blue
byte segmentData = A5; // yellow
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=





// SETUP
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
    pinMode(PAUSE_PIN, INPUT_PULLUP);
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
    showNumber(peopleCount);

  Serial.println("People Counter - setup complete");
}
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=





// FUNCTION: Large-Digit - Show Number
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showNumber(float value)
{
  int number = abs(value); //Remove negative signs and any decimals

  //Serial.print("number: ");
  //Serial.println(number);

  for (byte x = 0 ; x < 3 ; x++)
  {
    int remainder = number % 10;

    postNumber(remainder, false);

    number = number / 10;
  }

  // Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=





// FUNCTION: Large-Digit - Post Number
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void postNumber(byte number, boolean decimal){
  //Given a number, or '-', shifts it out to the display
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
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


// FUNCTION: Reads buttons and determines what's being pressed
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void buttonCheck() {

    int buttonValue = 1023;
    buttonValue = analogRead(A0);

    if (buttonValue > 845 && buttonValue < 865)  // left button
    {
        buttonindex = LEFT_BUTTON;
    }
    else if (buttonValue > 915 && buttonValue < 949)  // UP button
    {
        buttonindex = UP_BUTTON;
    }
    else if (buttonValue > 895 && buttonValue < 910) // down button
    {
        buttonindex = DN_BUTTON;
    }
    else if (buttonValue > 810 && buttonValue < 820) // right button
    {
        buttonindex = RIGHT_BUTTON;
    }
    else if (buttonValue > 605 && buttonValue < 620) // select button
    {
        buttonindex = SEL_BUTTON;
    }
    else { 
      buttonindex = UNDEFINED;
    }

    
    
//    lcd.clear();
//    lcd.print(buttons[buttonindex]);
//    lcd.print("  pressed");

}
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Function: fire the ping and listen for echo
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void pingMachine(){
   // fire the ping
  if (millis() >= pingTimer1) {
      pingTimer1 += pingSpeed; // Make sensor 1 fire again 100ms later (pingSpeed)
      int in1 = sonarYes.ping_in();
      if (in1 != 0) { // throw out 0 readings
        IN1 = in1;
      }
    }
}
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=



// Function: Check Pause Button and run ping
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void pauseCheck(){
    // are we paused?
  pauseButton = digitalRead(PAUSE_PIN);
  
  if (pauseButton == false){
    lcd.setCursor(0, 2);
    lcd.print("PAUSED ");
    pauseButton = digitalRead(PAUSE_PIN); 
    
  } else {
    pingMachine(); // run the Sonar Ping function
    lcd.setCursor(0, 2);
    lcd.print("ACTIVE ");
  }

//  Serial.print("Ping1: ");
//  Serial.print(IN1);
//  Serial.println("in");
    // lcd.clear(); // clear the screen
}



//
// Function: Flash LEDStrip
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void flashLEDStrip(){

  int r;

  for (int x = 0; x < 10; x++) {
    // fade from dark to light
    for (r = 0; r < 256; r++) { 
      analogWrite(LED_STRIP_PIN, r);
      delay(FADESPEED);
    }
    // fade from light to dark
    for (r = 255; r >= 0; r--) { 
      analogWrite(LED_STRIP_PIN, r);
      delay(FADESPEED);
    }
    analogWrite(LED_STRIP_PIN, 255);
    delay(FADESPEED * 25);
    analogWrite(LED_STRIP_PIN, 0);
    delay(FADESPEED * 25);
  }

      

//        for (int x = 0; x < 100; x++) {
//        analogWrite(LED_STRIP_PIN, 255);
//        delay(FADESPEED * 25);
//        analogWrite(LED_STRIP_PIN, 0);
//        delay(FADESPEED * 25);
//        }
        
         
}
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//
// BRING ON THE LOOP
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void loop(){
  
  
    pauseCheck(); // runs the sonar ping
    buttonCheck(); 

    
    lcd.home(); // move cursor home
    lcd.print("Dist:");
    lcd.print(IN1);
    lcd.print("in  ");
    lcd.setCursor(12,0); 
    lcd.print("[");
    lcd.print(triggerDistance);
    lcd.print("]");
  



  // below checks if someone is walking by
  // (tests the distance of the object in front of sonar)
  
  if (IN1 < triggerDistance && IN1 > 1){
      /* If the sonar is triggered 
       - true - someone is walking past 
       - light up the LED    
     */
     walkpastSwitch = true;
    
     // if someone wasn't walking past before, and now someone is...
    if(pwalkpastSwitch == false && walkpastSwitch == true){
      peopleCount = peopleCount + 1; // add a person
    }
    // send the peopleCount on the large digits
    showNumber(peopleCount); 
      // Latch the current segment data
//      digitalWrite(segmentLatch, LOW);
//      digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK

     flashLEDStrip();     

  } else {
      /* If the sonar is triggered
       *  turn off the LED
       *  false - someone is walking past
       */
       digitalWrite(LED_PIN, LOW); 
       walkpastSwitch = false;
  }


  // BUTTON ADJUSTMENTS
  // UP = more people
  if (pbuttonindex == UP_BUTTON && buttonindex == UNDEFINED){
    peopleCount = peopleCount + 1;
  }

  // DOWN = less people
  if (pbuttonindex == DN_BUTTON && buttonindex == UNDEFINED){
    peopleCount = peopleCount - 1;
  }

  // LEFT = less trigger distance
  if (pbuttonindex == LEFT_BUTTON && buttonindex == UNDEFINED){
    triggerDistance = triggerDistance - 1;
  }

  // RIGHT = more trigger distance
  if (pbuttonindex == RIGHT_BUTTON && buttonindex == UNDEFINED){
    triggerDistance = triggerDistance + 1;
  }
  
  // save the current state to previous state variables
  pwalkpastSwitch = walkpastSwitch;
  pbuttonindex = buttonindex;

  // send the peopleCount on the large digits
  // in case anything has changed (like the numbers)
  showNumber(peopleCount); 

//  Troubleshooting area...
//  Serial.print("downButton: ");
//  Serial.println(downButton);
//  Serial.print("triggerDistance: ");
//  Serial.println(triggerDistance);

}
