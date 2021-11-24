#include "calibrate.h"

void calibrate(CalibrationPair pairs[],
	int a, int b, int c, int d) {
	Serial.println("Take sample 1 in 2 seconds!");
	digitalWrite(LED_BUILTIN, HIGH);
	delay(2000);
	digitalWrite(LED_BUILTIN, LOW);
	pairs[0].low = analogRead(a);
	pairs[1].low = analogRead(b);
	pairs[2].low = analogRead(c);
	pairs[3].low = analogRead(d);
	Serial.println("Take sample 2 in 2 seconds!");
	digitalWrite(LED_BUILTIN, HIGH);
	delay(2000);
	digitalWrite(LED_BUILTIN, LOW);
	pairs[0].high = analogRead(a);
	pairs[1].high = analogRead(b);
	pairs[2].high = analogRead(c);
	pairs[3].high = analogRead(d);
	Serial.println("Calibrated.");
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);
}
