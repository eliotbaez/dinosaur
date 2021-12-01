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

#include <Arduino.h>

/* I've determined that the remote I have uses the NEC protocol. I'll
   define this flag before #include'ing <IRremote.h> to save some DRAM
   and processor time. For more information I recommend reading the
   documentation at 
   https://github.com/Arduino-IRremote/Arduino-IRremote */
#define DECODE_NEC
#include <IRremote.h>

#include "SensorArray.h"
#include "Obstacle.h"
#include "Config.h"
#include "remotecommands.h"
#include "debugutils.h"

/* using a define here because there are conditional preprocessor
   directives inside my ui.h file */
#define IR_TIMEOUT 280UL
#include "ui.h"

/****** CONSTANTS ******/

const unsigned OBSTACLE_TIMEOUT =	300;
const unsigned long SENSOR_WIDTH_UM =	80000;
const short IR_RECV_PIN =	2;
const short MARGIN =		5;
const short JUMP_SHORT =	1;
const short JUMP_LONG =		2;

/****** GLOBAL STRUCTS/OBJECTS ******/

// pins corresponding to the photoresistor array
SensorArray sensors = {
	.topRight =		14,
	.bottomRight =	15,
	.topLeft =		16,
	.bottomLeft =	17
};

// one calibration pair for each sensor in the array
CalibrationPair transform[4];

// config struct to store our settings
Config config;

// circular buffer of obstacle structs
Obstacle obstacles[3];

IRrecv irrecv(IR_RECV_PIN);

/****** GLOBAL VARIABLES ******/

// used by game() to access Obstacle structs in the array
int8_t obstacleIndex;
// used strictly inside the Obstacle checking ISR
volatile int8_t obstacleCheckIndex;

int threshold;
long waitMillis;

// IR remote specific stuff
unsigned int recvCmd = 0;
bool active = false;
bool newCommand = false;
unsigned long msLastSignal = 0;

/****** FUNCTION PROTOTYPES ******/

void jump(int type = JUMP_LONG);
void duck(void);
int game(int target);
int menu(void);
int checkObstacles();
void getConfig(void);

/****** FUNCTION DEFINITIONS ******/

void setup() {
	/* put your setup code here, to run once: */
	Serial.begin(19200);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(sensors.topRight, INPUT);
	pinMode(sensors.bottomRight, INPUT);
	pinMode(sensors.topLeft, INPUT);
	pinMode(sensors.bottomLeft, INPUT);
	
	/* set the velocity to all Obstacle structs to a negative number,
	   to show that they are not active obstacles */
	for (obstacleIndex = 0; obstacleIndex < 3; obstacleIndex++) {
		obstacles[obstacleIndex].velocity = -1;
		obstacles[obstacleIndex].entranceTime = 0;
		obstacles[obstacleIndex].expectedDuration = 0;
		obstacles[obstacleIndex].width = 0;
	}
	
	// reset this index so everything else runs smoothly
	obstacleIndex = 0;
	obstacleCheckIndex = 0;

	/* We will take advantage of the fact that timer0 is set up to run
	   at 1kHz by default to interrupt our code every millisecond to
	   check the Obstacle array and see if it's time to jump yet.
	   (today I learned that the millisecond timer is actually closer
	   to 976.5625Hz because 250 != 256)

	   Based on example code from
	   https://learn.adafruit.com/multi-tasking-the-arduino-part-2/timers */
	noInterrupts();
	/* set the compare match register to some arbitrary number that
	   isn't 0 or 255 */
	OCR0A = 150;
	// enable timer compare interrupts
	TIMSK0 |= (1 << OCIE0A);
	interrupts();
	
	// housekeeping tasks
	irrecv.enableIRIn();
	active = true;

	calibrate(&sensors, transform, &irrecv);
	threshold = 2 + 2 * getNoiseFloor(&sensors, transform, &irrecv);
	getConfig();
}

// the ISR for the interrupts we enabled in setup()
ISR(TIMER0_COMPA_vect) { // COMPA = compare against A?
	checkObstacles();
}

void loop() {
	static int status;
	int target = -1;
#if 0
	dumpSensorReadings(&sensors, transform);
	delay(500);
	return;
#endif

#if 0
	int trBrightness = analogRead(sensors.topRight);
	int brBrightness = map( analogRead(sensors.bottomRight),
		transform[1].low, transform[1].high,
		transform[0].low, transform[0].high );
	int differenceRight = abs(trBrightness - brBrightness);
	Serial.println(differenceRight);
	delay(100);
	return;
#endif
#if 0
	if (millis() - msLastSignal > IR_TIMEOUT) {
		if (!active) {
			irrecv.resume();
			active = true;
		}

		if (irrecv.decode()) {
			active = false;
			recvCmd = irrecv.decodedIRData.command;
			Serial.println(recvCmd);
			if (recvCmd != UNKNOWN) {
				msLastSignal = millis();
			}
		}
	}
#endif 
	switch (mainMenu(&irrecv)) {
	case 0:
		// soft reset
		// soft reset functin
		break;
	case 1:
		// start game
		status = game(0);
		break;
	case 2:
		// configure...
		//configurationMenu(&irrecv, &config);
		break;
	case 3:
		// calibrate...
		switch(calibrationMenu(&irrecv)) {
		case 0:
			// go back
			break;
		case 1:
			calibrate(&sensors, transform, &irrecv);
			break;
		case 2:
			threshold = 2 + 2 * getNoiseFloor(&sensors, transform, &irrecv);
		}
		break;
	case 4:
		// hard reset! sets the instruction pointer back to 0
		// reset function
		;
	}

	//status = awaitRemoteCommand(&irrecv);
	//Serial.println(status);
}

