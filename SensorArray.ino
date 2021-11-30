#include "SensorArray.h"
#include "debugutils.h"

void calibrate(SensorArray *pins, CalibrationPair pairs[4]) {
	Serial.println("Take sample 1 in 2 seconds!");
	blink(50);
	delay(950);
	blink(50);
	delay(950);
	/* See SensorArray.h for information about the photoresistor
	   numbering convention in use */
	pairs[0].low = analogRead(pins->topRight);
	pairs[1].low = analogRead(pins->bottomRight);
	pairs[2].low = analogRead(pins->topLeft);
	pairs[3].low = analogRead(pins->bottomLeft);
	Serial.println("Done.");
	
	Serial.println("Take sample 2 in 2 seconds!");
	blink(50);
	delay(950);
	blink(50);
	delay(950);
	pairs[0].high = analogRead(pins->topRight);
	pairs[1].high = analogRead(pins->bottomRight);
	pairs[2].high = analogRead(pins->topLeft);
	pairs[3].high = analogRead(pins->bottomLeft);
	Serial.println("Done.");

	Serial.println("Calibrated.\n");
	dumpSensorReadings(pins, pairs);
	blink(200);
}

int getNoiseFloor(SensorArray *pins, CalibrationPair pairs[4]) {
	/* sample the difference between the photoresistor pairs over a
	   period of one second and use the maximum difference as the
	   noise floor for difference readings */
	int i;
	int noiseFloor = -1;
	int difference;
	int topBrightness, bottomBrightness;
	
	Serial.println("Taking noise sample in one second!");
	blink(50);
	delay(950);

	Serial.println("Taking noise sample...");
	for (i = 0; i < 1000; i++) {
		topBrightness = analogRead(pins->topRight);
		bottomBrightness = map( analogRead(pins->bottomLeft),
			pairs[3].low, pairs[3].high,
			pairs[0].low, pairs[0].high );
		difference = abs(topBrightness - bottomBrightness);
		noiseFloor = max(noiseFloor, difference);

		topBrightness = map( analogRead(pins->topLeft),
			pairs[2].low, pairs[2].high,
			pairs[0].low, pairs[0].high );
		bottomBrightness = map( analogRead(pins->bottomLeft),
			pairs[3].low, pairs[3].high,
			pairs[0].low, pairs[0].high );
		difference = abs(topBrightness - bottomBrightness);
		noiseFloor - max(noiseFloor, difference);
		delay(1);
	}
	Serial.println("Done.");

	Serial.print("Noise floor is ");
	Serial.println(noiseFloor);
	Serial.println();
	blink(200);

	return noiseFloor;
}

void blink(int ms) {
	digitalWrite(LED_BUILTIN, HIGH);
	delay(ms);
	digitalWrite(LED_BUILTIN, LOW);
}
