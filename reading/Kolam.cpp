#include <stdio.h>
#include<SDL.h>
#include<SDL_image.h>
#include<string>
#include<iostream>


const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;
int SPACE, rows,cols ,offx, offy;
float thick;
int DOT,TOTAL_BUTTONS;
//const int TOTAL_BUTTONS = (SCREEN_HEIGHT * SCREEN_WIDTH) / (4 * SPACE * SPACE);
// 
//Satyam Check Commit

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
		bool createBlank(int width, int height, SDL_TextureAccess access);
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
SDL_Renderer* gRenderer = NULL;
Texture dot, sheet, blank;

SDL_Rect StateImg[2];

button * butts;

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
bool Texture::createBlank(int width, int height, SDL_TextureAccess access)
{

	//Create uninitialized texture
	text = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);
	if (text == NULL)
	{
		printf("Unable to create streamable blank texture! SDL Error: %s\n", SDL_GetError());
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
	bw= SPACE ;
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
		bw = bw + SPACE;
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
	
	//Render the image according to the current sprite
	switch (place)
	{
	case left:
	case right:
		sheet.render(pos.x - SPACE / 2, pos.y + SPACE / 2, &StateImg[cur], bh, bw, place * 90.0, NULL, SDL_FLIP_NONE);//the position has been offeseted and the widthd and height exchanged so that the image is rendered to the correct place after rotation
		break;
	case bottom:
	case top:
		sheet.render(pos.x, pos.y, &StateImg[cur], bw, bh, place * 90.0, NULL, SDL_FLIP_NONE);
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

void globeDec(int rows, int cols) {
	//TOTAL_BUTTONS = (SCREEN_HEIGHT * SCREEN_WIDTH) / (4 * SPACE * SPACE);
	SPACE = std::min(SCREEN_WIDTH / (4 * rows), SCREEN_HEIGHT / (4 * cols));
	DOT = SPACE/6;
	offx = (SCREEN_WIDTH-4*SPACE*rows)/2;
	offy = (SCREEN_HEIGHT - 4 * SPACE * cols) / 2;
	TOTAL_BUTTONS = 4 * rows * cols;
	butts = new button[TOTAL_BUTTONS];
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

	return pass;
}
bool load() {
	bool pass = true;
	if (!dot.LoadTexture("dot.png")) {
		printf("Failed to load button sprite\n");
		pass = false;
	}
	/*if (!sheet.LoadTexture("sheet.png")) {
		printf("Failed to load sheet sprite\n");
		pass = false;
	}*/
	if (!sheet.createBlank(SPACE * 2, SPACE * 2, SDL_TEXTUREACCESS_TARGET)) {
		printf("Failed to load button sprite\n");
		pass = false;
	}
	else {
		sheet.setAsRenderTarget();

		SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43,  0XFF);
		SDL_RenderClear(gRenderer);
		float y;
		SDL_Rect rec = { 0,0,thick,1 };
		SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);
		for (float x = 0;x < SPACE;x += 1) {
			y = (1-thick/(2*SPACE))*abs(x - SPACE);
			rec.x = x-thick; rec.y = y;
			SDL_RenderFillRect(gRenderer, &rec);
		}
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0xFF, 0xFF);

		for (float x = SPACE;x < 2 * SPACE;x += 1) {
			y = (1 - thick / (2 * SPACE)) * abs(x - SPACE);
			rec.x = x ; rec.y = y;
			SDL_RenderFillRect(gRenderer, &rec);
		}
		
		

		SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);
		for (int x = 0; x < thick; x++) {
			SDL_RenderDrawCircle(gRenderer, SPACE,3*SPACE ,(1.414*SPACE) + x);
		}
		

		
		
		SDL_SetRenderTarget(gRenderer, NULL);
		

		//blank.render(50, 50, NULL, NULL, NULL, NULL, NULL, SDL_FLIP_NONE);
		StateImg[0] = { 0,0, sheet.getWidth(), sheet.getHeight() / 2 };
		StateImg[1] = { 0, (sheet.getHeight() /2) +3, sheet.getWidth(), sheet.getHeight() / 2  };
	}
	if (!blank.createBlank(SPACE*2,SPACE, SDL_TEXTUREACCESS_TARGET)) {
		printf("Failed to load button sprite\n");
		pass = false;
	}
	else {
		blank.setAsRenderTarget();

		SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0XFF);
		SDL_RenderClear(gRenderer);
		float y;
		SDL_Rect rec = { 0,0,thick,thick };
		SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);
		for (float x = 0;x < 2 * SPACE;x += 0.1) {
			y = abs(x - SPACE);
			rec.x = x; rec.y = y;
			SDL_RenderFillRect(gRenderer, &rec);
		}

		SDL_SetRenderTarget(gRenderer, NULL);
	}
	return pass;
}
void close() {

	dot.~Texture();
	sheet.~Texture();
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
	IMG_Quit();
}
void drawdotsButtons() {
	
	for (int ri = 0, x = 2 * SPACE+offx, i=0; ri <rows; ri++, x += 4 * SPACE){
		for (int ci = 0, y = 2 * SPACE+offy;ci < cols; ci++, i += 4, y += 4 * SPACE) {
			dot.render(x - DOT, y - DOT, NULL, 2 * DOT, 2 * DOT );

			//0->left 1-> bottom 2-> right 3->top
			butts[i].setPosition(x-2*SPACE,y-SPACE,left);
			butts[i+1].setPosition(x - SPACE, y +SPACE, bottom);
			butts[i + 2].setPosition(x + SPACE, y - SPACE, right);
			butts[i + 3].setPosition(x - SPACE, y - 2 * SPACE, top);
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
	xin = (x - offx - (4 * SPACE) * ((x- offx) / (4 * SPACE))) / SPACE;
	yin = (y - offy - (4 * SPACE) * ((y - offy) / (4 * SPACE))) / SPACE;

	if (xin == 1 || xin == 2) {
		switch (yin)
		{
		case 0:
			place = top;
			inn = 1;
			break;
		case 3:
			place = bottom;
			inn = 1;
			break;
		}
	}
	else if (yin == 1 || yin == 2) {
		switch (xin)
		{
		case 0:
			place = left;
			inn = 1;
			break;
		case 3:
			place = right;
			inn = 1;
			break;
		}
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
	rows = 4;
	cols = 4;
	thick = 20;
	globeDec(rows, cols);
	
	printf("Spce = %d, dots = %d, total buts =%d\n", SPACE, DOT, TOTAL_BUTTONS);
	if (!init()) {
		printf("Failed to run init\n");
	}
	else {
		if (!load()) {

		}
		bool quit = false;
		SDL_Event e;
		
		//drawgrid();
		SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0xFF);
		SDL_RenderClear(gRenderer);
		drawdotsButtons();
		int pev = 0;
		while (!quit)
		{
			//Handle events on queue
			
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT) { quit = true; }
				butts[pev].mouseOut();
				buttonType place;
				int chk = checkInside(place);
				if (chk == 1) {
					pev = activebuttonID(place);
					if (e.type == SDL_MOUSEBUTTONDOWN) { butts[pev].render(); }
					butts[pev].mouseIn();
				}

				SDL_Rect cl = {0,0,2*SPACE,SPACE};

				//sheet.render(SPACE / 2, SPACE / 2,&cl, 2*SPACE, SPACE,90);

				sheet.render(0, 0);
				//blank.render(0, SPACE * 6,NULL, NULL, NULL, 0.0);
				
				//SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);
				//SDL_RenderDrawCircle(gRenderer, 300, 300, 100);
				
				SDL_RenderPresent(gRenderer);
				
			}
		}
		close();
		return 0;
	}

}