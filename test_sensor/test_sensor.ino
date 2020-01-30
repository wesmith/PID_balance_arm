/*test_sensor.ino
 * 
 * WESmith 01/29/20
 * 
 * test distance sensors using calibration data obtained with assistance from calibrate_sensor.ino
 * 
 */
// calibration data for the GP2Y0A21YKOF IR sensor: 
// white ping-pong ball on slide, distance from sensor to front of ping-pong ball in mm
// analogRead() values in MEANS array below found in fit_sensor.ipynb
const float DIST[]  = {100.0, 150.0, 200.0, 250.0, 300.0, 350.0, 400.0, 450.0, 500.0};  // mm

// can tweak the 9 values below to get more accurate distances: did this by hand (original commented out)
//const float MEANS[] = {465.3, 290.0, 218.7, 194.3, 186.3, 179.0, 170.0, 159.3, 146.7};  // raw analogRead() values
const float MEANS[] =   {465.3, 290.0, 218.7, 196.0, 190.0, 179.0, 170.0, 159.3, 146.7};  // TWEAKED values


const int N = 9; // number of calibration values
const int START = 147;  // lowest  analogRead() value for calibration
const int STOP  = 465;  // highest analogRead() value for calibration
int LUT[319];

const int NBINS = 400;
int   hist[401] = {0}; // histogram array, just out to 400 mm
float hist_mean = 0;   // mean of the histogram
float hist_mnsq = 0;   // used to calculate stddev of histogram
float hist_stdd = 0;   // stddev of the histogram

const int NDATA = 500; // number of points to collect for the histogram

const int NMEAS = 20; // number of measurements to average
float sum   = 0;
//float sumsq = 0;
int   n     = 0;
int   npts  = 0;
float mean;
float stddev;


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

int get_distance(int analog) {
  // analog is the analogRead() value from arduino
  // output distance is in mm
  int indx     = analog - START;
  int maxIndx  = STOP   - START; 
  if (indx < 0) {indx = 0;}
  if (indx > maxIndx) {indx = maxIndx;}
  return LUT[indx];
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
  //delay(500);
  //print_LUT();
}

void loop() {

  if (npts > NDATA) {
    Serial.print("number of measurements per distance value is ");
    Serial.println(NMEAS);
    Serial.println("Histogram follows: only non-zero bins are shown");
    for (int i = 0; i <= NBINS; i++) {
      if (hist[i] > 1){
      Serial.print("i = ");
      Serial.print(i);
      Serial.print("  hist(i) = ");
      Serial.println(hist[i]); 
      }
    }
    hist_mean /= float(NDATA);
    hist_mnsq /= float(NDATA);
    hist_stdd = hist_mnsq - (hist_mean * hist_mean);
    Serial.print("Histogram mean: ");
    Serial.println(hist_mean);
    Serial.print("Histogram variance: ");
    Serial.println(hist_stdd);
    delay(500);
    exit(0); // exit the loop
  }

  int analog = analogRead(A0);
  int val    = get_distance(analog);

  //delay(1);

  sum   += val;
  //sumsq += float(val) * float(val); 
  n     += 1;

  if (n == NMEAS) {
    mean   = sum/float(n);
    //stddev = sqrt(sumsq/float(n) - mean * mean);
    int indx = (int)mean;
    if (indx > NBINS) {indx = NBINS;}
    hist[indx] += 1;
    hist_mean  += mean;
    hist_mnsq  += mean * mean;
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
