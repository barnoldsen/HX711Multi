#include "HX711Multi.h"
/*
 * Example that illustrates how 3 loadcells can be read fast, using 3 HX711 modules.
 * The techniqe to read fast is using HX711Multi library, that does not wait for the 
 * conversion to complete, but checks if there are a value ready for reading.
 * Since the modules have slightly different converions speed, the prinout disrupts
 * once in a while, changing the order i which they are displayed.
 * The library can still convert 10 values pr. second from each module, using 10Hz mode
 * The averaging in the library puts the last read value in a buffer. Taking the average
 * of 20 values returns the readings made in the preciding two seconds
 */
// HX711.DOUT  - pin #2
// HX711.PD_SCK - pin #3

HX711Multi scale1(D2, D3, 20, 1);   // parameter "gain" is ommited; the default value 128 is used by the library
HX711Multi scale2(D5, D6, 20, 1);   // parameter "gain" is ommited; the default value 128 is used by the library
HX711Multi scale3(D7, D8, 20, 1);   // parameter "gain" is ommited; the default value 128 is used by the library
double scaleConstant = 227.18;

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("HX711 Demo Three cell Fast, all A-Channel");

  Serial.print("read1: \t");
  while (scale1.update() == -1);         // Wait for the first conversion to be ready
  Serial.print(scale1.getAverage(1, 1));     // print a raw reading from the ADC
  wdt_reset();
  Serial.print("\tavg 1:\t");
  for (byte n = 0; n < 20; n++) {
    while (scale1.update() == -1);
    Serial.println(scale1.getAverage(1, 20));   // print the average of 20 readings from the ADC
    wdt_reset();
  }
  Serial.println("Taring 1");
  scale1.tare(1, 20);                              // Tare off the scale
  scale1.setScale(1, scaleConstant);
  wdt_reset();

  Serial.print("read2: \t");
  while (scale2.update() == -1);         // Wait for the first conversion to be ready
  Serial.print(scale2.getAverage(1, 1));     // print a raw reading from the ADC
  wdt_reset();
  Serial.print("\tavg 2: \t");
  for (byte n = 0; n < 20; n++) {
    while (scale2.update() == -1);
    Serial.println(scale2.getAverage(1, 20));   // print the average of 20 readings from the ADC
    wdt_reset();
  }
  Serial.println("Taring 2");
  scale2.tare(1, 20);                              // Tare off the scale
  scale2.setScale(1, scaleConstant);
  wdt_reset();

  Serial.print("read3: \t");
  while (scale3.update() == -1);         // Wait for the first conversion to be ready
  Serial.print(scale3.getAverage(1, 1));     // print a raw reading from the ADC
  wdt_reset();
  Serial.print("\tavg 3: \t");
  for (byte n = 0; n < 20; n++) {
    while (scale3.update() == -1);
    Serial.println(scale3.getAverage(1, 20));   // print the average of 20 readings from the ADC
    wdt_reset();
  }
  Serial.println("Taring 3");
  scale3.tare(1, 20);                              // Tare off the scale
  scale3.setScale(1, scaleConstant);
  Serial.println("Readings:");
  wdt_reset();
}

void loop() {
  signed char cell = scale1.update();
  if (cell != -1) {
    Serial.print("read 1 ");
    Serial.print("\t");
    Serial.print(scale1.getUnits(1, 5));   // print scaled readings
  }
  cell = scale2.update();
  if (cell != -1) {
    Serial.print("read 2 ");
    Serial.print("\t");
    Serial.print(scale2.getUnits(1, 5));   // print scaled readings
  }
  cell = scale3.update();
  if (cell != -1) {
    Serial.print("read 3 ");
    Serial.print("\t");
    Serial.print(scale3.getUnits(1, 5));   // print scaled readings
    Serial.print("\t");
    Serial.println(millis());
  }
}
