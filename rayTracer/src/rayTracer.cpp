#include <stdio.h>
#include "rayTracer.h"
#include "renderer.h"


Application::Application(unsigned int width, unsigned height)
	:width(width), height(height), isRunning(true)
{
	camera.resolution = glm::vec2(width, height);
	camera.vfov = 90;
	camera.position  = glm::vec3(0, 0, 1);
	camera.direction = glm::vec3(0, 0, -1);

	initScene();
	

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
	}

	gSurface = SDL_GetWindowSurface(gWindow);
	gTexture = SDL_CreateTextureFromSurface(gRenderer, gSurface);

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);	


}

Application::~Application()
{
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_Quit();
}

void Application::start()
{
	

	Uint32 prevTime = SDL_GetTicks();
	Uint32 currentTime;
	float deltaTime;

	SDL_Event e;
	while (isRunning)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			event(e);
			
		}
		currentTime = SDL_GetTicks();
		deltaTime = (currentTime - prevTime) / 1000.0f; // Convert to seconds
		prevTime = currentTime;

		frame++;

		loop(deltaTime);
		
		SDL_UpdateWindowSurface(gWindow);

	}
}

void Application::event(SDL_Event& event) {
	
	if (event.type == SDL_QUIT)
	{
		isRunning = false;
	}
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym)
		{
		case SDLK_UP:
			camera.position+=glm::vec3(0,0,0.1);
			break;

		case SDLK_DOWN:
			camera.position += glm::vec3(0, 0, -0.1);
			break;

		case SDLK_LEFT:
			camera.position += glm::vec3(-0.1, 0,0 );
			break;

		case SDLK_RIGHT:
			camera.position += glm::vec3(0.1, 0, 0);
			break;

		default:
			break;
		}
	}
}



void Application::initScene()
{
	Sphere* s = new Sphere;
	s->r = 0.5;
	s->origin = glm::vec3(1,0,0);

	Sphere* s2 = new Sphere;
	s2->r = 0.5;
	s2->origin = glm::vec3(-1, 0, 0);

	scene.objects.push_back(s);
	scene.objects.push_back(s2);
}


void Application::loop(float dt) {
	//printf("%f\n", dt);
	
	

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			Ray ray = camera.getRay(glm::vec2(x, y));
			glm::vec3 color = glm::clamp(render(ray, scene),0.f,1.f);
			Uint32* pixel = (Uint32*)((Uint8*)gSurface->pixels + y * gSurface->pitch + x * sizeof(Uint32));
			*pixel = SDL_MapRGB(gSurface->format, color.x * 255, color.y * 255,  color.z * 255);
		}
	}
	SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

}