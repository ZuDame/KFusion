#include "raycast.h"
#include <iostream>


int main() {
	std::cout << "raycasting model" << std::endl;

	raycast_init(20);
	raycast();
	raycast_close();
}