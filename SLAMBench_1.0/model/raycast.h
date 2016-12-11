#ifndef RAYCAST_H
#define RAYCAST_H
#include "raycast_math.h"

#include <stdint.h>

#define MAX_VOL_SIZE 256
#define MAX_VOXELS (MAX_VOL_SIZE*MAX_VOL_SIZE*MAX_VOL_SIZE)

typedef struct {
	uint16_t tsdf;
	uint16_t weight;
}tsdf_weight;

void raycast_init(const unsigned &frame);
void raycast();
void raycast_close();
#endif