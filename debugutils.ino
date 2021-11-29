/* debug utilities */

#include "Config.h"
#include "Obstacle.h"
#include "SensorArray.h"
#include "debugutils.h"

void dumpSensorReadings_debug(SensorArray *sensors, CalibrationPair transform[4]) {
	int tr = analogRead(sensors->topRight);
	int br = analogRead(sensors->bottomRight);
	int tl = analogRead(sensors->topLeft);
	int bl = analogRead(sensors->bottomLeft);

	Serial.println("\nRaw sensor data:");
	Serial.print(tl); Serial.print('\t');
	Serial.println(tr); 
	Serial.print(bl); Serial.print('\t');
	Serial.println(br);
	Serial.println();
	
	Serial.println("Adjusted sensor data:");
	Serial.print(
		map( tl,
			transform[2].low, transform[2].high,
			transform[0].low, transform[0].high ));
	Serial.print('\t');
	Serial.println(tr);
	Serial.print(
		map( bl,
			transform[3].low, transform[3].high,
			transform[0].low, transform[0].high ));
	Serial.print('\t');
	Serial.println(
		map( br,
			transform[1].low, transform[1].high,
			transform[0].low, transform[0].high ));
	Serial.println("Sensor dump finished.\n");
}
