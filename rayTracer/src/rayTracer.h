#pragma once
#include <memory>
#include <memory.h>
#include <algorithm>
#include <execution>
#include <stdio.h>

#include "SDL.h" 
#include "rayTracer.h"
#include "renderer.h"
#include "spectrum.h"
#include "camera.h"
#include "shape.h"
#include "renderer.h"

class Application {
public:
	Application(unsigned int width, unsigned height);
	~Application();

	void start();

	Application& operator=(const Application&) = delete;
	Application(const Application&) = delete;

private:
	void event(SDL_Event& event);
	void loop(float dt);
	void initScene();
	
private:
	bool isRunning;
	bool isMoving;
	unsigned int width;
	unsigned int height;


	Scene scene;
	Camera camera;
	Renderer renderer;

	SDL_Window*   gWindow   = nullptr;
	SDL_Renderer* gRenderer = nullptr;
	SDL_Surface*  gSurface  = nullptr;
};