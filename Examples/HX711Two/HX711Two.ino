#include "HX711Multi.h"
/*
 * Example that illustrates how 2 loadcells can be read, using only 1 HX711 modules.
 * The techniqe to read both channels is using HX711Multi library, that does not wait for the 
 * conversion to complete, but checks if there are a value ready for reading.
 * The library changes the channel to be read each time, and since this channel-shift
 * in the module causes extra conversion time, each channel is limited to one conversion 
 * pr. second, using 10Hz mode
 * The averaging in the library puts the last read value in a buffer. Taking the average
 * of 5 values returns the readings made in the preciding 5 seconds
 */

// HX711.DOUT  - pin #2
// HX711.PD_SCK - pin #3

HX711Multi scale(D2, D3, 20, 2, 64);   // parameter "gain" is set to 64 - close to 32 of the B-Channel
double scaleConstant = 113.59;

void setup() {
  Serial.begin(9600);
  Serial.println("\r\nHX711 Demo - Two Channels");

  Serial.print("read: \t");
  while (scale.update() == -1);         // Wait for the first conversion to be ready
  Serial.print(scale.getAverage(1, 1));     // print a raw reading from the ADC
  while (scale.update() == -1);         // Wait for the first conversion to be ready
  Serial.println(scale.getAverage(2, 1));     // print a raw reading from the ADC
  wdt_reset();
  Serial.print("Filling average buffer");
  for (byte n = 0; n < 20; n++) {
    Serial.print("A: ");
    while (scale.update() == -1);
    Serial.print(scale.getAverage(1, 20));   // print the average of 20 readings from the ADC
    Serial.print("\tB: ");
    while (scale.update() == -1);
    Serial.println(scale.getAverage(2, 20));   // print the average of 20 readings from the ADC
    wdt_reset();
  }
  Serial.println("Taring A");
  wdt_reset();
  scale.tare(1, 20);                              // Tare off the scale
  scale.setScale(1, scaleConstant);
  wdt_reset();
  Serial.println("Taring B");
  scale.tare(2, 20);                              // Tare off the scale
  scale.setScale(2, scaleConstant/2.0);
  Serial.println("Readings:");
  wdt_reset();
}

void loop() {
  signed char cell = scale.update();
  if (cell != -1) {
    if (cell == 1) {
      Serial.print(millis());
    }
    Serial.print("\tread ");
    if (cell == 1) {
      Serial.print("A:\t");
    } else {
      Serial.print("B:\t");
    }
    Serial.print(scale.getAverage(cell, 1));     // print a raw reading from the ADC

    Serial.print("\tavg5:\t");
    Serial.print(scale.getUnits(cell, 5));   // print scaled readings
    if (cell == 2) {
      Serial.println();
    }
    wdt_reset();
  }
}
