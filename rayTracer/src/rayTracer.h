#pragma once
#include <memory>

#include "SDL.h" 
#include "camera.h"
#include "shape.h"

class Application {
public:
	Application(unsigned int width, unsigned height);
	~Application();
	void start();
	void event(SDL_Event& event);
	void loop(float dt);
	void initScene();
	
private:
	bool isRunning;
	bool isMoving;
	unsigned int width;
	unsigned int height;

	unsigned int frame = 0;

	Camera camera;
	Scene scene;

	SDL_Window*   gWindow   = nullptr;
	SDL_Renderer* gRenderer = nullptr;
	SDL_Surface*  gSurface  = nullptr;

	std::vector<glm::vec3> colorBuffer;
};