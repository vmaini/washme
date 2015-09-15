const uint8_t inputPin = 0;
const uint16_t samples = 1000;
const uint16_t delayTime = 1000; // ms
const uint16_t baudRate = 9600;
const uint8_t stddevThreshold = 50;
const uint16_t washOnThreshold = 10000;
const uint16_t washOffThreshold = 1000;

bool offAfterOn;
uint16_t count;
uint8_t a[samples];
uint16_t i;

void setup() {
  Serial.begin(baudRate);

  offAfterOn = false;
  count = 0;
  i = 0;
}

float mean() {
  float sum = 0;
  for (uint16_t n = 0; n < samples; ++n) {
    sum += a[n];
  }
  return sum/samples;
}

// mu is the mean of a
// returns the stddev of the values in a
float stddev(float mu) {
  float sum = 0;
  for (uint16_t n = 0; n < samples; ++n) {
    float theta = mu - a[n];
    float square = theta * theta;
    sum += square;
  }
  return sqrt(sum/samples);
}

bool checkIfOff(float sd) {
  if (offAfterOn) {
    if (sd <= stddevThreshold) {
      count++;
      if (count > washOffThreshold) {
        offAfterOn = false;
        count = 0;
        return true;
      }
    } else {

      // TODO We may need to be more tolerant of a single
      // stddev above the threshold. More testing is needed.
      offAfterOn = false;
      count = 1;
    }
  } else {
    if (sd > stddevThreshold) {
      count++;
    }
    else if (count > washOnThreshold) {
      // The washer just turned off
      // TODO We may need to be more tolerant of a single
      // stddev below the threshold. More testing is needed.
      offAfterOn = true;
      count = 1;
    } else {
      // Reset count
      count = 0;
    }
  }
  return false;
}

void sendAlert() {
  // TODO
  return;
}

void loop() {
  
  uint64_t startUs = micros();
  // analogRead returns a value in [0, 1023]
  // To save memory we compress this to an 8-bit value
  uint8_t reading = analogRead(inputPin)/4;
  
  a[i] = reading;

  ++i;
  if (i == samples) i = 0;

  float m = mean();
  float sd = stddev(m);


  // For debugging
  Serial.print("reading: ");
  Serial.println(reading);
  Serial.print("mean: ");
  Serial.println(m);
  Serial.print("stddev: ");
  Serial.println(sd);
  Serial.println("--------");

  if (checkIfOff(sd)) sendAlert();


  // TODO Check that the loop runs in under 5ms
  Serial.print("loop time in us: ");
  uint64_t endUs = micros();
  Serial.println(endUs-startUs);

  // We want the loop to take delayTime in total
  // We subtract the loop runtime so far from this
  // desired loop time and delay on the remainder.
  // We catch the case where we already take longer
  // than the desired delay - then we print an error
  // and do not delay.
  // TODO delayMicroseconds doc mentions that delayMicroseconds
  // should not be used for values > 16000. We should use delay instead.
  if (startUs + delayTime * 1000 > endUs) {
    delayMicroseconds(startUs+delayTime*1000-endUs);
  } else {
    Serial.println("***Warning: Loop time exceeds delay time");
  }
}

