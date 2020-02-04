/*
sketch_8_4 from 'Arduino Cookbook'

WSmith 01/23/20, run servo from serial port, set empirical limits

Running the Tower Pro SG-5010 servo
*/

#include <Servo.h>

Servo myservo;

int MINANG = 0; 
int MAXANG = 172; // by experiment: anything higher causes chatter
int INIT   = 90;  // 90 degrees
int angle; 
int val = 0;
char txt[100];

void setup()
{
 Serial.begin(9600);
 myservo.attach(9); 
 delay(100);
 myservo.write(INIT);
 delay(100);
 
 /*
 - spec sheet shows 600 us to 2400 us pulse widths
 - default values (which are?) gives range about 95-98% of desired range
 - you don't get the full 180 deg range, max looks like up to 170 deg or so
 - 600, 2400 gives range that looks like default range
 - 1000, 2000 gives 50% of desired range
 */
}

void loop()
{
  if ( Serial.available() )
  {
    char ch = Serial.read();
    
    if ( ch >= '0' && ch <= '9' ) // is ch a number?
      val = val * 10 + ch - '0'; 
      
    else if (ch == 10) // end of line
    {
      angle = val;
      if (angle > MAXANG) {angle = MAXANG;}
      if (angle < MINANG) {angle = MINANG;}
      myservo.write( angle );
      sprintf(txt, "Desired angle %i, Thresholded input %i", val, angle);
      Serial.println( txt );
      val = 0;
    }
  } 
}
