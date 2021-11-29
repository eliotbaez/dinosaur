#include "SensorArray.h"
#include "debugutils.h"

void calibrate(SensorArray *pins, CalibrationPair pairs[4]) {
	Serial.println("Take sample 1 in 2 seconds!");
	digitalWrite(LED_BUILTIN, HIGH);
	delay(2000);
	digitalWrite(LED_BUILTIN, LOW);
	/* See SensorArray.h for information about the photoresistor
	   numbering convention in use */
	pairs[0].low = analogRead(pins->topRight);
	pairs[1].low = analogRead(pins->bottomRight);
	pairs[2].low = analogRead(pins->topLeft);
	pairs[3].low = analogRead(pins->bottomLeft);

	Serial.println("Take sample 2 in 2 seconds!");
	digitalWrite(LED_BUILTIN, HIGH);
	delay(2000);
	digitalWrite(LED_BUILTIN, LOW);
	pairs[0].high = analogRead(pins->topRight);
	pairs[1].high = analogRead(pins->bottomRight);
	pairs[2].high = analogRead(pins->topLeft);
	pairs[3].high = analogRead(pins->bottomLeft);

	Serial.println("Calibrated.");
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);

	dumpSensorReadings(pins, pairs);
}
