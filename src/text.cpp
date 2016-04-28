#include "text.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <chrono>


text::text(SDL_Renderer *ren, int score, float xVal, float yVal, float width, float height, TTF_Font* font, SDL_Color colour)
{
	//std::string scoreString= "Score: " + std::to_string(score);

	textRect.x = xVal;
	textRect.y = yVal;
	textRect.w = width;
	textRect.h = height;

	textSurface = TTF_RenderText_Solid(font, "Score: " + score, colour);
	textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
}


text::~text()
{
}

void text::render(SDL_Renderer *ren)
{
	SDL_RenderCopy(ren, textTexture, NULL, &textRect);
}
