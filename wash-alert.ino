#include "Average.h"

const uint16_t samples = 512;

boolean isWashing;
uint8_t a[samples];
uint16_t i;

void setup() {
  pinMode(A0,INPUT);
  Serial.begin(9600);

  isWashing = false;
  i = 0;
}

float mean() {
  float sum = 0;
  for (uint16_t n = 0; n < samples; n++) {
    sum += a[n];
  }

  return sum/samples;
}

float stddev(float mu) {
  float sum = 0;
  for(uint16_t n = 0; n < samples; n++) {
    float theta = mu - a[n];
    float square = theta * theta;
    sum += square;
  }
  return sqrt(sum/samples);
}

void loop() {

  uint8_t val = analogRead(A0)/4;
  
  if (i == samples) i = 0;
  a[i] = val;
  i++;

  float m = mean();
  float sd = stddev(m);
  
  Serial.print("reading: ");
  Serial.println(val);
  Serial.print("mean: ");
  Serial.println(m);
  Serial.print("stddev: ");
  Serial.println(sd);

  
  delay(100);

  
}

