#include "sprite.h"

class enemy :
	public sprite
{
public:
	enemy(std::string imgPath, float rectX, float rectY, float rectW, float rectH, SDL_Renderer *ren);
	~enemy();
};