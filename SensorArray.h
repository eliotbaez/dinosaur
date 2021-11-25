/*
 * SensorArray.h
 * declares the photoresistor utility structs and helper functions
 */

#ifndef SENSOR_ARRAY_H
#define SENSOR_ARRAY_H

/*
 *** NUMBERING CONVENTION ***
 *
 * There will be an array of 4 SensorArray structs in this project. To
 * save myself some confusion, I will define a set order for indexing
 * each of these photoresistors in a square shaped array.
 *
 * 		2		0
 * 		TL		TR
 *
 *		3		1
 *		BL		BR
 * 
 * The reason for this numbering convention is because the two rightmost
 * photoresistors will be read first, and the leftmost pair will be read
 * second. Historically (in the context of this project), the top photo-
 * resistor was used as the reference for calibration. Thus, the upper
 * photoresistor in each pair takes precedence.
 */

/* struct storing the pin numbers of the array of photoresistors */
typedef struct {
	const uint8_t topLeft;
	const uint8_t topRight;
	const uint8_t bottmLeft;
	const uint8_t bottomRight;
} SensorArray;

/* struct to make it easier to keep track of calibraion readings from
   the photoresistors */
typedef struct {
	int low;
	int high;
} CalibrationPair;

/* expects a pointer to an array of 4 CalibrationPair structs, and four
   analog pin numbers */
void calibrate(CalibrationPair pairs[], const SensorArray *pins);

#endif // SENSOR_ARRAY_H
