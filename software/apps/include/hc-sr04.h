#ifndef HC_SR04_H
#define HC_SR04_H

#include <stdbool.h>
#include "microbit_v2.h"

#define WALL_DISTANCE_THRESHOLD 1
#define TRIG_PIN EDGE_P0
#define ECHO_PIN EDGE_P1

void hc_sr_init();
float find_distance();
bool check_hit_wall();

#endif
