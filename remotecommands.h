/* 
 * remotecommands.h
 * by Eliot Baez
 *
 * This file contains macro definitions corresponding to the command
 * code returned from the 21-button remote that came with the Arduino
 * kit. These are the values stored in IRData.command after calling
 * IrReceiver.decode, and as such they are all unsigned long.
 */

#ifndef REMOTECOMMANDS_H
#define REMOTECOMMANDS_H

#define CMD_POWER			69U
#define	CMD_VOLUME_UP		70U
#define CMD_VOLUME_DOWN		21U
#define CMD_FUNC_STOP		71U
#define CMD_REW				68U
#define CMD_FFWD			67U
#define CMD_UP				9U
#define CMD_DOWN			7U			
#define CMD_PLAY_PAUSE		64U
#define CMD_START_REPEAT	13U
#define CMD_EQ				25U
#define CMD_UNKNOWN			0U

#define CMD_0	22U
#define CMD_1	12U
#define CMD_2	24U
#define CMD_3	94U
#define CMD_4	8U
#define CMD_5	28U
#define CMD_6	90U
#define CMD_7	66U
#define CMD_8	82U
#define CMD_9	74U

#endif // REMOTECOMMANDS_H
