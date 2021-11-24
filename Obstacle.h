/* 
 * obstacle.h
 * contains the definition of the obstacle struct
 */

#ifndef OBSTACLE_H
#define OBSTACLE_H

typedef struct {
	/* 1-dimensional vector representint the obstacle's horizontal
	   velocity, in mm/s. Alternatively, you can think of it as um/ms,
	   since the onboard millisecond timer measures milliseconds. */
	int horizontalVelocity;
	/* millisecond timestamp when the obstacle entered the field of
	   view of the arduino system */
	unsigned long entranceTime;
	short width;	// width of the obstacle in millimeters
} Obstacle;

#endif // OBSTACLE_H
