#include "sprite.h"


//
// 
//
sprite::sprite(std::string imgPath, float rectX, float rectY, float rectW, float rectH, SDL_Renderer* ren)
{
	rect.x = rectX;
	rect.y = rectY;
	rect.w = rectW;
	rect.h = rectH;


	surface = IMG_Load(imgPath.c_str());
	if (surface == nullptr) {
		std::cout << "SDL IMG_Load Error: " << SDL_GetError() << std::endl;
		//cleanExit(1);
	}

	texture = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);
	if (texture == nullptr) {
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		//cleanExit(1);
	}

}


sprite::~sprite()
{
}
