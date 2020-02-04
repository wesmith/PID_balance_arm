#ifndef IR_sensor_h
#define IR_sensor_h

#include <Arduino.h>

class DistSensor {
  
public:

  enum sensors {
    GP2Y0A21YK0F_5V,
    GP2Y0A60SZLF_5V
  };
  
  // 'sensor' is the particular IR sensor
  // 'pin' is the Arduino analogRead pin 
  DistSensor(const sensors sensor, const byte pin);

  // 'number' is number of samples to average to a single distance value
  void get_dist(unsigned int &analog, unsigned int &dist, int number = 1);

  void print_LUT();
  
private:

  byte         _pin;
  int          _lo_meas;
  int          _hi_meas;
  int          _num;
  float        _meas[9];  // make larger than 9 in future to be more general
  float        _dist[9];
  unsigned int _LUT[297]; // for now LUT size is _hi_meas - _lo_meas + 1 
  
  void create_LUT();
  void set_model_coeffs(const sensors sensor);
  void add_coeffs(const int num, float* meas, float* dist,
                           const int low_meas, const int high_meas);
};

#endif
