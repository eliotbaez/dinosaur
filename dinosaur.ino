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

const int BOTTOM_INPUT =	14;
const int TOP_INPUT	=		15;
const int MARGIN =			5;
const int JUMP_SHORT = 		1;
const int JUMP_LONG	= 		2;

int threshold;
long waitMillis;
long lastCommand;

/* 
 * There is a certain amount of variation between the readings of one
 * photoresistor to the next. These values are pre-tuned to transform
 * the readings of the upper photoresistor to match those of the lower
 * photoresistor.
 */
int transform[4];

void jump(int ms);
void duck(void);
int game(int points);
int menu(void);
void calibrate(void);


void setup() {
	/* put your setup code here, to run once: */
	Serial.begin (19200);
	pinMode (BOTTOM_INPUT, INPUT);
	pinMode (TOP_INPUT, INPUT);
	
	calibrate();
	threshold = 7;
}

void loop() {
	// put your main code here, to run repeatedly:
	int topBrightness;
	int bottomBrightness;
	int difference; 
	
	topBrightness = analogRead(TOP_INPUT);
	bottomBrightness = analogRead(BOTTOM_INPUT);
	// adjust bottom brightness
	bottomBrightness = map( analogRead(BOTTOM_INPUT),
		transform[1], transform[3],
		transform[0], transform[2] );
	difference = abs(topBrightness - bottomBrightness);

	if (millis() - lastCommand > waitMillis) {
		if (difference > threshold) {
			//Serial.println(difference);
			jump(JUMP_LONG);
			lastCommand = millis();
			waitMillis = 200;
		}
	}
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

void calibrate(void) {
	Serial.println("Take sample 1 in 2 seconds!");
	digitalWrite(LED_BUILTIN, HIGH);
	delay(2000);
	digitalWrite(LED_BUILTIN, LOW);
	transform[0] = analogRead(TOP_INPUT);
	transform[1] = analogRead(BOTTOM_INPUT);
	Serial.println("Take sample 2 in 2 seconds!");
	digitalWrite(LED_BUILTIN, HIGH);
	delay(2000);
	digitalWrite(LED_BUILTIN, LOW);
	transform[2] = analogRead(TOP_INPUT);
	transform[3] = analogRead(BOTTOM_INPUT);
	Serial.println("Calibrated.");
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);
}