int game(int target) {
	int tlBrightness, trBrightness, blBrightness, brBrightness;
	int differenceRight, differenceLeft;
	unsigned long timeBuf;
	int status;
	int score = 0;
	//unsigned long 


	while (true) {
		/* Don't do anything if it's been less than OBSTACLE_TIMEOUT ms
		   since the last obstacle was observed. This is similar to
		   debouncing a button, by ignoring readings for a certain
		   amount of time. 

		   Here, timeBuf is set to the entranceTime of the previously
		   written Obstacle struct. I would use the modulus operator
		   here, but the math doesn't work quite how I would've wanted
		   it to with negative numbers, so enjoy this ternary operator
		   instead. */;
		timeBuf = obstacles[ (obstacleIndex == 0)
				? 2
				: obstacleIndex - 1 ].entranceTime;
		/* Then, set timeBuf equal to the time that has passed since the
		   last Obstacle was seen entering the screen */
		timeBuf = millis() - timeBuf;
		if (timeBuf < OBSTACLE_TIMEOUT) {
			continue;
		}

		/* For arbitrary personal reasons, the entire array of photo-
		   resistors is calibrated against the top right photoresistor.
		   Also see SensorArray.h for information about the sensor
		   numbering convention in use. */
		trBrightness = analogRead(sensors.topRight);
		brBrightness = map( analogRead(sensors.bottomRight),
			transform[1].low, transform[1].high,
			transform[0].low, transform[0].high );
		differenceRight = abs(trBrightness - brBrightness);
		
		if (differenceRight > threshold) {
			// obstacle detected, now measure the speed
			obstacles[obstacleIndex].entranceTime = millis();
			blink(1);
			while (true) {
				/* continually calculate the brightness difference
				   between the left photoresistor pair, waiting for the
				   obstacle to pass by the left edge of the sensors */
				tlBrightness = map( analogRead(sensors.topLeft),
					transform[2].low, transform[2].high,
					transform[0].low, transform[0].high );
				blBrightness = map( analogRead(sensors.bottomLeft),
					transform[3].low, transform[3].high,
					transform[0].low, transform[0].high );
				differenceLeft = abs(tlBrightness - blBrightness);
	
				if (differenceLeft > threshold) {
					// calculate obstacle velocity
					timeBuf = millis();
					obstacles[obstacleIndex].velocity
						= SENSOR_WIDTH_UM
						/ (timeBuf - obstacles[obstacleIndex].entranceTime);

					/* d = r*t
					   t = d/r
					   ms = um / (um / ms) 
					   ms = 1000 * mm / (um / ms) */
					obstacles[obstacleIndex].expectedDuration
					// jump 50mm ahead of the obstacle
						= 1000UL * (config.regionWidth - 50)
						/ obstacles[obstacleIndex].velocity
						- 400;
					break;
				}
			}

#ifdef USE_WIDTH
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
						= (unsigned long)obstacles[obstacleIndex].velocity
						* timeBuf / 1000;
					break;
				}
			}
#endif // USE_WIDTH

			dumpObstacleData(&obstacles[obstacleIndex]);
			// finally, increment the index
			obstacleIndex = (obstacleIndex + 1) % 3;
		}
	}
	return score;
}

void getConfig(void) {
	config.regionWidth = 300;
	config.targetScore = -1;
}

int checkObstacles() {
	short result = 0;
	
	// check if the Obstacle is not invalid...
	if (!(obstacles[obstacleCheckIndex].velocity < 0)) {
		// the obstacle is valid, now check it if's time to jump yet
		if (millis() - obstacles[obstacleCheckIndex].entranceTime
				>= obstacles[obstacleCheckIndex].expectedDuration) {
			// it's time to jump!
			jump(JUMP_LONG);
			result = 2;

			// now clear the Obstacle struct
			obstacles[obstacleCheckIndex].velocity = -1;
			obstacleCheckIndex = (obstacleCheckIndex + 1) % 3;
		}
	}
	return result;
}


void jump(int type = JUMP_LONG) {
	if (type == JUMP_SHORT) {
		Serial.print("1\n");
	} else if (type == JUMP_LONG) {
		Serial.print("\n2\n");
	}
}

void duck(void) {
	Serial.print("Duck\n");
}

