/*#include<stdio.h>
#include<SDL.h>
#include<SDL_image.h>
#include<string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 640;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Renderer* gRenderer = NULL;

bool init() {
	bool pass = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL Not initialized. Error %s\n", SDL_GetError());
		pass = false;
	}
	else
	{
		gWindow = SDL_CreateWindow("Zoom", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Failed to create Window.Erorr %s", SDL_GetError());
		}
		//Create renderer for window
		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
		if (gRenderer == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			pass = false;
		}
		else
		{
			//Initialize renderer color
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
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
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_FreeSurface(gScreenSurface);
	gScreenSurface = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
	IMG_Quit();
}


int main(int argc, char* args[]) {
	if (!init()) {
		printf("Failed to run init\n");
	}
	else {
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
					case SDLK_UP:
						break;

					case SDLK_DOWN:
						break;
					}
				}
			}

			SDL_Rect src = { x, y, x + 800, y + 640 };
			SDL_Rect des = {};
			des.w = static_cast<int>(src.w * zoomFactor); // width after zoom 1600, 1280
			des.h = static_cast<int>(src.h * zoomFactor);
			des.x = (SCREEN_WIDTH - des.w) / 2;
			des.y = (SCREEN_HEIGHT - des.h) / 2;
			//printf("%d,%d",des.x, des.y);

			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);

			SDL_Rect a = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2 };
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
			SDL_RenderFillRect(gRenderer, &a);

			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
			SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
			SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);


			//Update screen
			SDL_RenderPresent(gRenderer);

			//			
		}
	}
	close();
	return 0;
}*/