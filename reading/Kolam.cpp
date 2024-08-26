#include <stdio.h>
#include<SDL.h>
#include<SDL_image.h>
#include<string>
#include <SDL_mixer.h>
#include<iostream>

int SPACE, rows,cols ,offx, offy, SCREEN_WIDTH, SCREEN_HEIGHT, TOTAL_BUTTONS, MaxThick;
float thick;

enum buttonType {
	top,
	right,
	bottom,
	left,
};

class Texture {
	public:
		Texture();
		~Texture();

		bool LoadTexture(std::string p);
		bool createsheetLR(int width, int height, SDL_TextureAccess access);
		void setAsRenderTarget();
		void render(int x, int y, SDL_Rect* clip = NULL, int desW=NULL, int desH=NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
		int getHeight();
		int getWidth();

	private:
		SDL_Texture* text;
		int wd;
		int ht;

};

class button {
	public:
		button();
		void setPosition(int x, int y, buttonType w);
		void handleEvent(SDL_Event* e);
		void render();
		void mouseIn();
		void mouseOut();

	private:
		SDL_Point pos;
		int cur;
		buttonType place;
		int bw;
		int bh;
};

SDL_Window* gWindow = NULL;
SDL_Window* popWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Renderer* popRenderer = NULL;
Texture dot, sheetUD, sheetLR;
SDL_Rect ImgUD[2];
SDL_Rect ImgLR[2];
button * butts;
Mix_Music* music = NULL;
Mix_Chunk* buttSound = NULL;

//TEXTURES FUNCTIONS

Texture::Texture() {
	text = NULL;
	wd = 0;
	ht = 0;
}
Texture::~Texture() {
	if (text != NULL) {
		SDL_DestroyTexture(text);
		text = NULL;
		wd = 0;
		ht = 0;
	}
}
bool Texture::LoadTexture(std::string p) {
	SDL_Surface* temp = NULL;
	SDL_Texture* temptext = NULL;
	temp = IMG_Load(p.c_str());
	if (temp == NULL) {
		printf("Failed to laod %s! SDL Error:%s\n", p.c_str(), IMG_GetError());
	}
	else {
		
		SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 0XFF, 0XFF));

		temptext = SDL_CreateTextureFromSurface(gRenderer, temp);
		if (temptext == NULL) {
			printf("Cant convert surface to texture! SDL Error: %s", SDL_GetError());
		}
		else {
			wd = temp->w;
			ht = temp->h;
		}
		SDL_FreeSurface(temp);
	}
	text = temptext;
	return text != NULL;
}
bool Texture::createsheetLR(int width, int height, SDL_TextureAccess access)
{

	//Create uninitialized texture
	text = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);
	if (text == NULL)
	{
		printf("Unable to create streamable sheetLR texture! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		wd = width;
		ht = height;
	}

	return text != NULL;
}
void Texture::setAsRenderTarget()
{
	//Make self render target
	SDL_SetRenderTarget(gRenderer, text);
}
void Texture::render(int x, int y, SDL_Rect* clip, int desW, int desH, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect des = { x,y,desW,desH };
	if (desW == NULL) {
		des.w = wd;
	}
	if (desH == NULL) {
		des.h = ht;
	}
	SDL_RenderCopyEx(gRenderer, text, clip, &des, angle, center, flip);
}
int Texture::getHeight() {
	return ht;
}
int Texture::getWidth() {
	return wd;
}

//BUTTON FUNCTIONS

