#ifndef RENDER_H
#define RENDER_H
#include "raycast.h"

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
}pixel;

void model_top_view();
#endif