#pragma once

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include "sprite.h"

#else // NOT compiling on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

class text
{
public:
	text(SDL_Renderer *ren, int score, float xVal, float yVal, float width, float height, TTF_Font* font, SDL_Color colour);
	text::text(SDL_Renderer *ren, std::string message, float xVal, float yVal, float width, float height, TTF_Font* theFont, SDL_Color theColour);
	~text();
	void render(SDL_Renderer *ren);
	void setScore(SDL_Renderer* ren, int score);
	void setLife(SDL_Renderer* ren, int score);
	bool isShowing = true;

	TTF_Font* font;
	SDL_Color colour;
	SDL_Surface* textSurface;
	SDL_Texture* textTexture;
	SDL_Rect textRect;
};