button::button() {
	pos.x = 0;
	pos.y = 0;

	place = left;
	cur = 2;
	bw= SPACE+thick ;
	bh = 2 * SPACE;


}
void button::setPosition(int x, int y, buttonType w) {
	pos.x = x;
	pos.y = y;
	place = w;
	
	switch (place)
	{
	case top:
	case bottom:
		bw = 4*SPACE;
		bh = bh - SPACE;
		break;
	}
	render();

	SDL_Rect rec = { pos.x, pos.y, bw, bh };
	SDL_SetRenderDrawColor(gRenderer, 0xF2, 0X7C, 0X50, 0xFF);
	SDL_RenderDrawRect(gRenderer, &rec);
	SDL_RenderPresent(gRenderer);

	
}
void button::render() {
	
	//Clear the screen
	SDL_Rect a = { pos.x, pos.y, bw, bh };
	SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0xFF);
	SDL_RenderFillRect(gRenderer, &a);
	SDL_RenderPresent(gRenderer);
	//so that the white rectangle is still there
	if (cur == 2) {
		cur = (cur + 1) % 3;
		return;
	}
	//Render the image according to the current sprite
	switch (place)
	{
	case right:
	case left:
		sheetLR.render(pos.x, pos.y, &ImgLR[cur],bw,bh,(place-1)*90.0);
		break;
	case bottom:
	case top:
		sheetUD.render(pos.x, pos.y, &ImgUD[cur], bw, bh, place * 90.0);
		break;
	}
	cur = (cur + 1) % 3;

}
void button::mouseIn() {
	SDL_Rect a = { pos.x, pos.y , bw, bh };
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawRect(gRenderer, &a);
	SDL_RenderPresent(gRenderer);
}
void button::mouseOut() {
	SDL_Rect a = { pos.x, pos.y, bw, bh };
	SDL_SetRenderDrawColor(gRenderer, 0xF2, 0X7C, 0X50, 0xFF);
	SDL_RenderDrawRect(gRenderer, &a);
	SDL_RenderPresent(gRenderer);
}


// Rest of the functions
int SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
	int offsetx, offsety, d;
	int status;

	//CHECK_RENDERER_MAGIC(renderer, -1);

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {
		status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
		status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
		status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
		status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
		status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
		status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
		status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
		status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}
int SDL_RenderFillCircle(SDL_Renderer* renderer, int x, int y, int radius){
	int offsetx, offsety, d;
	int status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {

		status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
			x + offsety, y + offsetx);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
			x + offsetx, y + offsety);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
			x + offsetx, y - offsety);
		status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
			x + offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}

