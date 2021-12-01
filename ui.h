/*
 * ui.h
 *
 * contains declarations for user interface elements and functions
 */

#include <IRremote.h>
#include "Config.h"

#ifndef UI_H
#define UI_H

#ifndef IR_TIMEOUT
#define IR_TIMEOUT 200UL
#endif

// waits for input from a receiver
unsigned int awaitRemoteCommand(IRrecv *recv);

// main menu
short mainMenu(IRrecv *recv);

// menu for selecting calibration functions
short calibrationMenu(IRrecv *recv);

// menu to change configuration
short configurationMenu(IRrecv *recv, Config *config);

#endif // UI_H
