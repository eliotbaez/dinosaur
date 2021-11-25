/*
 * Chrome dinosaur game cheating device 
 * by Eliot Baez
 *
 * This sketch allows the arduino to be used as a cheating device in
 * Chrome dinosaur game. It is designed to be used with an accompanying
 * python script to run on the host machine, but you can hack around
 * with it to use a servo to press the spacebar instead.
 * 
 * Special thanks to Dylan Leifer-Ives for the idea of using 4
 * photoresistors to track the horizontal speed of the obstacles on the
 * screen, as well as for lending me two extra photoresistors.
 */

#include "SensorArray.h"
#include "Obstacle.h"

// pins corresponding to the photoresistor array
SensorArray sensors = {
	.topLeft =		14,
	.topRight =		15,
	.bottomLeft =	16,
	.bottomRight =	17
};

// one calibration pair for each sensor in the array
CalibrationPair transform[4];

// circular buffer of obstacle structs
Obstacle obstacles[3];
uint8_t obstacleIndex = 0;

const short int SENSOR_WIDTH_MM =	20;
const int SENSOR_WIDTH_UM =		20000;
const short int MARGIN =		5;
const short int JUMP_SHORT = 	1;
const short int JUMP_LONG = 	2;

int threshold;
long waitMillis;
long lastCommand;

void jump(int ms);
void duck(void);
int game(int target);
int menu(void);
int listen(SensorArray *pins);

void setup() {
	/* put your setup code here, to run once: */
	Serial.begin (19200);
	pinMode (BOTTOM_INPUT, INPUT);
	pinMode (TOP_INPUT, INPUT);
	
	calibrate(transform, &sensors);
	threshold = 7;
}

void loop() {
	game(1000);
}

int game(int target) {
	int tlBrightness, trBrightness, blBrightness, brBrightness;
	int differenceRight, differenceLeft;
	unsigned long timeBuf;
	int score;
	
	/* For arbitrary personal reasons, the entire array of photo-
	   resistors is calibrated against the top right photoresistor. Also
	   see SensorArray.h for information about the sensor numbering
	   convention in use. */
	trBrightness = analogRead(sensors.topRight);
	brBrightness = map( analogRead(sensors.bottomRight),
		transform[1].low, transform[1].high,
		transform[0].low, transform[0].high );
	differenceRight = abs(trBrightness - brBrightness);

	if (differenceRight > threshold) {
		// obstacle detecred, now measure the speed
		obstacles[obstacleIndex].entranceTime = millis();
		while (true) {
			/* continually calculate the brightness difference between
			   the left photoresistor pair, waiting for the obstacle to
			   pass by the left edge of the sensors */
			tlBrightness = map( analogRead(sensors.topLeft),
				transform[2].low, transform[2].high,
				transform[0].low, transform[0],high );
			blBrightness = map( analogRead(sensors.bottomLeft),
				transform[3].low, transform[3].high,
				transform[0].low, transform[0].high );
			differenceLeft = abs(tlBrightness - blBrightness);

			if (differenceLeft > threshold) {
				// set timeBuf to the current time
				timeBuf = millis();
				obstacles[obstacleIndex].velocity
					= SENSOR_WIDTH_UM / (timeBuf - obstacles[obstacleIndex].entranceTime);
				break;
			}
		}

		// now find the width of the obstacle
		while (true) {
			tlBrightness = map( analogRead(sensors.topLeft),
				transform[2].low, transform[2].high,
				transform[0].low, transform[0],high );
			blBrightness = map( analogRead(sensors.bottomLeft),
				transform[3].low, transform[3].high,
				transform[0].low, transform[0].high );
			differenceLeft = abs(tlBrightness - blBrightness);

			if (differenceLeft > threshold) {
				/* set timeBuf equal to the duration for which the
				   obstacle was in the view of a single sensor pair */
				timeBuf -= millis();
				/* Using the standard distance formula:
				   d = r*t
				   um = (um / ms) * (ms)
				   width (mm) = (velocity * duration) / 1000 */
				obstacles[obstacleIndex].width
					= (unsigned long)obstacles[obstacleIndex].velocity * timeBuf / 1000;
				break;
			}
		}
	}
	
	if (millis() - lastCommand > waitMillis) {
		if (difference > threshold) {
			//Serial.println(difference);
			jump(JUMP_LONG);
			lastCommand = millis();
			waitMillis = 200;
		}
	}
	return score;
}

int listen(SensorArray *pins) {
	}


void jump(int type) {
	if (type == JUMP_SHORT) {
		Serial.print("1\n");
	} else if (type == JUMP_LONG) {
		Serial.print("2\n");
	}
}

void duck(void) {
	Serial.print("Duck\n");
}

