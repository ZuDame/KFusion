#include "raycast.h"
#include "render.h"
#include <iostream>


int main() {
	std::cout << "raycasting model" << std::endl;
  unsigned x_v, x_n, y_v, y_n;
  float v_err, n_err;

	raycast_init(20);  
	raycast();
  model_top_view();
  model_top_view_from_vertices();
  get_max_error(x_v, y_v, x_n, y_n, v_err, n_err);

  printf("vertex (%u,%u): max error %3.5f\n", x_v, y_v, v_err);
  printf("normal (%u,%u): max error %3.5f\n", x_n, y_n, n_err);
	raycast_close();
}