void globeDec(int r, int c) {
	rows= (rows + 15 - abs(rows - 15)) / 2; // find min
	cols = (cols + 10 - abs(cols - 10)) / 2; // find min
	SPACE = std::min(SCREEN_WIDTH / (4 * r), SCREEN_HEIGHT / (4 * c));
	offx = (SCREEN_WIDTH-4*SPACE*r)/2;
	offy = (SCREEN_HEIGHT - 4 * SPACE * c) / 2;
	TOTAL_BUTTONS = 4 * r * c;
	butts = new button[TOTAL_BUTTONS];
	MaxThick = 1.172 * SPACE;// 2(2-root(2))*thick -> where the outer circle will touch the boundary
	thick = (thick+MaxThick-abs(thick-MaxThick)) / 2; // find min of thick and maxthick
}
bool init() {
	bool pass = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL Not initialized. Error %s\n", SDL_GetError());
		pass = false;
	}
	else
	{
		gWindow = SDL_CreateWindow("Kolam", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		if (gWindow == NULL) {
			printf("Failed to create Window.Erorr %s", SDL_GetError());
		}
		else {
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
			}
		}
		popWindow = SDL_CreateWindow("Pop Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 200, 200, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_HIDDEN | SDL_WINDOW_SKIP_TASKBAR);
		if (popWindow == NULL) {
			printf("Failed to create Window.Erorr %s", SDL_GetError());
		}
		else {
			//Create renderer for window
			popRenderer = SDL_CreateRenderer(popWindow, -1, SDL_RENDERER_ACCELERATED);
			if (popRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				pass = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(popRenderer, 0x00, 0x00, 0x00, 0x00);
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					pass = false;
				}
			}
		}


		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
			pass = false;
		}
	}

	return pass;
}
bool load() {
	bool pass = true;
	if (!dot.createsheetLR(SPACE /3, SPACE /3, SDL_TEXTUREACCESS_TARGET)) {
		printf("Failed to load button sprite\n");
		pass = false;
	}
	else {
		dot.setAsRenderTarget();
		SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0XFF);
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);

		SDL_RenderFillCircle(gRenderer, SPACE / 6, SPACE / 6, SPACE / 7) ;
		SDL_SetRenderTarget(gRenderer, NULL);

	}
	if (!sheetUD.createsheetLR(SPACE * 4, SPACE * 2, SDL_TEXTUREACCESS_TARGET)) {
		printf("Failed to load button sprite\n");
		pass = false;
	}
	else {
		sheetUD.setAsRenderTarget();

		SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0XFF);
		SDL_RenderClear(gRenderer);
		float y;
		
		SDL_Rect rec = { 0,0,thick,1 };
		//Draws the triangle
		SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);
		for (float x = SPACE;x < 2 * SPACE;x += 1) {
			y = -x + 2 * SPACE;
			rec.x = x-thick/2; rec.y = y;
			SDL_RenderFillRect(gRenderer, &rec);
		}
		for (float x = 2*SPACE;x < 3 * SPACE;x += 1) {
			y = x - 2*SPACE;
			rec.x = x-thick/2; rec.y = y;
			SDL_RenderFillRect(gRenderer, &rec);
		}
		//Draws the circle
		SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);
		for (int x = -thick*0.3; x < 0.4 * thick; x++) {
			SDL_RenderDrawCircle(gRenderer, 2 * SPACE, 3 * SPACE, (1.414 * SPACE) + x);
		}

		SDL_SetRenderTarget(gRenderer, NULL);

		ImgUD[0] = { 0,0, SPACE * 4, SPACE };
		ImgUD[1] = { 0, SPACE, SPACE * 4, SPACE };
		}
	if (!sheetLR.createsheetLR(2*(SPACE+thick), 2 * SPACE, SDL_TEXTUREACCESS_TARGET)) {
		printf("Failed to load button sprite\n");
		pass = false;
	}
	else {
		sheetLR.setAsRenderTarget();

		SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0XFF);
		SDL_RenderClear(gRenderer);
		float y;
		SDL_Rect rec = { 0,0,1,thick };

		//Draws lines
		SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);
		for (float x = sheetLR.getWidth()/2;x < sheetLR.getWidth();x += 1) {
			y = - SPACE + x-2*thick;
			rec.x = x; rec.y = y - thick / 2;
			SDL_RenderFillRect(gRenderer, &rec);
		}
		for (float x = sheetLR.getWidth()/2;x < sheetLR.getWidth();x += 1) {
			y = 3 * SPACE - x + thick * 2;
			rec.x = x; rec.y = y - thick / 2;
			SDL_RenderFillRect(gRenderer, &rec);
		}
		//Draws the circle
		SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);
		for (int x = -0.3*thick; x < 0.4 * thick; x++) {
			SDL_RenderDrawCircle(gRenderer, -SPACE+thick, SPACE, (1.414 * SPACE) + x);
		}

		SDL_SetRenderTarget(gRenderer, NULL);
		ImgLR[1] = { 0,0, sheetLR.getWidth()/2, sheetLR.getHeight()};
		ImgLR[0] = { sheetLR.getWidth()/2,0, sheetLR.getWidth()/2, sheetLR.getHeight()};
		}

	//Load music
	music = Mix_LoadMUS("Music/santoor.mp3");
	if (music == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		pass = false;
	}
	buttSound = Mix_LoadWAV("Music/low.wav");
	if (buttSound == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		pass = false;
	}
	return pass;
}
void close() {

	dot.~Texture();
	sheetUD.~Texture();
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	Mix_FreeMusic(music);
	music = NULL;
	Mix_FreeChunk(buttSound);
	buttSound = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
	IMG_Quit();
}
void drawdotsButtons() {
	int a = dot.getWidth();

	for (int ri = 0, x = 2 * SPACE+offx, i=0; ri <rows; ri++, x += 4 * SPACE){
		for (int ci = 0, y = 2 * SPACE+offy;ci < cols; ci++, i += 4, y += 4 * SPACE) {
			
			dot.render(x - a/2, y - a/2 );

			//0->left 1-> bottom 2-> right 3->top
			butts[i].setPosition(x-2*SPACE,y-SPACE,left);
			butts[i + 2].setPosition(x + SPACE-thick, y - SPACE, right);
			butts[i + 1].setPosition(x - 2*SPACE, y + SPACE, bottom);
			butts[i + 3].setPosition(x - 2*SPACE, y - 2 * SPACE, top);
		}
	}
	SDL_RenderPresent(gRenderer);
}
int activebuttonID(buttonType &place) {
	int x, y,i;
	SDL_GetMouseState(&x, &y);
	int N = cols * ((x-offx) / (4 * SPACE)) + ((y-offy) / (4 * SPACE));
	switch (place)
	{
	case left:
		i = 4 * N;
		break;
	case bottom:
		i = 4 * N + 1;
		break;
	case right:
		i = 4 * N + 2;
		break;
	case top:
		i = 4 * N + 3;
		break;
	}
	return i;
}
int checkInside(buttonType& place) {

	int x, y, xin, yin;
	int inn = 0;

	SDL_GetMouseState(&x, &y);

	if (x<offx || x>(offx + rows * 4 * SPACE)) {
		return 0;
	}
	if (y<offy || y>(offy + cols * 4 * SPACE)) {
		return 0;
	}
	yin = (y - offy - (4 * SPACE) * ((y - offy) / (4 * SPACE))) / SPACE;

	if (yin == 0) {
		place = top;
		return 1;
	}
	else if( yin ==3){
		place = bottom;
		return 1;
	}

	xin = (x - offx - (4 * SPACE) * ((x - offx) / (4 * SPACE)));
	if(xin <= SPACE+thick) {
		place = left;
		return 1;
	}
	else if (3*SPACE-thick<=xin) {
		place = right;
		return 1;
	}
	return inn;
}


