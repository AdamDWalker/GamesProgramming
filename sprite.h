#pragma once

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

#else // NOT compiling on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

class sprite
{
public:
	sprite(std::string imgPath, float rectX, float rectY, float rectW, float rectH, SDL_Renderer* ren);
	~sprite();

	SDL_Surface* surface;
	SDL_Texture* texture;
	SDL_Rect rect;
};

