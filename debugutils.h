/*
 * debugutils.h
 * basically a bunch of debugging shenanigans lol
 */

#include "SensorArray.h"
#include "Config.h"
#include "Obstacle.h"

#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

/* Define this macro before #include'ing debugutils.h to enable debug
   output. Leaving it undefined will save you space at compile time. */
#ifdef DINOSAUR_DEBUG

#define dumpSensorReadings(SEN_ARR_PTR, CAL_PAIR_PTR) dumpSensorReadings_debug((SEN_ARR_PTR), (CAL_PAIR_PTR))
#define dumpObstacleData(OBS_PTR) dumpObstacleData_debug((OBS_PTR))

#else // DINOSAUR_DEBUG

#define dumpSensorReadings(SEN_ARR_PTR, CAL_PAIR_PTR)
#define dumpObstacleData(OBS_PTR)

#endif // DINOSAUR_DEBUG

#endif // DEBUGUTILS_H
