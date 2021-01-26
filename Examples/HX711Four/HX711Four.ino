#include "HX711Multi.h"
/*
 * Example that illustrates how 4 loadcells can be read, using only 2 HX711 modules.
 * The techniqe to read both channels is using HX711Multi library, that does not wait for the 
 * conversion to complete, but checks if there are a value ready for reading.
 * The library changes the channel to be read each time, and since this channel-shift
 * in the module causes extra conversion time, each channel is limited to one conversion 
 * pr. second, using 10Hz mode
 * Since the modules have slightly different converions speed, the prinout disrupts
 * once in a while, changing the order i which they are displayed
 * The averaging in the library puts the last read value in a buffer. Taking the average
 * of 5 values returns the readings made in the preciding 5 seconds
 */

// HX711.DOUT  - pin #2
// HX711.PD_SCK - pin #3

HX711Multi scale1(D2, D3, 20, 2, 64);   // parameter "gain" is set to 64 - close to 32 of the B-Channel
HX711Multi scale2(D5, D6, 20, 2, 64);   // parameter "gain" is set to 64 - close to 32 of the B-Channel
double scaleConstant = 113.59;

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("\r\nHX711 Demo - Four Channels");

  Serial.print("read2: \t");
  while (scale1.update() == -1);         // Wait for the first conversion to be ready
  Serial.print(scale1.getAverage(1, 1));     // print a raw reading from the ADC
  while (scale1.update() == -1);         // Wait for the first conversion to be ready
  Serial.println(scale1.getAverage(2, 1));     // print a raw reading from the ADC
  wdt_reset();
  Serial.print("read2: \t");
  while (scale2.update() == -1);         // Wait for the first conversion to be ready
  Serial.print(scale2.getAverage(1, 1));     // print a raw reading from the ADC
  while (scale2.update() == -1);         // Wait for the first conversion to be ready
  Serial.println(scale2.getAverage(2, 1));     // print a raw reading from the ADC
  wdt_reset();
  Serial.print("Filling average buffer");
  for (byte n = 0; n < 20; n++) {
    Serial.print("A1: ");
    while (scale1.update() == -1);
    Serial.print(scale1.getAverage(1, 20));   // print the average of 20 readings from the ADC
    Serial.print("\tB1: ");
    while (scale1.update() == -1);
    Serial.println(scale1.getAverage(2, 20));   // print the average of 20 readings from the ADC
    wdt_reset();
    Serial.print("A2: ");
    while (scale2.update() == -1);
    Serial.print(scale2.getAverage(1, 20));   // print the average of 20 readings from the ADC
    Serial.print("\tB2: ");
    while (scale2.update() == -1);
    Serial.println(scale2.getAverage(2, 20));   // print the average of 20 readings from the ADC
    wdt_reset();
  }
  Serial.println("Taring A1");
  wdt_reset();
  scale1.tare(1, 20);                              // Tare off the scale
  scale1.setScale(1, scaleConstant);
  wdt_reset();
  Serial.println("Taring B1");
  scale1.tare(2, 20);                              // Tare off the scale
  scale1.setScale(2, scaleConstant/2.0);
  wdt_reset();
  Serial.println("Taring A2");
  wdt_reset();
  scale2.tare(1, 20);                              // Tare off the scale
  scale2.setScale(1, scaleConstant);
  wdt_reset();
  Serial.println("Taring B2");
  scale2.tare(2, 20);                              // Tare off the scale
  scale2.setScale(2, scaleConstant/2.0);
  Serial.println("Readings:");
  wdt_reset();
}

byte printed = 0;

void loop() {
  signed char cell = scale1.update();
  if (cell != -1) {
    printed++;
    if (printed == 4) {
      Serial.println(millis());
      printed = 0;
    }
    Serial.print("read ");
    if (cell == 1) {
      Serial.print("A1:\t");
    } else {
      Serial.print("B1:\t");
    }
    Serial.print(scale1.getUnits(cell, 5));   // print scaled readings
    Serial.print("\t");
    wdt_reset();
  }
  cell = scale2.update();
  if (cell != -1) {
    printed++;
    if (printed == 4) {
      Serial.println(millis());
      printed = 0;
    }
    Serial.print("read ");
    if (cell == 1) {
      Serial.print("A2:\t");
    } else {
      Serial.print("B2:\t");
    }
    Serial.print(scale2.getUnits(cell, 5));   // print scaled readings
    Serial.print("\t");
    wdt_reset();
  }
}
