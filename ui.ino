/* 
 * ui.ino
 * contains user interface utilities
 */

#include <IRremote.h>
#include "remotecommands.h"
#include "ui.h"

unsigned int awaitRemoteCommand(IRrecv *recv) {
	recv->resume();
	int cmd = CMD_UNKNOWN;
	
	// turn on the LED to indicate that the user must provide input
	digitalWrite(LED_BUILTIN, HIGH);

	// repeat until recvCmd is some known value, then return that value
	while (cmd == CMD_UNKNOWN) {
		if (recv->decode()) {
			cmd = recv->decodedIRData.command;
			recv->resume();
		}
	}

	digitalWrite(LED_BUILTIN, LOW);
	return cmd;
}

short mainMenu(IRrecv *recv) {
	Serial.println("\n*** MAIN MENU *** (1)");
	Serial.println("[0] Soft reset");
	Serial.println("[1] Start game");
	Serial.println("[2] Configure...");
	Serial.println("[3] Calibrate...");
	Serial.println("[4] Hard reset");

	/* just keep getting input until the user chooses something on the
	   menu, then return the index of that option */
	while (true) {
		switch (awaitRemoteCommand(recv)) {
		case CMD_0:
			return 0;
		case CMD_1:
		case CMD_PLAY_PAUSE:
			return 1;
		case CMD_2:
			return 2;
		case CMD_3:
			return 3;
		case CMD_4:
			return 4;
		}
	}
}

short calibrationMenu(IRrecv *recv) {
	Serial.println("\n*** CALIBRATION MENU *** (1)");
	Serial.println("[0] Go back...");
	Serial.println("[1] Calibrate sensor array...");
	Serial.println("[2] Get noise floor...");

	/* same as in mainMenu(), get input until we receive a valid choice */
	while (true) {
		switch (awaitRemoteCommand(recv)) {
		case CMD_0:
			return 0;
		case CMD_1:
		case CMD_PLAY_PAUSE:
			return 1;
		case CMD_2:
			return 2;
		}
	}
}

short configurationMenu(IRrecv *recv, Config *config) {
	Serial.println("\n*** CONFIGURATION MENU *** (3)");
	Serial.println("[0] Go back...");
	Serial.println("[1] Change screen width...");
	Serial.println("[2] Change lag compensation...");
	Serial.println("[3] Change target score...");

	/* yeah, if you're this far along in the code you already know what
	   what's going on */
	while (true) {
		switch (awaitRemoteCommand(recv)) {
		case CMD_0:
			return 0;
		case CMD_1:
			return 1;
		case CMD_2:
			return 2;
		case CMD_3:
		case CMD_PLAY_PAUSE:
			return 0;
		}
	}
}

short awaitDigit(IRrecv *recv) {
	while (true) {
		switch (awaitRemoteCommand(recv)) {
		case CMD_0:
			return 0;
		case CMD_1:
			return 1;
		case CMD_2:
			return 2;
		case CMD_3:
			return 3;
		case CMD_4:
			return 4;
		case CMD_5:
			return 5;
		case CMD_6:
			return 6;
		case CMD_7:
			return 7;
		case CMD_8:
			return 8;
		case CMD_9:
			return 9;
		case CMD_PLAY_PAUSE:
			return -1;
		}
	}
}

int getInt(IRrecv *recv) {
	int result = 0;
	short digit;
	
	// get digits until play/pause signal received
	while ( (digit = awaitDigit(recv)) != -1 ) {
		Serial.write(digit + '0');
		Serial.flush();
		// shift result left 1 decimal place
		result *= 10;
		// add digit to ones place
		result += digit;
	}
	Serial.println();
	return result;
}

long getLong(IRrecv *recv) {
	long result = 0;
	short digit;

	// see above
	while ( (digit = awaitDigit(recv)) != -1) {
		Serial.write(digit + '0');
		Serial.flush();
		result *= 10;
		result += digit;
	}
	Serial.println();
	return result;
}
