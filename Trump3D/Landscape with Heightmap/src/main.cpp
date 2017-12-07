#include "LandscapeApp.h"

int main() {
	
	auto app = new LandscapeApp();
	app->run("TRUMP 3D Simulator", 1280, 720, false);
	delete app;

	return 0;
}