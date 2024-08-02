/*#include<stdio.h>
#include<SDL.h>
#include<SDL_image.h>
#include<string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 640;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;

bool init() {
	bool pass = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL Not initialized. Error %s\n", SDL_GetError());
		pass = false;
	}
	else {
		gWindow=SDL_CreateWindow("Zoom", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Failed to create Window.Erorr %s", SDL_GetError());
		}
		else {
			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				pass = false;
			}
			else
			{
				//Get window surface
				gScreenSurface = SDL_GetWindowSurface(gWindow);
			}
		}

	}

	return pass;
}

void close() {
	
	SDL_FreeSurface(gScreenSurface);
	gScreenSurface = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
}


int main(int argc, char* args[]) {
	if (!init()) {
		printf("Failed to run init\n");
	}
	else {
		printf("Working");
		SDL_Surface* loadedSurface = NULL;
		SDL_Surface* image = NULL;

		loadedSurface= IMG_Load("grains.png");
	    image = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
		if (image == NULL) {
			printf("Not Working");
		}
		bool quit = false;
		int x = 0;
		int y = 0;
		int inc = 10;
		float zoomFactor = 1.0f;
		SDL_Event e;
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT) { quit = true; }
				if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym)
					{
					case SDLK_w:
						zoomFactor = zoomFactor + 0.25f;
						break;
					case SDLK_s:
						zoomFactor = zoomFactor - 0.25f;
						break;

					case SDLK_DOWN:
						y = y + inc;
						if (y + 640 >= image->h) {
							y = image->h - 640;
						}
						break;

					case SDLK_UP:
						y = y - inc;
						if (y < 0) {
							y = 0;
						}
						break;

					case SDLK_LEFT:
						x = x - inc;
						if (x < 0) {
							x = 0;
						}
						break;
						break;

					case SDLK_RIGHT:
						x = x + inc;
						if (x + 800 >= image->w) {
							x = image->w - 800;
						}
						break;
						break;
					}
				}
			}

			SDL_Rect src = { x, y, x+800, y+640 };
			SDL_Rect des = {};
			des.w = static_cast<int>(src.w * zoomFactor); // width after zoom 1600, 1280
			des.h = static_cast<int>(src.h * zoomFactor);
			des.x = (SCREEN_WIDTH-des.w)/2;
			des.y = (SCREEN_HEIGHT- des.h) / 2;
			//printf("%d,%d",des.x, des.y);

			SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, rand()%255, rand() % 255, rand() % 255));
			SDL_BlitScaled(image, &src, gScreenSurface, &des);

//			SDL_BlitSurface(image, &src, gScreenSurface, NULL);
			//Update the surface
			
			//SDL_TICKS_PASSED()%255;
			SDL_UpdateWindowSurface(gWindow);
		}
	}
	close();
	return 0;
}*/