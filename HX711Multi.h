#ifndef HX711Multi_h
#define HX711Multi_h

/*
 * Library for the HX711 module, based on an old version from https://github.com/bogde/HX711 - thanks to Bogde
 * Library is changed from waiting for the HX711 to convert, to a method that updates, and returns, if there are nothing to do
 * The update stores converted values in a buffer array, so an average value can be calculated, when the wanted number of values
 * are stored in the buffer.
 * The library can handle two loadcells on the same HX711, and update switches automatically between the loadcells.
 * However, the HX711 puts in an extra delay of about 4 conversions, when switching between the loadcelles.
 * This means that the conversion will take 0.5 second in 10Hz mode, resulting in that using two channels gives one conversion
 * per second at each loadcell in 10Hz mode - in 80 Hz mode this reduced to about 60 ms, giving conversion from both 8 times a second.
 * Another change in the library is that the readings are changed from values with the zero-point at 8388608 to a normal long format
 * with zero-point at 0 thus reading positive and negative values with the getAverage.
 * The library is adapted with the changes from the new version from https://github.com/bogde/HX711 - thanks to Bogden
 * These changes ensures that the library can function at som fast processors - only testet at WeMOS D1 Pro (ESP8266 based board)
 * The library does not protect against interrupts.
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class HX711Multi
{
	private:
		byte PD_SCK;	// Power Down and Serial Clock Input Pin
		byte DOUT;		// Serial Data Output Pin
		byte GAIN;		// amplification factor
		byte CELLS; 	// number of loadcells attached (1 or 2), if one, it is at the A-channel, named 0
		byte MAXAVG;	// the max nubmer of used averageSize, in order not to overflow the array values
		long* values;   // array for values
		long OFFSET1;	// used for tare weight
		float SCALE1;	// used to return weight in grams, kg, ounces, whatever
		long OFFSET2;	// used for tare weight
		float SCALE2;	// used to return weight in grams, kg, ounces, whatever
		byte _nextGain; // used when shifting between two channels

		// is only to be called if the chip is ready - therefor it is private. It returns the reading
		long read();

		// check if HX711 is ready
		// from the datasheet: When output data is not ready for retrieval, digital output pin DOUT is high. Serial clock
		// input PD_SCK should be low. When DOUT goes to low, it indicates data is ready for retrieval.
		bool isReady();

	public:
		// define clock and data pin, average size, number of Loadcells connected and gain factor for channel 0 (A-channel)
		// channel selection is made by passing the appropriate gain: 128 or 64 for channel A, 32 for channel B
		// gain: 128 or 64 for channel A; channel B works with 32 gain factor only
		HX711Multi(byte dout, byte pd_sck, byte averageSize = 20, byte loadCells = 1, byte gain0 = 128);

		virtual ~HX711Multi();

		// check if HX711 is ready
		// returns -1, if there are no new value converted.
		// returns 1 if channel A is ready and 2 if channel B is ready in the averaging buffer
		signed char update();

		// set the gain factor; takes effect only after a call to read()
		// channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
		// depending on the parameter, the channel is also set to either A or B
		void setGain(byte gain = 128);

		// returns an average reading; times = how many values to use from the averaging buffer
		// the method returns a value from the latest reading stored by update() in the averaging buffer
		double getAverage(byte cell = 1, byte times = 10);

		// returns (getAverage() - OFFSET), that is the current value without the tare weight
		// times = how many readings to take from the averaging buffer
		// different offset should be set for each cell, using tare
		double getValue(byte cell = 1, byte times = 1);

		// returns getValue() divided by SCALE, that is the raw value divided by a value obtained via calibration
		// times = how many readings to take from the averaging buffer
		float getUnits(byte cell = 1, byte times = 1);

		// set the OFFSET value for tare weight of the cell; times = how many times to read the tare value
		// the method waits for the module to measure the number og times, before calculation the OFFSET
		// taring should be done for each cell
		void tare(byte cell = 1, byte times = 10);

		// set the SCALE value; this value is used to convert the raw data to "human readable" data (measure units)
		// scaling sould be done for each cell
		void setScale(byte cell = 1, float scale = 1.0);

		// set OFFSET, the value that's subtracted from the actual reading (tare weight)
		void setOffset(byte cell = 1, long offset = 0);

		// puts the chip into power down mode
		void powerDown();

		// wakes up the chip after power down mode
		void powerUp();
};

#endif /* HX711Multi_h */