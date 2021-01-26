#include "HX711Multi.h"
/*
 * Simple example that illustrates how 1 loadcell can be read, from a HX711 modules.
 * The techniqe used in the HX711Multi library, is not to wait for the 
 * conversion to complete, but checks if there are a value ready for reading.
 * The library can still convert 10 values pr. second, using 10Hz mode
 * The averaging in the library puts the last read value in a buffer. Taking the average
 * of 20 values returns the readings made in the preciding two seconds
 */

// HX711.DOUT  - pin #2
// HX711.PD_SCK - pin #3

HX711Multi scale(D2, D3, 20, 1);   // parameter "gain" is ommited; the default value 128 is used by the library
double scaleConstant = 113.59;

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 Demo One cell - A-Channel");
  scale.setGain(128);

  Serial.print("read: \t");
  while (scale.update() == -1);         // Wait for the first conversion to be ready
  Serial.print(scale.getAverage(1, 1));     // print a raw reading from the ADC
  wdt_reset();
  Serial.print("\t average: \t");
  for (byte n = 0; n < 20; n++) {
    while (scale.update() == -1);
    Serial.println(scale.getAverage(1, 20));   // print the average of 20 readings from the ADC
    wdt_reset();
  }
  Serial.println("Taring");
  scale.tare(1, 20);                              // Tare off the scale
  scale.setScale(1, scaleConstant);
  Serial.println("Readings:");
  wdt_reset();
}

void loop() {
  signed char cell = scale.update();
  if (cell != -1) {
    Serial.print("read: ");
    Serial.print(cell);
    Serial.print("\t");
    Serial.print(millis());
    Serial.print("\t");
    Serial.print(scale.getAverage(1, 1));     // print a raw reading from the ADC

    Serial.print("\t average: \t");
    Serial.print(scale.getUnits(1, 5));   // print scaled readings
    Serial.print("\t");
    Serial.println(scale.getValue(1, 5) / scaleConstant);         // print the average of 5 readings from the ADC
  }
  wdt_reset();
}
