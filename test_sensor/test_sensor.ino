/*test_sensor.ino
 * 
 * WESmith 01/29/20
 * 
 * test distance sensors using calibration data obtained from calibrate_sensor.ino
 * NOTE: values can be tweaked in real-time using this code: see modified MEANS array below. 
 */

#include "IR_sensor.h"

const byte sensor_pin = A0;
DistSensor sensor1(DistSensor::GP2Y0A21YK0F_5V, sensor_pin);

const int NBINS = 400; //350;
// histogram array, just out to NBINS (note: have to reduce this if too much global mem used)
int   hist[401] = {0}; //hist[351] = {0};
float hist_mean = 0;   // mean of the histogram
float hist_mnsq = 0;   // used to calculate stddev of histogram
float hist_stdd = 0;   // stddev of the histogram

const int NDATA = 100; // number of points to collect for the histogram

const int NMEAS = 20; // number of measurements to average
float sum   = 0;
//float sumsq = 0;
int   n     = 0;
int   npts  = 0;
float mean;
float stddev;
float analog_mean; // debug

float get_dist(int n) {  // elecronoobs version, using exponential model
  long sum=0;
  for(int i=0;i<n;i++) {
    sum=sum+analogRead(A0);
  }  
  float adc=sum/n;
  // WS changed to mm, multiply by 10.0
  float distance_mm = 10.0 * 17569.7 * pow(adc, -1.2062); 
  return(distance_mm);
}

void print_LUT() {
  // exceed legal bounds to test
  for (int i = (START - 5); i < (STOP + 5); i++) {
    Serial.print("i = ");
    Serial.print(i);
    Serial.print("  LUT(i) = ");
    Serial.println(sensor1.getDist());
    delay(100);
  }
}

void waitForSerial() {
  while(Serial.available()){Serial.read();}
  while (!Serial.available()) { }
  Serial.println(Serial.read());
  Serial.println("");
}

void setup() {
  Serial.begin(9600);
  //delay(500);
  //print_LUT();
}

void loop() {

  int val = sensor1.getDist(20);

  if (npts == NDATA) {
    Serial.print("num of analogReads per sample: ");
    Serial.println(NMEAS);
    Serial.print("num of samples: ");
    Serial.println(NDATA);
    Serial.println("Histo: only non-zero bins are shown");
    for (int i = 0; i <= NBINS; i++) {
      if (hist[i] >= 1){
        Serial.print("i = ");
        Serial.print(i);
        Serial.print("  hist(i) = ");
        Serial.println(hist[i]); 
        hist[i] = 0; // get ready for next histogram
      }
    }
    hist_mean /= float(NDATA);
    hist_mnsq /= float(NDATA);
    hist_stdd = sqrt(hist_mnsq - hist_mean * hist_mean);
    analog_mean /= (float(NDATA) * float(NMEAS));  // debug
    Serial.print("LUT          distance: ");
    Serial.print(hist_mean);
    Serial.print("   stddev: ");
    Serial.println(hist_stdd);

    Serial.print("Electronoobs distance: ");
    float noobs_dist = get_dist(NMEAS);
    Serial.println(noobs_dist);

    Serial.print("Analog mean: ");
    Serial.println(analog_mean);

    
    Serial.println("");
    Serial.print("enter any key:");
    waitForSerial();
    hist_mean = 0;
    hist_mnsq = 0;
    npts      = 0;
    
    //exit(0); // exit the loop
  }


  //delay(1);

  sum   += val;
  //sumsq += float(val) * float(val); 
  n     += 1;

  if (n == NMEAS) {
    mean   = sum/float(n);
    //stddev = sqrt(sumsq/float(n) - mean * mean);
    int indx = (int)mean;
    //Serial.print("indx ");
    //Serial.println(indx);
    if (indx > NBINS) {indx = NBINS;}
    hist[indx] += 1;
    hist_mean  += indx;
    hist_mnsq  += (float)indx * (float)indx;
    npts += 1;
    /*
    Serial.print("Mean: ");
    Serial.print(int(mean));
    Serial.print("  Stddev: ");
    Serial.println(int(stddev));
    delay(100);
    */
    n     = 0;
    sum   = 0;
    //sumsq = 0;
    }
}
