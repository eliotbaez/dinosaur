/*
 * Obstacle.ino
 * defines functions used with Obstacle structs
 */

#include "Obstacle.h"

/* Set the velocity to all Obstacle structs to a negative number, to
   show that they are not active obstacles. Also initialize everything
   else for good measure. */
void initObstacles(Obstacle obstacles[4]) {
	int i;

	for (i = 0; i < 3; i++) {
		obstacles[i].velocity = -1;
		obstacles[i].entranceTime = 0;
		obstacles[i].expectedDuration = 0;
		obstacles[i].width = 0;
	}
	Serial.println("Obstacle data initialized.");
}
