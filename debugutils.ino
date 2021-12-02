/* debug utilities */

#include "Config.h"
#include "Obstacle.h"
#include "SensorArray.h"
#include "debugutils.h"

void dumpSensorReadings_debug(SensorArray *sensors, CalibrationPair transform[4]) {
	long tr = analogExpRead(sensors->topRight);
	long br = analogExpRead(sensors->bottomRight);
	long tl = analogExpRead(sensors->topLeft);
	long bl = analogExpRead(sensors->bottomLeft);

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

void dumpObstacleData_debug(Obstacle *obstacle) {
	Serial.println("\nObstacle stored data:");
	
	Serial.print("velocity:\t\t");
	Serial.print(obstacle->velocity);
	Serial.println(" mm/s");

	Serial.print("entranceTime:\t\t");
	Serial.print(obstacle->entranceTime);
	Serial.println(" ms");

	Serial.print("expectedDuration:\t");
	Serial.print(obstacle->expectedDuration);
	Serial.println(" ms");
	
	Serial.print("width:\t\t\t");
	Serial.print(obstacle->width);
	Serial.println(" mm\n");
}
