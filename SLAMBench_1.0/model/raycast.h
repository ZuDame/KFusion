#ifndef RAYCAST_H
#define RAYCAST_H
#include "raycast_math.h"

#include <stdint.h>

#define MAX_VOL_SIZE 256
#define MAX_VOXELS (MAX_VOL_SIZE*MAX_VOL_SIZE*MAX_VOL_SIZE)

typedef struct {
	int16_t tsdf;
	int16_t weight;
}tsdf_weight;

void raycast_init(const unsigned &frame);
void raycast();
void raycast_close();

void get_max_error(
  unsigned &x_v,
  unsigned &y_v,
  unsigned &x_n,
  unsigned &y_n,
  float &v_err,
  float &n_err);
#endif