#include <Arduino.h>
#include <HX711Multi.h>

/*
 * Library for the HX711 module, based on an old version fra https://github.com/bogde/HX711 - thanks to Bogde
 * Library is changed from waiting for the HX711 to convert, to a method that updates, and returns, if there are nothing to do
 * The update stores converted values in a buffer array, so an average value can be calculated, when the wanted number of values
 * are stored in the buffer.
 * The library can handle two loadcells on the same HX711, and update switches automatically between the loadcells.
 * However, the HX711 puts in an extra delay of about 4 conversions, when switching between the loadcelles
 * This means that the conversion will take 0.5 second in 10Hz mode, resulting in that using two channels gives one conversion
 * per second at each loadcell in 10Hz mode - in 80 Hz mode this reduced to about 60 ms, giving conversion from both 8 times a second.
 * Another change in the library is that the readings are changed from values with the zero-point at 8388608 to a normal long format
 * with zero-point at 0 thus reading positive and negative values with the getAverage.
 * The library is adapted with the changes from the new version from https://github.com/bogde/HX711 - thanks to Bogden
 * These changes ensures that the library can function at som fast processors - only testet at WeMOS D1 Pro (ESP8266 based board)
 * The library does not protect against interrupts.
*/

// TEENSYDUINO has a port of Dean Camera's ATOMIC_BLOCK macros for AVR to ARM Cortex M3.
#define HAS_ATOMIC_BLOCK (defined(ARDUINO_ARCH_AVR) || defined(TEENSYDUINO))

// Whether we are running on either the ESP8266 or the ESP32.
#define ARCH_ESPRESSIF (defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32))

// Whether we are actually running on FreeRTOS.
#define IS_FREE_RTOS defined(ARDUINO_ARCH_ESP32)

// Define macro designating whether we're running on a reasonable
// fast CPU and so should slow down sampling from GPIO.
#define FAST_CPU \
    ( \
    ARCH_ESPRESSIF || \
    defined(ARDUINO_ARCH_SAM)     || defined(ARDUINO_ARCH_SAMD) || \
    defined(ARDUINO_ARCH_STM32)   || defined(TEENSYDUINO) \
    )

#if HAS_ATOMIC_BLOCK
// Acquire AVR-specific ATOMIC_BLOCK(ATOMIC_RESTORESTATE) macro.
#include <util/atomic.h>
#endif

HX711Multi::HX711Multi(byte dout, byte pd_sck, byte averageSize, byte loadCells, byte gain0)
{
	PD_SCK = pd_sck;
	DOUT = dout;
	pinMode(PD_SCK, OUTPUT);
	pinMode(DOUT, INPUT);
	MAXAVG = averageSize;

	SCALE1 = 1.0;
	OFFSET1 = 0;
	SCALE2 = 1.0;
	OFFSET2 = 0;
	if (loadCells == 2)
	{
		CELLS = 2;
	}
	else
	{
		CELLS = 1;
	}

	setGain(gain0);
	values = new long[averageSize * CELLS];
}

HX711Multi::~HX711Multi()
{
}

bool HX711Multi::isReady()
{
	return digitalRead(DOUT) == LOW;
}

signed char HX711Multi::update()
{
	if (!isReady())
	{
		return -1;
	}
	byte arrOffset = 0;
	byte retChannel = 1;
	if (CELLS == 2)
	{
		if (_nextGain == 2) {
			_nextGain = GAIN;
			retChannel = 2;
			arrOffset = MAXAVG;
		} else {
			_nextGain = 2;
			retChannel = 1;
		}
	}
	for (byte n = MAXAVG - 1; n > 0; n--)
	{
		values[n + arrOffset] = values[n - 1 + arrOffset];
	}
	values[arrOffset] = read();
	delay(0);   // Feeding WatchDog
	return retChannel;
}

void HX711Multi::setGain(byte gain0)
{
	switch (gain0)
	{
	case 128: // channel A, gain factor 128
		GAIN = 1;
		break;
	case 64: // channel A, gain factor 64
		GAIN = 3;
		break;
	case 32: // channel B, gain factor 32
		GAIN = 2;
		break;
	default:
		GAIN = 1;
	}

	_nextGain = GAIN;
	digitalWrite(PD_SCK, LOW);
}

// Reading a long integer value with Zero at 0 int
long HX711Multi::read()
{

	long data;
	digitalWrite(PD_SCK, HIGH);
#if FAST_CPU
	delayMicroseconds(1);
#endif
	if (digitalRead(DOUT))
	{ // High bit at Sign, means negative number
		data = -1;
	}
	else
	{
		data = 0;
	}
	digitalWrite(PD_SCK, LOW);
#if FAST_CPU
	delayMicroseconds(1);
#endif

	// pulse the clock pin 23 remaining times to read the data
	for (byte i = 0; i < 23; i++)
	{
		digitalWrite(PD_SCK, HIGH);
#if FAST_CPU
		delayMicroseconds(1);
#endif
		data *= 2;
		if (digitalRead(DOUT))
		{
			data += 1;
		}
		digitalWrite(PD_SCK, LOW);
#if FAST_CPU
		delayMicroseconds(1);
#endif
	}

	// set the channel and the gain factor for the next reading using the clock pin
	for (int i = 0; i < _nextGain; i++)
	{
		digitalWrite(PD_SCK, HIGH);
		digitalWrite(PD_SCK, LOW);
	}

	return data;
}

double HX711Multi::getAverage(byte cell, byte times)
{
	long sum = 0;
	byte arrOffset = 0;
	if (cell == 2) {
		arrOffset = MAXAVG;
	}
	if (times > MAXAVG)
	{
		times = MAXAVG;
	}
	for (byte i = 0; i < times; i++)
	{
		sum += values[i+arrOffset];
	}
	return (double)sum / times;
}

double HX711Multi::getValue(byte cell, byte times)
{
	if (cell == 2) {
		return getAverage(cell, times) - (double)OFFSET2;
	} else {
		return getAverage(cell, times) - (double)OFFSET1;
	}
}

float HX711Multi::getUnits(byte cell, byte times)
{
	if (cell == 2) {
		return getValue(cell, times) / SCALE2;
	} else {
		return getValue(cell, times) / SCALE1;
	}
}

void HX711Multi::tare(byte cell, byte times)
{
	if (times > MAXAVG)
	{
		times = MAXAVG;
	}
	for (byte n = 0; n < times*CELLS; n++)
	{
		while (update() == -1) {
			;
		}
	}

	double sum = getAverage(cell, times);
	setOffset(cell, sum);
}

void HX711Multi::setScale(byte cell, float scale)
{
	if (cell == 1) {
		SCALE1 = scale;
	}
	if (cell == 2) {
		SCALE2 = scale;
	}
}

void HX711Multi::setOffset(byte cell, long offset)
{
	if (cell == 1) {
		OFFSET1 = offset;
	}
	if (cell == 2) {
		OFFSET2 = offset;
	}
}

void HX711Multi::powerDown()
{
	digitalWrite(PD_SCK, LOW);
	digitalWrite(PD_SCK, HIGH);
}

void HX711Multi::powerUp()
{
	digitalWrite(PD_SCK, LOW);
}