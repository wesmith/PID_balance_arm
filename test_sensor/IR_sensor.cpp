#include "IR_sensor.h"

unsigned int LUT[297]; // LUT size is STOP - START + 1  // had to put this here: didn't compile in .h

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
                            + DIST[j]); 
        break;
      }
    }
  }
  LUT[STOP-START] = DIST[0]; // outlier missed in above loop
}

void DistSensor::print_LUT() {
  for (int i = START; i <= STOP; i++) {
    Serial.print("i = ");
    Serial.print(i);
    Serial.print("  LUT(i - START) = ");
    Serial.println(LUT[i - START]);
  }
}

void DistSensor::get_dist(unsigned int &analog, unsigned int &dist, int number = 1) {
  analog = 0;
  for (int i = 1; i <= number; i++) {
    unsigned int val = analogRead(_pin);
    analog += val;  
  }
  analog /= number;
  int indx = analog - START;  // this can go negative, so keep signed int
  int maxIndx = STOP - START;
  if (indx < 0) {indx = 0;}
  if (indx > maxIndx) {indx = maxIndx;}
  dist = LUT[indx];
}
