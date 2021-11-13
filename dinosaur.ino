/*
 * Chrome dinosaur game cheating device 
 * by Eliot Baez
 * This sketch allows the arduino to be used as a cheating device in
 * Chrome dinosaur game. It is designed to be used with an accompanying
 * python script to run on the host machine, but you can hack around
 * with it to use a servo to press the spacebar instead.
 */

const int BOTTOM_INPUT = 14;
const int TOP_INPUT = 15;
const int  MARGIN  = 5;

int threshold;
int daylight;	/* standard brightness of daylight */

/* 
 * There is a certain amount of variation between the readings of one
 * photoresistor to the next. These values are pre-tuned to transform
 * the readings of the upper photoresistor to match those of the lower
 * photoresistor.
 */
double scale = 1.195;
double offset = 6.02;

/* The transform function that uses our parameters above */
int transform (int x) {
	double y;
	y = (double)x * scale;
	y += offset;
	return (int)round (y);
}

/* circular buffers :D */
int lastTopReadings[50];
int lastBottomReadings[50];
uint8_t lastElementUsed = 0;

int mean50 (int* top, int* bottom) {
	unsigned long sum = 0;
	for (int i = 0; i < 50; i++) {
		sum += top[i];
		sum += bottom[i];
	}
	return sum / 100;
}

void setup() {
	/* put your setup code here, to run once: */
	Serial.begin (9600);
	pinMode (BOTTOM_INPUT, INPUT);
	pinMode (TOP_INPUT, INPUT);

	/* collect 100 light samples over 1 second */
	Serial.print ("Calibrating... please wait 1000ms\n");
	for (int count = 0; count < 50; count++) {
		lastTopReadings[count] = transform (1024 - analogRead (TOP_INPUT));
		lastBottomReadings[count] = 1024 - analogRead (BOTTOM_INPUT);
		delay (20);
	}

	/* calibrate photoresistors */
	int meanTop = mean50 (lastTopReadings, lastTopReadings);
	int meanBottom = mean50 (lastBottomReadings, lastBottomReadings);
	offset += meanBottom - meanTop;

	// repopulate with calibrated values
	for (int count = 0; count < 50; count++) {
		lastTopReadings[count] = transform (1024 - analogRead (TOP_INPUT));
		lastBottomReadings[count] = 1024 - analogRead (BOTTOM_INPUT);
	}
	
	daylight = meanBottom;
	threshold = daylight - MARGIN;

#ifdef DEBUG_INFO
	Serial.print ("\nThreshold initial value: ");
	Serial.print (threshold);
	Serial.print ("\n");
	Serial.print ("Daylight baseline: ");
	Serial.print (daylight);
	Serial.print ("\n");
	delay (2000);
#endif /* DEBUG_INFO */
}

void loop() {
	// put your main code here, to run repeatedly:
	bool isDay;
	long lastCommand;
	long waitMillis;
	int topBrightness;
	int bottomBrightness;
	
	topBrightness = transform (1024 - analogRead (TOP_INPUT));// + offset;
	bottomBrightness = 1024 - analogRead (BOTTOM_INPUT);

	// overwrite the current slot in the circular buffers
	lastBottomReadings[lastElementUsed] = bottomBrightness;
	lastTopReadings[lastElementUsed++] = topBrightness;
	lastElementUsed %= 50;

	threshold = mean50 (lastTopReadings, lastBottomReadings) - MARGIN;

#ifdef DEBUG_INFO
	Serial.print (daylight);
	Serial.print (" D/T ");
	Serial.println (threshold); 

	Serial.print (topBrightness);
	Serial.print (" T/B ");
	Serial.println (bottomBrightness);
#endif /* DEBUG_INFO */

//  if (0){
	if (millis () - lastCommand > waitMillis) {
		if (threshold < daylight - 35) {
			// nighttime
			// update daytime status
			if (isDay) {
				isDay = 0;
				lastCommand = millis ();
				waitMillis = 250;
			} else if (topBrightness > threshold){// && bottomBrightness <= threshold) {
				Serial.print ("2\n"); // Jump!
				lastCommand = millis ();
				waitMillis = 16;
			} else if (bottomBrightness > threshold) {
				Serial.print ("1\n"); // Jump.
				lastCommand = millis ();
				waitMillis = 16;
			}
		} else {
			// daytime
			if (!isDay) {
				isDay = 1;
				lastCommand = millis ();
				waitMillis = 250;
			} else if (topBrightness < threshold) {
				Serial.print ("2\n"); // Jump!
				lastCommand = millis ();
				waitMillis = 16;
			} else if (bottomBrightness < threshold) {
				Serial.print ("1\n"); // Jump.
				lastCommand = millis ();
				waitMillis = 16;
			}
		}
	}
	
}	
