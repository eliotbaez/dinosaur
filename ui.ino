/* 
 * ui.ino
 * contains user interface utilities
 */

#include <IRremote.h>
#include "remotecommands.h"
#include "ui.h"

unsigned int awaitRemoteCommand(IRrecv *recv) {
	irrecv.resume();
	int cmd = CMD_UNKNOWN;
	
	// repeat until recvCmd is some known value, then return that value
	while (cmd == CMD_UNKNOWN) {
		if (irrecv.decode()) {
			cmd = irrecv.decodedIRData.command;
		}
	}
	return cmd;
}

short mainMenu(IRrecv *recv) {
	Serial.println("\n*** MAIN MENU *** (1)");
	Serial.println("0. Soft reset");
	Serial.println("1. Start game");
	Serial.println("2. Configure...");
	Serial.println("3. Calibrate...");
	Serial.println("4. Hard reset");

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
	Serial.println("0. Go back...");
	Serial.println("1. Calibrate sensor array...");
	Serial.println("2. Get noise floor...");

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
