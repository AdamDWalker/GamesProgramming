#include "sprite.h"


//
// Standard constructor for now
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

// Not sure what to put in here at the moment - Will come back to
sprite::~sprite()
{
}

void sprite::render(SDL_Renderer * ren, SDL_Texture * texture, SDL_Rect * srcRect, SDL_Rect * dstRect)
{
	SDL_RenderCopy(ren, texture, srcRect, dstRect);
}
