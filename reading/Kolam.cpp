#include<stdio.h>
#include<SDL.h>
#include<SDL_image.h>
#include<string>
#include<SDL_mixer.h>
#include<iostream>
#include<SDL_ttf.h>
#include <functional> 

//Classes
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
		void render(int x, int y, SDL_Rect* clip = NULL, int desW = NULL, int desH = NULL, double angle = 0.0, int pop = 0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
		int getHeight();
		int getWidth();
		bool loadFromRenderedText(std::string textureText, SDL_Color textColor, int textWidth, int bold=0);

	private:
		SDL_Texture* text;
		int wd;
		int ht;

};
class button {
	public:
		button();
		~button();
		void setPosition(int x, int y, buttonType w);
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
class InputBox {
	public:
		InputBox();
		void makeBox(int x=0, int y=0, int w=50, int h=10, Texture *text=NULL);
		int getPosX();
		int getPosY();
		int getHeight();
		int getWidth();
		void render(int center=0);
	protected:
		SDL_Rect box;
		Texture* TextTexure;
};
class OtherButton: public InputBox
{
public:
	OtherButton();
	void click(char a,char d);
	void makeButton(int x = 0, int y = 0, int w = 50, int h = 10, Texture* text = NULL, std::function<void(char,char)> func = nullptr);
	void render(SDL_Color color,int center = 0);
	bool isInside();
private:
	std::function<void(char,char)> onClick; // Function pointer
};

//Declarations
int SPACE, cols, rows, offx, offy, SCREEN_WIDTH, SCREEN_HEIGHT, TOTAL_BUTTONS, MaxThick;
float thick;
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
Texture dot, sheetUD, sheetLR, RowText, ColText, NameText, WelText, EnterName, plus,minus,RowNum,ColNum,LetsDraw;
InputBox NameBox, RowBox, ColBox;
OtherButton ButRowPlus, ButRowMinus, ButColPlus, ButColMinus, LetDrawBut;
SDL_Rect ImgUD[2];
SDL_Rect ImgLR[2];
button * butts;
Mix_Music* music = NULL;
Mix_Chunk* buttSound = NULL;
TTF_Font* gFont = NULL;
TTF_Font* BoldFont = NULL;

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
void Texture::render(int x, int y, SDL_Rect* clip, int desW, int desH, double angle,int pop, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect des = { x,y,desW,desH };
	if (desW == NULL) {
		des.w = wd;
	}
	if (desH == NULL) {
		des.h = ht;
	}
	if (SDL_RenderCopyEx(gRenderer, text, clip, &des, angle, center, flip) != 0) {
		printf("Could not Render Texture Eror: %s\n", SDL_GetError());
	}
}
int Texture::getHeight() {
	return ht;
}
int Texture::getWidth() {
	return wd;
}
bool Texture::loadFromRenderedText(std::string textureText, SDL_Color textColor, int textWidth, int bold){
	
	//Render text surface
	//SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	SDL_Surface* textSurface = NULL;

	if (bold == 1) {
		textSurface=TTF_RenderText_Solid_Wrapped(BoldFont, textureText.c_str(), textColor, textWidth);
	}
	else {
		textSurface = TTF_RenderText_Solid_Wrapped(gFont, textureText.c_str(), textColor, textWidth);
	}
	
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		text = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (text == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			wd = textSurface->w;
			ht = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return text != NULL;
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
button::~button() {
	pos = { NULL,NULL };
	place = left;
	cur = NULL;
	bw = NULL;
	bh = NULL;
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

//INPUT BOX
InputBox::InputBox() {
	box = { 0,0,0,0 };
	TextTexure = NULL;
}
void InputBox::makeBox(int x,int y,int w,int h, Texture *text) {
	box = { x,y,w,h };
	TextTexure = text;
}
int InputBox::getHeight() {
	return box.h;
}
int InputBox::getWidth() {
	return box.w;
}
void InputBox::render(int center) {
	SDL_SetRenderDrawColor(gRenderer, 220, 220, 220, 255);
	SDL_RenderFillRect(gRenderer, &box);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(gRenderer, &box);
	SDL_RenderPresent(gRenderer);
	if (center) {
		TextTexure->render(box.x + (box.w-TextTexure->getWidth())/2, box.y);
	}
	else {
		TextTexure->render(box.x + 5, box.y);
	}
	printf("render Input box\n");
}
int InputBox::getPosX() {
	return box.x;
}
int InputBox::getPosY() {
	return box.y;
}

//Other Button Functions

OtherButton::OtherButton() {
	box = { 0,0,0,0 };
	TextTexure = NULL;
}
void OtherButton::makeButton(int x , int y , int w, int h, Texture* text, std::function<void(char,char)> func) {
	box = { x,y,w,h };
	TextTexure = text;
	onClick=func;
}
void OtherButton::render(SDL_Color color,int center) {
	SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(gRenderer, &box);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(gRenderer, &box);
	SDL_RenderPresent(gRenderer);
	if (TextTexure == NULL) {
		return;
	}
	else if(center) {
		TextTexure->render(box.x + (box.w - TextTexure->getWidth()) / 2, box.y+(box.h-TextTexure->getHeight())/2);
	}
	else {
		TextTexure->render(box.x + 5, box.y);
	}
}
bool OtherButton::isInside() {
	int x, y;
	SDL_GetMouseState(&x, &y);
	if (x > box.x && x<(box.x + box.w) && y>box.y && y < (box.y + box.h)) {
		render({ 150,150, 150, 0 },1);
		return true;
	}
	else {
		render({ 100,100, 100, 0 },1);
		return false;
	}
}
void OtherButton::click(char a,char d) {
		if (onClick) {
			onClick(a,d);
		}
		else {
			std::cout << "No function assigned.\n";
		}
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
void addsub(char a,char d) {
	switch (a){
	case 'r':
		switch (d){
		case '+': cols += 1; break;
		case '-': cols -= 1;break;}
		cols = SDL_clamp(cols, 1, 15); 
		break;
	case 'c':
		switch (d){
		case '+':rows += 1;break;
		case '-':rows -= 1;break;}
		rows = SDL_clamp(rows, 1, 15); 
		break;
	}
	printf("cols %d,rows %d\n", cols,rows);
}
void globeDec(int r, int c) {
	cols= (cols + 15 - abs(cols - 15)) / 2; // find min
	rows = (rows + 10 - abs(rows - 10)) / 2; // find min
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
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}
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

		
		if (TTF_Init() == -1)
		{
			printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
			pass = false;
		}

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
			pass = false;
		}
	}

	return pass;
}
bool load( int scene=0) {
	bool pass = true;
	switch (scene)
	{
		case 1:
	{
		//Make Textures - Scene 1
		if (!dot.createsheetLR(SPACE / 3, SPACE / 3, SDL_TEXTUREACCESS_TARGET)) {
			printf("Failed to load button sprite\n");
			pass = false;
		}
		else {
			dot.setAsRenderTarget();
			SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0XFF);
			SDL_RenderClear(gRenderer);
			SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);

			SDL_RenderFillCircle(gRenderer, SPACE / 6, SPACE / 6, SPACE / 7);
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
				rec.x = x - thick / 2; rec.y = y;
				SDL_RenderFillRect(gRenderer, &rec);
			}
			for (float x = 2 * SPACE;x < 3 * SPACE;x += 1) {
				y = x - 2 * SPACE;
				rec.x = x - thick / 2; rec.y = y;
				SDL_RenderFillRect(gRenderer, &rec);
			}
			//Draws the circle
			SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);
			for (int x = -thick * 0.3; x < 0.4 * thick; x++) {
				SDL_RenderDrawCircle(gRenderer, 2 * SPACE, 3 * SPACE, (1.414 * SPACE) + x);
			}

			SDL_SetRenderTarget(gRenderer, NULL);

			ImgUD[0] = { 0,0, SPACE * 4, SPACE };
			ImgUD[1] = { 0, SPACE, SPACE * 4, SPACE };
		}
		if (!sheetLR.createsheetLR(2 * (SPACE + thick), 2 * SPACE, SDL_TEXTUREACCESS_TARGET)) {
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
			for (float x = sheetLR.getWidth() / 2;x < sheetLR.getWidth();x += 1) {
				y = -SPACE + x - 2 * thick;
				rec.x = x; rec.y = y - thick / 2;
				SDL_RenderFillRect(gRenderer, &rec);
			}
			for (float x = sheetLR.getWidth() / 2;x < sheetLR.getWidth();x += 1) {
				y = 3 * SPACE - x + thick * 2;
				rec.x = x; rec.y = y - thick / 2;
				SDL_RenderFillRect(gRenderer, &rec);
			}
			//Draws the circle
			SDL_SetRenderDrawColor(gRenderer, 0XFF, 0xFF, 0xFF, 0xFF);
			for (int x = -0.3 * thick; x < 0.4 * thick; x++) {
				SDL_RenderDrawCircle(gRenderer, -SPACE + thick, SPACE, (1.414 * SPACE) + x);
			}

			SDL_SetRenderTarget(gRenderer, NULL);
			ImgLR[1] = { 0,0, sheetLR.getWidth() / 2, sheetLR.getHeight() };
			ImgLR[0] = { sheetLR.getWidth() / 2,0, sheetLR.getWidth() / 2, sheetLR.getHeight() };
		}

	}
		break;
		default:
	{
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

		//Load TTF - Scene 0

		gFont = TTF_OpenFont("Font/roboto/Roboto-Thin.ttf", 28);
		BoldFont = TTF_OpenFont("Font/roboto/Roboto-Regular.ttf", 44);
		if (gFont == NULL || BoldFont == NULL)
		{
			printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
			pass = false;
		}
		else
		{
			//Welcome text
			if (!WelText.loadFromRenderedText(" Welcome  To Digital Kolam!", { 0,0,0,0 }, SCREEN_WIDTH, 1))
			{
				printf("Failed to render Weltext texture!\n");
				pass = false;
			}
			if (!RowText.loadFromRenderedText("No. of Rows: ", { 0,0,0,0 }, SCREEN_WIDTH))
			{
				printf("Failed to render Row text texture!\n");
				pass = false;
			}
			if (!ColText.loadFromRenderedText("No. of Columns: ", { 0,0,0,0 }, SCREEN_WIDTH))
			{
				printf("Failed to render Col text texture!\n");
				pass = false;
			}
			if (!NameText.loadFromRenderedText("Name your Design: ", { 0,0,0,0 }, SCREEN_WIDTH))
			{
				printf("Failed to render Name text texture!\n");
				pass = false;
			}
			if (!EnterName.loadFromRenderedText("Enter Name", { 0,0,0,0 }, SCREEN_WIDTH))
			{
				printf("Failed to render Name text texture!\n");
				pass = false;
			}
			std::string rowsC = std::to_string(rows);
			std::string colsC = std::to_string(cols);

			if (!ColNum.loadFromRenderedText(colsC, { 0,0,0,0 }, SCREEN_WIDTH))
			{
				printf("Failed to render col num texture!\n");
				pass = false;
			}
			if (!RowNum.loadFromRenderedText(rowsC, { 0,0,0,0 }, SCREEN_WIDTH))
			{
				printf("Failed to render row num texture!\n");
				pass = false;
			}
			TTF_SetFontSize(BoldFont, 28);
			if (!plus.loadFromRenderedText("+", { 255,255,255,0 }, SCREEN_WIDTH,1))
			{
				printf("Failed to render + text texture!\n");
				pass = false;
			}
			if (!minus.loadFromRenderedText("-", { 255,255,255,0 }, SCREEN_WIDTH,1))
			{
				printf("Failed to render - text texture!\n");
				pass = false;
			}
			if (!LetsDraw.loadFromRenderedText("Let's Draw!", { 255,255,255,0 }, SCREEN_WIDTH, 1))
			{
				printf("Failed to render - text texture!\n");
				pass = false;
			}
		}
	}
		break;
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

	for (int i = 0; i < TOTAL_BUTTONS; i++) {
		butts[i].~button();
	}
	delete[] butts;
	butts = NULL;
}
void drawdotsButtons() {
	//Freeing the existing buttons
	for (int i = 0; i < TOTAL_BUTTONS; i++) {
		butts[i].~button();
	}
	delete[] butts;
	butts = NULL;

	//Decl
	butts = new button[TOTAL_BUTTONS];
	int a = dot.getWidth();

	for (int ri = 0, x = 2 * SPACE+offx, i=0; ri <cols; ri++, x += 4 * SPACE){
		for (int ci = 0, y = 2 * SPACE+offy;ci < rows; ci++, i += 4, y += 4 * SPACE) {
			
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
	int N = rows * ((x-offx) / (4 * SPACE)) + ((y-offy) / (4 * SPACE));
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

	if (x<offx || x>(offx + cols * 4 * SPACE)) {
		return 0;
	}
	if (y<offy || y>(offy + rows * 4 * SPACE)) {
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

void DrawScene(int scene) {
	if (scene == 0) {
		int pad = 10;
		
		NameBox.makeBox((SCREEN_WIDTH-200+pad+NameText.getWidth())/ 2, SCREEN_HEIGHT / 2 - NameText.getHeight() - pad, 200, NameText.getHeight(), &EnterName);
		RowBox.makeBox(NameBox.getPosX()+50,SCREEN_HEIGHT/2,100, NameText.getHeight(),&RowNum);
		ColBox.makeBox(RowBox.getPosX(),RowBox.getPosY()+ NameText.getHeight() + pad,100, NameText.getHeight(),&ColNum);
	
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderClear(gRenderer);
		NameBox.render();
		RowBox.render(1);
		ColBox.render(1);

		WelText.render(SCREEN_WIDTH / 2 - WelText.getWidth() / 2, SCREEN_HEIGHT / 2 - 2 * NameText.getHeight() - 12 * pad, NULL, NULL, NULL, NULL, 1);
		NameText.render(SCREEN_WIDTH / 2 - (NameText.getWidth() + NameBox.getWidth() + pad) / 2, SCREEN_HEIGHT / 2 - NameText.getHeight() - pad, NULL, NULL, NULL, NULL, 1);
		RowText.render(SCREEN_WIDTH / 2 - (NameText.getWidth() + NameBox.getWidth() + pad) / 2, SCREEN_HEIGHT / 2, NULL, NULL, NULL, NULL, 1);
		ColText.render(SCREEN_WIDTH / 2 - (NameText.getWidth() + NameBox.getWidth() + pad) / 2, SCREEN_HEIGHT / 2 + NameText.getHeight() + pad, NULL, NULL, NULL, NULL, 1);

		ButRowPlus.makeButton(RowBox.getPosX()+ RowBox.getWidth(), RowBox.getPosY(), 50, RowBox.getHeight(),&plus,addsub);
		ButRowMinus.makeButton(RowBox.getPosX()-50, RowBox.getPosY(), 50, RowBox.getHeight(), &minus, addsub);
		ButColPlus.makeButton(ColBox.getPosX()+ColBox.getWidth(), ColBox.getPosY(), 50, ColBox.getHeight(), &plus, addsub);
		ButColMinus.makeButton(ColBox.getPosX()-50, ColBox.getPosY(), 50, ColBox.getHeight(), &minus, addsub);
		LetDrawBut.makeButton(SCREEN_WIDTH / 2 - 75,ColBox.getPosY() + ColBox.getHeight() + 7*pad, 170, ColBox.getHeight()+10, &LetsDraw, addsub);


		ButRowPlus.render({100,100,100,0},1);
		ButRowMinus.render({100,100,100,0},1);
		ButColPlus.render({ 100,100,100,0 },1);
		ButColMinus.render({ 100,100,100,0 },1);
		LetDrawBut.render({ 100,100,100,0 }, 1);


		SDL_RenderPresent(gRenderer);
	}
	else{
		SDL_SetRenderDrawColor(gRenderer, 0xCB, 0x68, 0x43, 0xFF);
		SDL_RenderClear(gRenderer);
		drawdotsButtons();
		SDL_RenderPresent(gRenderer);
		
	}
	
}
void HandleEvent(int *scene, int* ChangeScene, SDL_Event* e) {
	if (*scene == 0) {
		int x, y, pevCM = 0, chgCM;
		SDL_GetMouseState(&x, &y);
		//if (x > ButRowMinus.getPosX() && x<(ButRowPlus.getPosX() + ButRowPlus.getWidth()) && y>ButRowPlus.getPosY() && y < (ButColPlus.getPosY() + ButColPlus.getHeight())) {

			/*chgCM = ButColMinus.isInside() - pevCM;
			pevCM = ButColMinus.isInside();
			if (chgCM) {
				if (pevCM) {
					ButColMinus.render({ 150,150,150,0 }, 1);
				}
				else
				{
					ButColMinus.render({ 100,100,100,0 }, 1);
				}
			}*/

		if (ButRowPlus.isInside() && e->type == SDL_MOUSEBUTTONDOWN)
		{
			ButRowPlus.click('c', '+');
			std::string rowsC = std::to_string(rows);
			RowNum.loadFromRenderedText(rowsC, { 0,0,0,0 }, SCREEN_WIDTH);
			RowNum.render(0, 0);
			printf("RowPlus x y");
			RowBox.render(1);
		}
		if (ButRowMinus.isInside() && e->type == SDL_MOUSEBUTTONDOWN)
		{
			ButRowMinus.click('c', '-');
			std::string rowsC = std::to_string(rows);
			RowNum.loadFromRenderedText(rowsC, { 0,0,0,0 }, SCREEN_WIDTH);
			RowBox.render(1);
		}
		if (ButColPlus.isInside() && e->type == SDL_MOUSEBUTTONDOWN)
		{
			ButColPlus.click('r', '+');
			std::string colsC = std::to_string(cols);
			ColNum.loadFromRenderedText(colsC, { 0,0,0,0 }, SCREEN_WIDTH);
			ColBox.render(1);
		}
		if (ButColMinus.isInside() && e->type == SDL_MOUSEBUTTONDOWN)
		{
			ButColMinus.click('r', '-');
			std::string colsC = std::to_string(cols);
			ColNum.loadFromRenderedText(colsC, { 0,0,0,0 }, SCREEN_WIDTH);
			ColBox.render(1);
		}

		if (LetDrawBut.isInside() && e->type == SDL_MOUSEBUTTONDOWN) {
			*scene = 1;
			*ChangeScene = 1;
		}
	}
	
}


int main(int argc, char* args[]) {	

	//Initial declaration to give init window size
	cols = 3;
	rows = 3;
	thick = 20;
	SCREEN_HEIGHT = 600;
	SCREEN_WIDTH = 1000;
	if (!init()) {
		printf("Failed to run init\n");
	}
	else {
		globeDec(cols, rows);
		if (!load()) {
			printf("Failed to run load.Error: %s\n",SDL_GetError());
		}
		else {
			//Declarations
			bool quit = false;
			SDL_Event e;
			int pev = 0, scene = 0, ChangeScene = 0, chk = 0;
			std::string name = "Enter Name";
			buttonType place;

			//First Scene
			//Mix_PlayMusic(music, -1);
			DrawScene(scene);

			globeDec(cols, rows);
			load(1);

			//Update Loop
			while (!quit)
			{

				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_WINDOWEVENT) {
						if (e.window.event == SDL_WINDOWEVENT_CLOSE) { quit = true; }
						else if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
							SCREEN_WIDTH = e.window.data1;
							SCREEN_HEIGHT = e.window.data2;
							globeDec(cols, rows);
							load();
							DrawScene(scene);
						}
					}
					HandleEvent(&scene,&ChangeScene, &e);
					switch (scene)
					{
					/*case 0: {
						if (e.type == SDL_TEXTINPUT) {
							name += e.text.text;
							//EnterName.loadFromRenderedText(name, { 0,0,0,0 }, SCREEN_WIDTH);
							//EnterName.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - NameText.getHeight() - 10);
						}
						SDL_RenderPresent(gRenderer);
						if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_1) {
							ChangeScene = 1;
							scene = 1;
						}
						break;
					}*/
					case 1:

					{
						butts[pev].mouseOut();
						chk = checkInside(place);
						if (chk == 1) {
							pev = activebuttonID(place);
							if (e.type == SDL_MOUSEBUTTONDOWN) {
								Mix_PlayChannel(-1, buttSound, 0);
								butts[pev].render();
							}
							butts[pev].mouseIn();
						}

						if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_2) {
							ChangeScene = 1;
							scene = 0;
						}
						break;
					}
					}
				}
				if (ChangeScene) {
					if (cols != 3 || rows != 3) {
						globeDec(cols, rows);
						load(1);
					}
					DrawScene(scene);
					ChangeScene = 0;
				}
			}
			
			close();
			return 0;
		}
	}

}
