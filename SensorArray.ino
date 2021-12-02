#include <IRremote.h>
#include "SensorArray.h"
#include "debugutils.h"
#include "remotecommands.h"
#include "ui.h"

/* Returns the rounded exponent of the analogRead of pin, adjusted to
   fit the needs of this project */
long analogExpRead(int pin) {
	return (long)round( exp((double)analogRead(pin) / 128) );
}

void calibrate(SensorArray *pins, CalibrationPair pairs[4], IRrecv *recv) {
	Serial.println("Press Play to take sample 1...");
	while (awaitRemoteCommand(recv) != CMD_PLAY_PAUSE) ; // wait

	/* See SensorArray.h for information about the photoresistor
	   numbering convention in use */
	pairs[0].low = analogExpRead(pins->topRight);
	pairs[1].low = analogExpRead(pins->bottomRight);
	pairs[2].low = analogExpRead(pins->topLeft);
	pairs[3].low = analogExpRead(pins->bottomLeft);
	Serial.println("Done.");
	delay(100);
	
	Serial.println("Press Play to take sample 2...");
	while (awaitRemoteCommand(recv) != CMD_PLAY_PAUSE) ; // wait
	pairs[0].high = analogExpRead(pins->topRight);
	pairs[1].high = analogExpRead(pins->bottomRight);
	pairs[2].high = analogExpRead(pins->topLeft);
	pairs[3].high = analogExpRead(pins->bottomLeft);
	Serial.println("Done.");
	
	Serial.println("Calibrated.\n");
	dumpSensorReadings(pins, pairs);
	delay(100);
	blink(50);
}

long getNoiseFloor(SensorArray *pins, CalibrationPair pairs[4], IRrecv *recv) {
	/* sample the difference between the photoresistor pairs over a
	   period of one second and use the maximum difference as the
	   noise floor for difference readings */
	int i;
	long noiseFloor = -1;
	long difference;
	long topBrightness, bottomBrightness;
	
	Serial.println("Press play to take noise sample...");
	while (awaitRemoteCommand(recv) != CMD_PLAY_PAUSE) ; // wait

	Serial.println("Taking noise sample...");
	for (i = 0; i < 1000; i++) {
		topBrightness = analogExpRead(pins->topRight);
		bottomBrightness = map( analogExpRead(pins->bottomRight),
			pairs[1].low, pairs[1].high,
			pairs[0].low, pairs[0].high );
		difference = abs(topBrightness - bottomBrightness);
		noiseFloor = max(noiseFloor, difference);

		topBrightness = map( analogExpRead(pins->topLeft),
			pairs[2].low, pairs[2].high,
			pairs[0].low, pairs[0].high );
		bottomBrightness = map( analogExpRead(pins->bottomLeft),
			pairs[3].low, pairs[3].high,
			pairs[0].low, pairs[0].high );
		difference = abs(topBrightness - bottomBrightness);
		noiseFloor = max(noiseFloor, difference);
		delay(1);
	}
	Serial.println("Done.");

	Serial.print("Noise floor is ");
	Serial.println(noiseFloor);
	Serial.println();
	blink(50);

	return noiseFloor;
}

void blink(int ms) {
	digitalWrite(LED_BUILTIN, HIGH);
	delay(ms);
	digitalWrite(LED_BUILTIN, LOW);
}
