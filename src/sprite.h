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
	void render(SDL_Renderer *ren);
	enum state { idle, movingLeft, movingRight, climbUp, climbDown, jumping, falling };
	enum spriteType {player, platform, ladder, egg, grain };
	void movement(); // This goes with the enum stuff in the cpp file, movement may get moved here - if not rename probably
	state playerState = idle;
	spriteType type; // Basically just a tag for sprites to be easily differentiated from one another
	bool isGrounded = false; // Is the player on a platform.
	bool active = true; // This means the egg/grain is still available to be picked up
	int playerScore; // This should move to the player class when I get around to that haha
	

	SDL_Surface* surface;
	SDL_Texture* texture;
	SDL_Rect rect;
};

