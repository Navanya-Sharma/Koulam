#include <stdio.h>
#include<SDL.h>
#include<SDL_image.h>
#include<string>
#include<iostream>


const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;
int SPACE, rows,cols ,offx, offy;
int DOT,TOTAL_BUTTONS;
//const int TOTAL_BUTTONS = (SCREEN_HEIGHT * SCREEN_WIDTH) / (4 * SPACE * SPACE);

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
Texture dot;
Texture sheet;

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
		
		//SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0X17, 0XF6, 0XF1));
		SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 0XFF, 0XFF));
		//SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0X07, 0XFC, 0XFF));
		//SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0X61, 0XFF, 0XFF));

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

	SDL_Rect rec = { pos.x, pos.y, bw, bh };
	SDL_SetRenderDrawColor(gRenderer, 0xF2, 0X7C, 0X50, 0xFF);
	SDL_RenderDrawRect(gRenderer, &rec);
	SDL_RenderPresent(gRenderer);
	

}
void button::handleEvent(SDL_Event* e) {
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN) { // To know when the mouse is inside and clicking
		
		int x, y, w, h;
		//To find out the w,h of the left/right or the bottom/top 
		h = SPACE ;
		w = 2 * SPACE ;
		switch (place)
		{
		case left:
		case right:
			w = SPACE ;
			h = 2 * SPACE;
			break;
		}
		//To check if the mouse is outside the button
		SDL_GetMouseState(&x, &y);
		bool inn = true;
		if (x < pos.x) {
			inn = false;
		}
		else if (x > pos.x + w) {
			inn = false;
		}
		else if (y < pos.y) {
			inn = false;
		}
		else if (y > pos.y + h) {
			inn = false;
		}
		
		//Actions when the mouse is inside
		if (inn) {
			//make the rectangle white to show that the mouse in with in the square
			SDL_Rect a = { pos.x, pos.y , w, h };
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderDrawRect(gRenderer, &a);
			SDL_RenderPresent(gRenderer);

			if (e->type == SDL_MOUSEBUTTONDOWN) { // When the mouse is clicked inside
				cur = (cur + 1) % 3;
				//Clear the screen
				SDL_Rect a = { pos.x, pos.y, w, h };
				SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0xFF);
				SDL_RenderFillRect(gRenderer, &a);
				SDL_RenderPresent(gRenderer);
				//Render the image according to the current sprite
				switch (place)
				{
				case left:
					sheet.render(pos.x - SPACE/2, pos.y + SPACE / 2, &StateImg[cur], 2*SPACE, SPACE, 270.0, NULL, SDL_FLIP_NONE);//left 0,50
					break;
				case bottom:
					sheet.render(pos.x, pos.y, &StateImg[cur], 2 * SPACE, SPACE, 180.0, NULL, SDL_FLIP_NONE);
					break;
				case right:
					sheet.render(pos.x - SPACE / 2, pos.y + SPACE / 2, &StateImg[cur], 2 * SPACE, SPACE, 90.0, NULL, SDL_FLIP_NONE);
					break;
				case top:
					sheet.render(pos.x, pos.y, &StateImg[cur], 2 * SPACE, SPACE, 0.0, NULL, SDL_FLIP_NONE);
					break;
				default:
					break;
				}
			}
			
		}
		else {
			//make the rectangle the default color when the mouse is not inside
			SDL_Rect a = { pos.x, pos.y, w, h };
			SDL_SetRenderDrawColor(gRenderer, 0xF2, 0X7C, 0X50, 0xFF);
			SDL_RenderDrawRect(gRenderer, &a);
			SDL_RenderPresent(gRenderer);
		}
		
	}
}
void button::render() {
	
	cur = (cur + 1) % 3;
	//Clear the screen
	SDL_Rect a = { pos.x, pos.y, bw, bh };
	SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0xFF);
	SDL_RenderFillRect(gRenderer, &a);
	SDL_RenderPresent(gRenderer);
	//so that the white rectangle is still there
	mouseIn();
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
	if(!sheet.LoadTexture("sheet.png")) {
		printf("Failed to load sheet sprite\n");
		pass = false;
	}
	else {
		StateImg[0] = { 0,0, sheet.getWidth(), sheet.getHeight() / 2 };
		StateImg[1] = { 0, (sheet.getHeight() /2) +3, sheet.getWidth(), sheet.getHeight() / 2  };
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
	//butts[i].setPosition(100,100, left);
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
	using namespace std;
	cout << "Enter no. of Dots you want in a Row:";
	cin >> rows;
	cout << "Enter no. of Dots you want in a Colum:";
	cin >> cols;

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
					int i = activebuttonID(place);
					pev = i;
					butts[i].mouseIn();
					if (e.type == SDL_MOUSEBUTTONDOWN) { butts[i].render(); }
				}
				SDL_RenderPresent(gRenderer);
			}
		}
		close();
		return 0;
	}

}