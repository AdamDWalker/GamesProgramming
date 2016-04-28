#include "enemy.h"

enemy::enemy(std::string imgPath, float rectX, float rectY, float rectW, float rectH, SDL_Renderer *ren) : sprite(imgPath, rectX, rectY, rectW, rectH, ren)
{
}


enemy::~enemy()
{
	SDL_DestroyTexture(texture);
}

// Target 1 will be it's spawn position
void enemy::moveEnemy(float target1X, float target1Y, float target2X, float target2Y)
{

}