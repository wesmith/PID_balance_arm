#include "IR_sensor.h"

DistSensor::DistSensor(const sensors sensor, const byte pin) {
  _pin    = pin;
  set_model_coeffs(sensor);
  create_LUT();
}

void DistSensor::set_model_coeffs(const sensors sensor) {

  switch (sensor) {

    case GP2Y0A21YK0F_5V: {
      /*calibration data for the GP2Y0A21YKOF IR sensor: 
       A white ping-pong ball was moved along a V slide, with the IR sensor at one end.
       The distance in mm was measured from the front of the ball to the IR sensor, as the
       ball was moved along the slide. Average analogRead() values from the sensor were found
       at each of 9 positions of the ball (in the dist[] array) and entered into the meas[] array. 
       Original analogRead values were found with calibrate_sensor.ino and analyzed with 
       fit_sensor.ipynb. A piecewise linear fit was found to be much better than a polynomial
       fit. With test_sensor.ino, it is possible to tweak the meas[] values below by hand to improve 
       distance performance in a straightforward way: for a given distance, test_sensor.ino outputs
       the estimated distance and the average analogRead value for that distance. This analogRead
       value can be substituted for the appropriate distance in the meas[] array below. 
       In fact, test_sensor.ino can be used for the entire calibration process if reasonable 
       starting values are used to initialize the meas[] array. 
      */
      float meas[] =   {478.0, 379.0, 310.0, 259.0, 222.0, 195.0, 192.0, 184.0, 183.0}; 
      float dist[] =   {100.0, 150.0, 200.0, 250.0, 300.0, 350.0, 370.0, 390.0, 400.0};
      add_coeffs(9, meas, dist, 183.0, 478.0);
      break;
    }
    case GP2Y0A60SZLF_5V: {
      // not yet implemented
      break;
    }
  }
}

void DistSensor::add_coeffs(const int num, float* meas, float* dist,
                           const int low_meas, const int high_meas) {
  _num     = num;
  _lo_meas = low_meas;
  _hi_meas = high_meas;
  for (byte i = 0; i < num; i++) {
    _meas[i] = meas[i];
    _dist[i] = dist[i];
  }
  
}

void DistSensor::create_LUT() {

  for (int i = _lo_meas; i <= _hi_meas; i++) {
    for (int j = 0; j < (_num-1); j++) {
      if ((i <= _meas[j]) && (i >= _meas[j+1])) {
        _LUT[i-_lo_meas] = (int)((i - _meas[j]) * (_dist[j+1] - _dist[j]) / (_meas[j+1] - _meas[j]) 
                            + _dist[j]); 
        break;
      }
    }
  }
}

void DistSensor::print_LUT() {
  for (int i = _lo_meas; i <= _hi_meas; i++) {
    Serial.print("i = ");
    Serial.print(i);
    Serial.print("  LUT(i - lowest) = ");
    Serial.println(_LUT[i - _lo_meas]);
  }
}

void DistSensor::get_dist(unsigned int &analog, unsigned int &dist, int number = 1) {
  analog = 0;
  for (int i = 1; i <= number; i++) {
    unsigned int val = analogRead(_pin);
    analog += val;  
  }
  analog /= number;
  int indx = analog - _lo_meas;  // this can go negative, so keep signed int
  int maxIndx = _hi_meas - _lo_meas;
  if (indx < 0) {indx = 0;}
  if (indx > maxIndx) {indx = maxIndx;}
  dist = _LUT[indx];
}
