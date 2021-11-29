/*
 * Config.h
 * contains declaration of Config struct
 */

#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
	/* width in millimeters of the region on the screen between the
	   rightmost sensor pair and the dinosaur */
	int regionWidth;
	long targetScore;
} Config;

#endif // CONFIG_H
