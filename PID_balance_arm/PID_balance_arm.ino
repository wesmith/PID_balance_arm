/*PID_balance_arm.ino
 * 
 * WESmith 02/05/20
 * 
 * balance a ping-pong ball along a balance arm using PID feedback 
 * 
 * original code by http://electronoobs.com/eng_arduino_tut100_code1.php , 
 * modified for experimentation
 * 
 * hardware: Arduino UNO, GP2Y0A21YK0F_5V IR distance sensor (connected to pin A0), 
 *           and the Tower Pro SG-5010 servo (connected to pin 9)
 * 
 * NOTE: Before loading this program into the Arduino, remove the lever arm from the servo
 *       to prevent spurious out-of-bound motion of the servo during program startup 
 *       from damaging/stressing the setup. After the program has initialized, put the lever 
 *       arm on the servo in the horizontal position, which corresponds to the beginning 
 *       servo_ang of 90 deg. 
 */

#include "IR_sensor.h" 
#include <Servo.h>

int NMEAS             =  100; // 60  number of distance measurements to average for 1 point
const int MINANG      =  65; // in deg
const int MAXANG      = 115; 

const byte sensor_pin =  A0;
const byte servo_pin  =   9;


float servo_ang = 90; 
int   baseline  = 90; // baseline angle of slide

unsigned int analog; // not used in this sketch, but required for get_dist()
unsigned int distance;

DistSensor sensor(DistSensor::GP2Y0A21YK0F_5V, sensor_pin);
Servo servo;

////////////////////////Variables///////////////////////
float tt;
float prev_error = 0;
float error      = 0;
int   period     = 20;  // refresh rate of the loop in ms
///////////////////////////////////////////////////////

///////////////////PID constants///////////////////////
float alpha = 0.9; 
float kp = 2.0; //
float ki = 20.0; //
float kd = 600.0; //
float setpoint = 275;  // desired placement of the ball in mm
float PID_p, PID_i, PID_d, PID_total;
int   PID_i_bound = 10;
///////////////////////////////////////////////////////


void waitForSerial() {
  while(Serial.available()){Serial.read();}
  while (!Serial.available()) { }
  //Serial.println(Serial.read());
  //Serial.println("");
}

void setup() {
  Serial.begin(9600);
  servo.attach(servo_pin);
  delay(100);
  servo.write(baseline);
  pinMode(sensor_pin, INPUT);
  //sensor.print_LUT();  // diagnostic

  //Serial.println("");
  //Serial.println("In setup(): CR to continue");
  //waitForSerial();
  Serial.println("loop has begun");

  tt = millis();
}

void loop() {
  
  if (millis() > tt + period) {
    
    tt = millis();    
    sensor.get_dist(analog, distance, NMEAS);  // analog not used here
    error = setpoint - distance;
    
    PID_p = kp * error;
    
    float diff = error - prev_error;     
    PID_d = kd * (diff/(float)period);
      
    if((error > -PID_i_bound) && (error < PID_i_bound))
    {
      PID_i = PID_i + (ki * error);
    }
    else
    {
      PID_i = 0;
    }
  
    PID_total = PID_p + PID_i + PID_d;  
    servo_ang = alpha * servo_ang + (1.0 - alpha) * PID_total; 
    //servo_ang = baseline + PID_total; ;
  
    if(servo_ang < MINANG) {servo_ang = MINANG;}
    if(servo_ang > MAXANG) {servo_ang = MAXANG;} 

    /*
    Serial.print("error: ");
    Serial.print(error);
    Serial.print("  servo_ang: ");
    Serial.println(servo_ang);
    */

    servo.write((int)servo_ang);  
    prev_error = error;
  }
}
