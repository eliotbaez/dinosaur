/*
 * Chrome dinosaur game cheating device 
 * by Eliot Baez
 *
 * This project allows the arduino to be used as a cheating device in
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
#include "Config.h"
#define DINOSAUR_DEBUG
#include "debugutils.h"

/****** GLOBAL STRUCTS ******/

// pins corresponding to the photoresistor array
SensorArray sensors = {
	.topLeft =		14,
	.topRight =		15,
	.bottomLeft =	16,
	.bottomRight =	17
	/* Should I number these in accordance to the numbering convention?
	   maybe. For now, I'll leave it like this and see if it makes sense
	   to. The only thing really affected by the pin order would be how
	   simple it is to wire up. We'll see. */
};

// one calibration pair for each sensor in the array
CalibrationPair transform[4];

// config struct to store our settings
Config config;

// circular buffer of obstacle structs
Obstacle obstacles[3];
int8_t obstacleIndex;

/****** CONSTANTS ******/

const short SENSOR_WIDTH_MM =	20;
const int SENSOR_WIDTH_UM = 20000;
const short MARGIN =		5;
const short JUMP_SHORT = 	1;
const short JUMP_LONG = 	2;

/****** GLOBAL VARIABLES ******/

int threshold;
long waitMillis;
long lastCommand;

/****** FUNCTION PROTOTYPES ******/

void jump(int type = JUMP_LONG);
void duck(void);
int game(int target);
int menu(void);
int checkObstacles();
void getConfig(void);


void setup() {
	/* put your setup code here, to run once: */
	Serial.begin(19200);
	pinMode(sensors.topRight, INPUT);
	pinMode(sensors.bottomRight, INPUT);
	pinMode(sensors.topLeft, INPUT);
	pinMode(sensors.bottomLeft, INPUT);
	
	/* set the velocity to all Obstacle structs to a negative number,
	   to show that they are not active obstacles */
	for (obstacleIndex = 0; obstacleIndex < 3; obstacleIndex++)
		obstacles[obstacleIndex].velocity = -1;
	
	// reset this index so everything else runs smoothly
	obstacleIndex = 0;

	calibrate(&sensors, transform);
	getConfig();
	threshold = 7;
}

void loop() {
	game(0);
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
		// obstacle detected, now measure the speed
		obstacles[obstacleIndex].entranceTime = millis();
		while (true) {
			/* continually calculate the brightness difference between
			   the left photoresistor pair, waiting for the obstacle to
			   pass by the left edge of the sensors */
			tlBrightness = map( analogRead(sensors.topLeft),
				transform[2].low, transform[2].high,
				transform[0].low, transform[0].high );
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
				transform[0].low, transform[0].high );
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

		dumpObstacleData(&obstacles[obstacleIndex]);
		// finally, increment the index
		obstacleIndex = (obstacleIndex + 1) % 3;
	}
	
	checkObstacles();
	return score;
}

void getConfig(void) {
	config.regionWidth = 300;
	config.targetScore = -1;
}

int checkObstacles() {
	static short i = 0;
	short result = 0;
	
	// check if the Obstacle is not invalid...
	if (!obstacles[i].velocity < 0) {
		// the obstacle is valid, now check it if's time to jump yet
		if (millis() - obstacles[i].entranceTime >= obstacles[i].expectedDuration) {
			// it's time to jump!
			jump(JUMP_LONG);
			result = 2;

			// now clear the Obstacle struct
			obstacles[i].velocity = -1;
			i = (i + 1) % 3;
		}
	}
	return result;
}


void jump(int type = JUMP_LONG) {
	if (type == JUMP_SHORT) {
		Serial.print("1\n");
	} else if (type == JUMP_LONG) {
		Serial.print("2\n");
	}
}

void duck(void) {
	Serial.print("Duck\n");
}

