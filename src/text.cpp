#include "text.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <chrono>


text::text(SDL_Renderer *ren, int score, float xVal, float yVal, float width, float height, TTF_Font* theFont, SDL_Color theColour)
{
	//std::string scoreString= "Score: " + std::to_string(score);

	font = theFont;
	colour = theColour;

	textRect.x = xVal;
	textRect.y = yVal;
	textRect.w = width;
	textRect.h = height;

	textSurface = TTF_RenderText_Solid(font, "Score: " + score, colour);
	textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
}

text::text(SDL_Renderer *ren, std::string message, float xVal, float yVal, float width, float height, TTF_Font* theFont, SDL_Color theColour)
{
	//std::string scoreString= "Score: " + std::to_string(score);

	font = theFont;
	colour = theColour;

	textRect.x = xVal;
	textRect.y = yVal;
	textRect.w = width;
	textRect.h = height;

	const char* theMessage = message.c_str();

	textSurface = TTF_RenderText_Solid(font, theMessage, colour);
	textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
}


text::~text()
{
	SDL_DestroyTexture(textTexture);
}

void text::render(SDL_Renderer *ren)
{
	SDL_RenderCopy(ren, textTexture, NULL, &textRect);
}

void text::setScore(SDL_Renderer* ren, int score)
{
	std::string gameScore = "Score: " + std::to_string(score);
	const char* theScore = gameScore.c_str();
	textSurface = TTF_RenderText_Solid(font, theScore, colour);
	SDL_DestroyTexture(textTexture);
	textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
}

void text::setLife(SDL_Renderer* ren, int lives)
{
	std::string gameScore = "Lives: " + std::to_string(lives);
	const char* theLife = gameScore.c_str();
	textSurface = TTF_RenderText_Solid(font, theLife, colour);
	SDL_DestroyTexture(textTexture);
	textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
}
