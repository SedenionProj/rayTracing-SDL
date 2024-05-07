#include "rayTracer.h"

int main(int argc, char* args[])
{
	std::unique_ptr<Application> app = std::make_unique<Application>(1280, 720);
	app->start();
	return 0;
}