int main(int argc, char* args[]) {
	/*using namespace std;
	cout << "Enter no. of Dots you want in a Row:";
	cin >> rows;
	cout << "Enter no. of Dots you want in a Colum:";
	cin >> cols;
	cout << "Enter the line thickness (1-4):";
	cin >> thick;
	if (thick > 4) {
		thick = 4;
	}
	else if (thick < 1) {
		thick = 1;
	}*/
	rows = 3;
	cols = 3;
	thick = 20;
	SCREEN_HEIGHT = 600;
	SCREEN_WIDTH = 1000;
	globeDec(rows, cols);
	
	if (!init()) {
		printf("Failed to run init\n");
	}
	else {
		if (!load()) {

		}
		bool quit = false;
		SDL_Event e;
		
		//Mix_PlayMusic(music, -1);
		SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0xFF);
		SDL_RenderClear(gRenderer);
		drawdotsButtons();
		int pev = 0;
		int popVisible = 0;
		while (!quit)
		{
			
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				//if (e.type == SDL_QUIT) { quit = true; }

				if (e.type == SDL_WINDOWEVENT) {
					if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
						printf("New");
						quit = true;
					}
					else if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						SCREEN_WIDTH = e.window.data1;
						SCREEN_HEIGHT = e.window.data2;
						globeDec(rows, cols);
						load();
						SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0xFF);
						SDL_RenderClear(gRenderer);
						drawdotsButtons();
					}
				}
				

				if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
					popVisible = !popVisible;
					if (popVisible) {
						SDL_ShowWindow(popWindow);
						SDL_SetRenderDrawColor(popRenderer, 255, 255, 255, 255);
						SDL_RenderClear(popRenderer);
						SDL_RenderPresent(popRenderer);
					}
					else {
						SDL_HideWindow(popWindow);
					}
				}

				butts[pev].mouseOut();
				buttonType place;
				int chk = checkInside(place);
				if (chk == 1) {
					pev = activebuttonID(place);
					if (e.type == SDL_MOUSEBUTTONDOWN) { 
						Mix_PlayChannel(-1, buttSound, 0);
						butts[pev].render(); 
					}
					butts[pev].mouseIn();
				}
				SDL_RenderPresent(gRenderer);
				
			}
		}
		close();
		return 0;
	}

}