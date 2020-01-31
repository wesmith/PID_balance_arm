/*test_sensor.ino
 * 
 * WESmith 01/29/20
 * 
 * test distance sensors using calibration data obtained with assistance from calibrate_sensor.ino
 * 
 */
// calibration data for the GP2Y0A21YKOF IR sensor: 
// white ping-pong ball on slide, distance from sensor to front of ping-pong ball in mm
// analogRead() values in MEANS array below found in fit_sensor.ipynb (before tweaking)

// can tweak the 9 values below to get more accurate distances: did this by hand (original commented out)
//const float MEANS[] = {465.3, 290.0, 218.7, 194.3, 186.3, 179.0, 170.0, 159.3, 146.7};  // raw analogRead() values
const float MEANS[] =   {465.3, 290.0, 218.7, 196.0, 193.0, 179.0, 170.0, 159.3, 146.7};  // TWEAKED values
const float DIST[]  =   {100.0, 150.0, 200.0, 250.0, 300.0, 350.0, 400.0, 450.0, 500.0};  // dist in mm


const int N = 9; // number of calibration values
const int START = 147;  // lowest  analogRead() value for calibration
const int STOP  = 465;  // highest analogRead() value for calibration
int LUT[319];

const int NBINS = 350;
int   hist[351] = {0}; // histogram array, just out to NBINS (note: have to reduce this if too much global mem used)
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


void create_LUT() {
// LUT incorporating linear interpolation: polynomial fits were problematic with this data
  for (int i = START; i <= STOP; i++) {
    for (int j = 0; j < (N-1); j++) {
      if ((i < MEANS[j]) && (i >= MEANS[j+1])) {
        LUT[i-START] = (int)((i - MEANS[j]) * (DIST[j+1] - DIST[j]) / (MEANS[j+1] - MEANS[j]) + DIST[j] + 1); 
        break;
      }
    }
  }
}

int get_distance(int analog) {  // WS version, using linear LUT
  // analog is the analogRead() value from arduino
  // output distance is in mm
  int indx     = analog - START;
  int maxIndx  = STOP   - START; 
  if (indx < 0) {indx = 0;}
  if (indx > maxIndx) {indx = maxIndx;}
  return LUT[indx];
}

float get_dist(int n) {  // elecronoobs version, using exponential model
  long sum=0;
  for(int i=0;i<n;i++)
  {
    sum=sum+analogRead(A0);
  }  
  float adc=sum/n;
  //float volts = analogRead(adc)*0.0048828125;  // value from sensor * (5/1024)
  //float volts = sum*0.003222656;  // value from sensor * (3.3/1024) EXTERNAL analog refference

  float distance_mm = 10.0 * 17569.7 * pow(adc, -1.2062);  // WS changed to mm, multiply by 10.0
  //float distance_cm = 13*pow(volts, -1); 
  return(distance_mm);
}

void print_LUT() {
  // exceed legal bounds to test
  for (int i = (START - 5); i < (STOP + 5); i++) {
    Serial.print("i = ");
    Serial.print(i);
    Serial.print("  LUT(i) = ");
    Serial.println(get_distance(i));
    delay(100);
  }
}

void setup() {
  Serial.begin(9600);
  create_LUT();
  Serial.println("Created LUT");
  //delay(500);
  //print_LUT();
}

void loop() {

  if (npts == NDATA) {
    Serial.print("number of measurements per distance value is ");
    Serial.println(NMEAS);
    Serial.println("Histogram follows: only non-zero bins are shown");
    for (int i = 0; i <= NBINS; i++) {
      if (hist[i] >= 1){
        Serial.print("i = ");
        Serial.print(i);
        Serial.print("  hist(i) = ");
        Serial.println(hist[i]); 
      }
    }
    hist_mean /= float(NDATA);
    hist_mnsq /= float(NDATA);
    hist_stdd = sqrt(hist_mnsq - hist_mean * hist_mean);
    analog_mean /= (float(NDATA) * float(NMEAS));  // debug
    Serial.print("Histogram mean (ie, distance using the LUT): ");
    Serial.println(hist_mean);
    //Serial.print("hist_mnsq diagnostic ");
    //Serial.println(hist_mnsq);
    Serial.print("Histogram stddev (ie, stddev of distance): ");
    Serial.println(hist_stdd);
    Serial.print("Analog mean: ");
    Serial.println(analog_mean);

    Serial.print("Electronoobs distance: ");
    float noobs_dist = get_dist(NMEAS);
    Serial.println(noobs_dist);

    
    delay(500);
    exit(0); // exit the loop
  }

  int analog = analogRead(A0);
  analog_mean += analog;  // debug
  int val    = get_distance(analog);

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
