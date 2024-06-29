#include <stdio.h>
#include "rayTracer.h"
#include "renderer.h"
#include <memory.h>
#include "spectrum.h"

Application::Application(unsigned int width, unsigned height)
	:width(width), height(height), isRunning(true),
	camera(glm::vec3(0, 0, 1), glm::vec3(0, 0, -1), glm::vec2(width, height)){
	initScene();

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	gWindow = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
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

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);	
}

Application::~Application() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_Quit();
}

void Application::start() {
	Uint32 prevTime = SDL_GetTicks();
	Uint32 currentTime;
	float deltaTime;

	SDL_Event e;
	while (isRunning) {
		isMoving = false;
		while (SDL_PollEvent(&e) != 0) {
			event(e);
		}
		currentTime = SDL_GetTicks();
		deltaTime = (currentTime - prevTime);
		prevTime = currentTime;

		frame++;

		loop(deltaTime);
		
		SDL_UpdateWindowSurface(gWindow);

	}
}

void Application::event(SDL_Event& event) {
	
	if (event.type == SDL_QUIT) {
		isRunning = false;
	}
	if (event.type == SDL_KEYDOWN) {
		isMoving = true;
		switch (event.key.keysym.sym) {
		case SDLK_z:
			camera.position -= glm::vec3(0,0,0.1);
			break;
		case SDLK_s:
			camera.position -= glm::vec3(0, 0, -0.1);
			break;
		case SDLK_q:
			camera.position -= glm::vec3(-0.1, 0,0 );
			break;
		case SDLK_d:
			camera.position -= glm::vec3(0.1, 0, 0);
			break;
		case SDLK_SPACE:
			camera.position -= glm::vec3(0, 0.1, 0);
			break;
		case SDLK_LSHIFT:
			camera.position -= glm::vec3(0, -0.1, 0);
			break;
		}
		
	}
}

void Application::initScene() {
	camera.vfov = 90;
	
	//{
	//	std::shared_ptr<Sphere> s = std::make_shared<Sphere>(glm::vec3(0.2, 0, 0), 0.5f);
	//	SampledSpectrum* spec = new SampledSpectrum(glm::vec3(1, 0, 0));
	//	s->material = std::make_shared<Diffuse>(*spec);
	//	//s->light = std::make_shared<AreaLight>(3000, 10.f);
	//	scene.objects.push_back(s);
	//}
	//
	//
	//std::shared_ptr<Sphere> s2 = std::make_shared<Sphere>(glm::vec3(-1, 0, 0), 0.5);
	//SampledSpectrum* spec1 = new SampledSpectrum(glm::vec3(0, 1, 0));
	//s2->material = std::make_shared<Diffuse>(*spec1);
	//
	//std::shared_ptr<Sphere> s3 = std::make_shared<Sphere>(glm::vec3(0, 20.5, 0), 20);
	//SampledSpectrum* spec2 = new SampledSpectrum(glm::vec3(0.9));;
	//s3->material = std::make_shared<Diffuse>(*spec2);

	for (int i = 0; i < 1000; i++) {
		std::shared_ptr<Sphere> s4 = std::make_shared<Sphere>(glm::vec3(randomUniform()-0.5f, randomUniform() - 0.5f, randomUniform() - 0.5f)*10.f,0.1f);
		SampledSpectrum* spec2 = new SampledSpectrum(glm::vec3(1,0,0));;
		s4->material = std::make_shared<Diffuse>(*spec2);

		scene.objects.push_back(s4);
	}
	
	scene.sky = new Sky(5000, 3.f);
	
	scene.build();
}

glm::mat3 xyz_to_rgb = glm::mat3(
	glm::vec3(3.2406, -0.9689, 0.0557),
	glm::vec3(-1.5372, 1.8758, -0.2040),
	glm::vec3(-0.4986, 0.0415, 1.0570));

void Application::loop(float dt) {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {

			Uint32* pixel = (Uint32*)((Uint8*)gSurface->pixels + y * gSurface->pitch + x * sizeof(Uint32));
			Ray ray = camera.getRay(glm::vec2(x, y));

			if (isMoving) {
				camera.film.setSample(glm::vec2(x, y), renderPixel(ray, scene));
				frame = 1;
			} else {
				camera.film.addSample(glm::vec2(x, y), renderPixel(ray, scene));
			}

			glm::vec3 rgb_color = xyz_to_rgb * (camera.film.getSample(glm::vec2(x, y)) / (float)frame);
			rgb_color = glm::pow(rgb_color, glm::vec3(1.0 / 2.2));
			*pixel = SDL_MapRGB(gSurface->format,
				glm::clamp(rgb_color.x, 0.f, 1.f) * 255.f,
				glm::clamp(rgb_color.y, 0.f, 1.f) * 255.f,
				glm::clamp(rgb_color.z, 0.f, 1.f) * 255.f);
		}
	}
}