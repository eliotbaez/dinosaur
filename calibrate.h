/*
 * calibrate.h
 * declares the photoresistor calibration function
 */

#ifndef CALIBRATE_H
#define CALIBRATE_H

typedef struct {
	int low;
	int high;
} CalibrationPair;

void calibrate(CalibrationPair pairs[],
	int a, int b, int c, int d);

#endif // CALIBRATE_H
