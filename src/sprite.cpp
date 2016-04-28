#include "sprite.h"

//
// Standard constructor for now
//
sprite::sprite(std::string imgPath, float rectX, float rectY, float rectW, float rectH, SDL_Renderer *ren)
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
	//std::cout << "Created sprite object sucessfully" << std::endl;
}

// Not sure what to put in here at the moment - Will come back to
sprite::~sprite()
{
	SDL_DestroyTexture(texture);
}

void sprite::render(SDL_Renderer *ren)
{
	//std::cout << "Render called" << std::endl;
	SDL_RenderCopy(ren, texture, NULL, &rect );
}

void sprite::render(SDL_Renderer *ren, bool isAnimated)
{
	//std::cout << "Render called" << std::endl;
	SDL_RenderCopyEx(ren, texture, &srcRect, &rect, NULL, NULL, flipSprite);
}

void sprite::movement()
{
	switch (playerState)
	{
	case idle:
		// Position not changing, player just standing still
		srcRect.x = 0.0f;
		animFrameBuffer = 0;
		break;
	case movingLeft:
		flipSprite = SDL_FLIP_HORIZONTAL;
		//srcRect.x = 0.0f;
		if (animFrameBuffer >= bufferMax)
		{
			if (srcRect.x == 54.0f)
			{
				srcRect.x = 0.0f;
			}
			else
			{
				srcRect.x += 27.0f;
			}
			animFrameBuffer = 0;
		}
		// Position should go left
		// Sprite animation for left
		// Audio for moving should play
		break;
	case movingRight:
		flipSprite = SDL_FLIP_NONE;
		//srcRect.x = 0.0f;
		if (animFrameBuffer >= bufferMax)
		{
			if (srcRect.x == 54.0f)
			{
				srcRect.x = 0.0f;
			}
			else
			{
				srcRect.x += 27.0f;
			}
			animFrameBuffer = 0;
		}
		// Position should change to go right
		// Sprite animation for right
		// Audio for moving
		break;
	case climbing:
		//srcRect.x = 81.0f;
		if (animFrameBuffer >= bufferMax)
		{
			if (srcRect.x == 189.0f)
			{
				srcRect.x = 81.0f;
			}
			else
			{
				srcRect.x += 27.0f;
			}
			animFrameBuffer = 0;
		}
		// Position goes up
		// sprite animation for climbing
		// audio for climbing
		break;
	case climbingIdle:
		srcRect.x = 81.0f;
		animFrameBuffer = 0;
		// Position goes down
		// sprite animation for climbing
		// Audio for climbing
		break;
	case jumping:
		// Position goes up a bit then comes back down
		// Audio for jumping
		break;
	case falling:
		// Position changes downward from gravity
		// Falling audio?
		break;
	default:
		break;
	}
}
