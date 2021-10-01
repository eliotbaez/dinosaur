#define BOTTOM_INPUT 14
#define TOP_INPUT 15
#define MARGIN 5

int threshold;
int daylight;
bool isDay;

/* 
 * parameters to transform the readings from 
 * the upper resistor
 */
double scale = 1.195;
double offset = 6.02;

/* transform function that uses our parameters above */
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
	
	Serial.print ("\nThreshold initial value: ");
	Serial.print (threshold);
	Serial.print ("\n");
	Serial.print ("Daylight baseline: ");
	Serial.print (daylight);
	Serial.print ("\n");
	delay (2000);
}

void loop() {
	// put your main code here, to run repeatedly:
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

//  Serial.print (daylight);
//  Serial.print (" D/T ");
//  Serial.println (threshold); 
//   
//  Serial.print (topBrightness);
//  Serial.print (" T/B ");
//  Serial.println (bottomBrightness);

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
	
	
//    spacebar.write (50);
//    delay (100);
//    spacebar.write (35);
//    delay (100);
//  delay (50);
}
		 
