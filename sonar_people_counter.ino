/* Sonar People Counter
 *  v.0.2
 *  2018-07-08 7:22 PM
 *  
 *  Get sonar triggering our digits
 */

/////////////////
// SONAR STUFF //
/////////////////
#include <NewPing.h>

// Sensor Yes: trigger pin, echo pin, maximum distance in cm
NewPing sonarYes(2, 3, 400);

// Sensor 2: trigger pin, echo pin, maximum distance in cm
//NewPing sonarNo(4, 5, 400); 

// Ping frequency (in milliseconds), fastest we should ping is about 35ms per sensor
#define pingSpeed 125 
// onboard LED for feedback
#define LED_PIN 13
unsigned long pingTimer1, pingTimer2, pingTimer3; 

// Global Variables
int triggerDistance = 30; // This is the short distance, in inches
int IN1 = 0; // Global inches variable 1
//int IN2 = 0; // Global inches variable 2
//int IN3 = 0; // Global inches variable 3

int peopleCount = 0;
boolean walkpastSwitch = false;
boolean pwalkpastSwitch = true;

//GPIO declarations
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
byte segmentClock = 6;
byte segmentLatch = 5;
byte segmentData = 7;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup() {
  // put your setup code here, to run once:
  
  // Open serial monitor at 115200 baud to see ping results.
  Serial.begin(115200);
  Serial.println("People Counter - setup begins!");

  // Sensor 1 fires after 100ms (or whatever pingSpeed is)
  pingTimer1 = millis() + pingSpeed;

  // set up pins an ardunio
  pinMode(LED_PIN, OUTPUT);
  pinMode(segmentClock, OUTPUT);
  pinMode(segmentData, OUTPUT);
  pinMode(segmentLatch, OUTPUT);

  //start with everything low
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, LOW);
    digitalWrite(segmentLatch, LOW);
  
  // start at 00
    postNumber('00', false);

  Serial.println("People Counter - setup complete");
}



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



void loop() {
  // put your main code here, to run repeatedly:


  if (millis() >= pingTimer1) {
      pingTimer1 += pingSpeed; // Make sensor 1 fire again 100ms later (pingSpeed)
      int in1 = sonarYes.ping_in();
      IN1 = in1;
    }

  Serial.print("Ping1: ");
  Serial.print(IN1);
  Serial.println("in");

  if (IN1 < triggerDistance){
      /* If the sonar is triggered 
       - light up the LED
       
     */
    // turn on the LED!
   digitalWrite(LED_PIN, HIGH); 
   walkpastSwitch = true;
  } else {
    // shut off the LED
   digitalWrite(LED_PIN, LOW); 
   walkpastSwitch = false;
  }


  if(pwalkpastSwitch == false && walkpastSwitch == true){
    peopleCount = peopleCount + 1;
  }

  
  showNumber(peopleCount); //Don't show decimal

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK


  pwalkpastSwitch = walkpastSwitch;

} // end loop
