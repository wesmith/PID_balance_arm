#include "IR_sensor.h"

unsigned int LUT[308]; // LUT size is STOP - START + 1  // had to put this here: didn't compile in .h

DistSensor::DistSensor(const sensors sensor, const byte pin) {
  _pin    = pin;
  _sensor = sensor; // for future use
  create_LUT();
}

void DistSensor::create_LUT() {
// LUT incorporating linear interpolation: polynomial fits were problematic with this data
// (not generalized to different sensors yet: 'sensor' not used at present)
  for (int i = START; i <= STOP; i++) {
    for (int j = 0; j < (N-1); j++) {
      if ((i < MEANS[j]) && (i >= MEANS[j+1])) {
        LUT[i-START] = (int)((i - MEANS[j]) * (DIST[j+1] - DIST[j]) / (MEANS[j+1] - MEANS[j]) 
                            + DIST[j] + 1); 
        break;
      }
    }
  }
}

unsigned int DistSensor::getDist(int number = 1) {
  unsigned int analog_mean = 0;
  for (int i = 1; i = number; i++) {
    unsigned int analog = analogRead(_pin);
    analog_mean += analog;  
  }
  analog_mean /= number;
  unsigned int indx = analog_mean - START;
  unsigned int maxIndx = STOP - START;
  if (indx < 0) {indx = 0;}
  if (indx > maxIndx) {indx = maxIndx;}
  return LUT[indx];
}
