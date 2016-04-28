#include "enemy.h"

enemy::enemy(std::string imgPath, float rectX, float rectY, float rectW, float rectH, SDL_Renderer *ren) : sprite(imgPath, rectX, rectY, rectW, rectH, ren)
{
}


enemy::~enemy()
{

}

void enemy::render(SDL_Renderer *ren, bool isAnimated) : sprite(ren, isAnimated)
{
	SDL_RenderCopyEx(ren, texture, &srcRect, &rect, NULL, NULL, flipSprite);
}