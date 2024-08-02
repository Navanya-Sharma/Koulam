/*
// Drawing a circle
void DrawFilledCircle(SDL_Renderer* renderer, int centreX, int centreY, int radius) {
	for (int w = 0; w < radius * 2; w++) {
		for (int h = 0; h < radius * 2; h++) {
			int dx = radius - w; // horizontal offset
			int dy = radius - h; // vertical offset
			if ((dx * dx + dy * dy) <= (radius * radius)) {
				SDL_RenderDrawPoint(renderer, centreX + dx, centreY + dy);
			}
		}
	}
}


*/