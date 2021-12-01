/* 
 * obstacle.h
 * contains the definition of the obstacle struct
 */

#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <Arduino.h>

/* The photoresistors are nominally placed on the corners of a 8cm*1.5cm
   rectangle, measuring from the center of the photoresistor. */

typedef struct {
	/* 1-dimensional vector representing the obstacle's horizontal
	   velocity, in mm/s. Alternatively, you can think of it as um/ms,
	   since the onboard millisecond timer measures milliseconds. */
	int16_t velocity;
	/* millisecond timestamp when the obstacle entered the field of
	   view of the arduino system */
	uint32_t entranceTime;
	/* expected duration from when the obstacle enters the field of view
	   to when the dinosaur should jump to dodge the obstacle, in milli-
	   seconds. */
	uint32_t expectedDuration;
	int8_t width;	// width of the obstacle in millimeters
} Obstacle;

/* sets the velocity for all 4 Obstacle structs in obstacles to -1 to
   mark them as inactive Obstacles */
void initObstacles(Obstacle obstacles[4]);

#endif // OBSTACLE_H
