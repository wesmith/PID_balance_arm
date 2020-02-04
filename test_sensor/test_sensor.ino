/*test_sensor.ino
 * 
 * WESmith 01/29/20
 * 
 * test distance sensors using calibration data obtained from calibrate_sensor.ino
 * NOTE: values can be tweaked in real-time using this code: see modified MEANS array below. 
 */

#include "IR_sensor.h"

int       NMEAS = 20;  // number of measurements to average for 1 point
const int NDATA = 100; // number of points to collect for the histogram
const int NBINS = 400; //350;

const byte sensor_pin = A0;
DistSensor sensor1(DistSensor::GP2Y0A21YK0F_5V, sensor_pin);

float hist_mean   = 0;   // mean of the histogram
float hist_mnsq   = 0;   // used to calculate stddev of histogram
float hist_stdd   = 0;   // stddev of the histogram
float analog_mean = 0;   // mean of the analog readings: useful for on-the-fly LUT analysis
int   npts        = 0;
float mean;
float stddev;
unsigned int analog;
unsigned int dist;
int val = 0;

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

void waitForSerial() {
  while(Serial.available()){Serial.read();}
  while (!Serial.available()) { }
  Serial.println(Serial.read());
  Serial.println("");
}

void histogram() {
  
  // histogram array out to NBINS + 1 
  // has to be static to preserve state, uses up SRAM
  static int hist[401] = {0};

  // generate histogram
  if (dist > NBINS) {dist = NBINS;}
  hist[dist]  += 1;
  hist_mean   += dist;
  hist_mnsq   += (float)dist * (float)dist;
  analog_mean += analog;
  npts        += 1;

  // print histogram
  if (npts == NDATA) {
    Serial.print("analogReads per sample: ");
    Serial.print(NMEAS);
    Serial.print("  num of samples: ");
    Serial.println(NDATA);
    Serial.println("Histo: non-zero bins");
    for (int i = 0; i <= NBINS; i++) {
      if (hist[i] > 0){
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
    analog_mean /= float(NDATA);
    Serial.print("LUT distance: ");
    Serial.print(hist_mean);
    Serial.print("   stddev: ");
    Serial.print(hist_stdd);
    Serial.print("   analog mean: ");
    Serial.println(analog_mean);

    //Serial.print("Electronoobs distance: ");
    //float noobs_dist = get_dist(NMEAS);
    //Serial.println(noobs_dist);
    
    hist_mean   = 0;
    hist_mnsq   = 0;
    analog_mean = 0;
    npts        = 0;

    Serial.println("CR to continue");
    waitForSerial();
    //getInput();
  }
}

void setup() {
  Serial.begin(9600);
  //sensor1.print_LUT();
}

void loop() {

  sensor1.get_dist(analog, dist, NMEAS);

  histogram(); 
}
