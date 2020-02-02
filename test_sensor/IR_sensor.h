//#pragma once  // don't use this
#ifndef IR_sensor_h
#define IR_sensor_h

// to get byte definition, etc:
#include <Arduino.h>

/* calibration data for the GP2Y0A21YKOF IR sensor: 
   white ping-pong ball on slide, distance from sensor to front of ping-pong ball in mm
   analogRead() values in MEANS array below found in fit_sensor.ipynb (before tweaking)
*/
// can tweak the 9 values below to get more accurate distances: did this by hand 
// (original calibration commented out)
// raw analogRead() values
//const float MEANS[] = {465.3, 290.0, 218.7, 194.3, 186.3, 179.0, 170.0, 159.3, 146.7}; 
// TWEAKED values
const float MEANS[] =   {479.0, 375.0, 306.0, 259.0, 220.0, 196.0, 183.0, 172.0, 172.0}; 
// dist in mm
const float DIST[]  =   {100.0, 150.0, 200.0, 250.0, 300.0, 350.0, 400.0, 450.0, 500.0};
// NOTE with sensor/slide configuration on 02/01/20, after 450mm the analogRead value 
//      doesn't change
const int N = 9; // number of calibration values
const int START = 172; //147;  // lowest  analogRead() value for calibration
const int STOP  = 479; //465;  // highest analogRead() value for calibration
//unsigned int LUT[308]; // LUT size is STOP - START + 1  // had to take this out of here, put in .cpp
// END calibration data

class DistSensor {
  
public:
  enum sensors {
    GP2Y0A21YK0F_5V,
    GP2Y0A60SZLF_5V
  };
  /* 'sensor' is the particular IR sensor
   *  'pin' is the Arduino analogRead pin 
  */
  DistSensor(const sensors sensor, const byte pin);

  // 'number' is number of samples to average
  unsigned int getDist(int number = 1);

private:
  byte _pin;
  sensors _sensor;
  void create_LUT();
};

#endif