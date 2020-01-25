/*calibrate_sensor.ino
 * 
 * WSmith 01/25/20
 * 
 * calibrate Pololu-mounted sensor GP2Y0A60SZLF
 * 
 * The mean and standard deviation of the raw analogRead counts are scrolled to the serial port:
 * place an object at the desired distance to the sensor, record the mean value of the raw data, and repeat for 
 * different object distances. This will produce a table of raw counts vs distance. A curve-fitting routine is then
 * applied (typically a 5th-order polynomial) to associate the raw analogRead counts to the desired distance units. 
 * 
 */

const int N = 1000; // number of measurements to average
float sum   = 0;
float sumsq = 0;
int   n     = 0;
float mean;
float stddev;


void setup() {
  Serial.begin(9600);
}

void loop() {
  
  int val = analogRead(A0);

  sum   += val;
  sumsq += float(val) * float(val); 
  n     += 1;

  if (n == N) {
    mean   = sum/float(n);
    stddev = sqrt(sumsq/float(n) - mean * mean);
    Serial.print("Mean: ");
    Serial.print(mean);
    Serial.print("  Stddev: ");
    Serial.println(stddev);
    delay(50);
    n     = 0;
    sum   = 0;
    sumsq = 0;
  }
}
