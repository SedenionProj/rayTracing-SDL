#include "rayTracer.h"

Application::Application(unsigned int width, unsigned int height)
	:width(width), height(height), isRunning(true),
	camera(glm::vec3(0, 0, -2), glm::vec3(0, 0, 1), glm::vec2(width, height)),
	renderer(scene, camera)
{
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
		auto& k = event.key.keysym.sym;
		if(k == SDLK_z)
			camera.position += glm::vec3(0,0,0.1);
		if (k == SDLK_s)
			camera.position += glm::vec3(0, 0, -0.1);
		if (k == SDLK_q)
			camera.position += glm::vec3(-0.1, 0, 0);
		if (k == SDLK_d)
			camera.position += glm::vec3(0.1, 0, 0);
		if (k == SDLK_SPACE)
			camera.position += glm::vec3(0, 0.1, 0);
		if (k == SDLK_LSHIFT)
			camera.position += glm::vec3(0, -0.1, 0);
		if (k == SDLK_LEFT)
			camera.direction = rotateY(0.1f) * camera.direction;
		if (k == SDLK_RIGHT)
			camera.direction = rotateY(-0.1f) * camera.direction;
		if (k == SDLK_UP)
			camera.direction = rotateX(0.1f) * camera.direction;
		if (k == SDLK_DOWN)
			camera.direction = rotateX(-0.1f) * camera.direction;
	}
}

void Application::initScene() {
	camera.vfov = 90;
	
	std::shared_ptr<Sphere> s = std::make_shared<Sphere>(glm::vec3(0, 0, 0), 0.5f);
	s->material = std::make_shared<Diffuse>(glm::vec3(1, 1, 0));
	s->light = std::make_shared<AreaLight>(4000, 10.f, s);
	
	std::shared_ptr<Sphere> s3 = std::make_shared<Sphere>(glm::vec3(0, -10.5, 0), 10);
	s3->material = std::make_shared<Diffuse>(glm::vec3(1.f));
	
	scene.addShape(s);
	//scene.addShape(s2);
	scene.addShape(s3);
	//scene.addShape(s4);

	scene.addSky(std::make_shared<Sky>(5000, .5f));
	
	scene.build();
}

void Application::loop(float dt) {
	//std::cout << dt << "\n";
	renderer.render(isMoving, gSurface);
}