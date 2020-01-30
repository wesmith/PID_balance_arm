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
const float MEANS[] = {465.3, 290.0, 218.7, 194.3, 186.3, 179.0, 170.0, 159.3, 146.7};  // raw analogRead() values
const int N = 9; // number of calibration values
const int START = 147;  // lowest  analogRead() value for calibration
const int STOP  = 465;  // highest analogRead() value for calibration
int LUT[319];

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
  int indx     = analog - START;
  int maxIndx  = STOP   - START; 
  if (indx < 0) {indx = 0;}
  if (indx > maxIndx) {indx = maxIndx;}
  return LUT[indx];
}

void print_LUT() {
  // exceed legal bounds to test
  for (int i = (START - 5); i = (STOP + 5); i++) {
    Serial.println(i, get_distance(i));
  }
}

void setup() {
  Serial.begin(9600);
  create_LUT();
  print_LUT();
}

void loop() {
  //Serial.println('Hello');

}
