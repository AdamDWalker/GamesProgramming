#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "sprite.h"

#else // NOT compiling on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

std::string exeName;
SDL_Window *win; //pointer to the SDL_Window
SDL_Renderer *ren; //pointer to the SDL_Renderer
SDL_Surface *surface; //pointer to the SDL_Surfaced
SDL_Texture *tex; //pointer to the SDL_Texture
SDL_Surface *messageSurface; //pointer to the SDL_Surface for message
SDL_Texture *messageTexture; //pointer to the SDL_Texture for message
SDL_Rect message_rect; //SDL_rect for the message

sprite* player;

const int screenW = 800;
const int screenH = 600;

float moveX = 0.0f;
float moveY = 0.0f;
float moveSpeed = 25.0f;
float gravity = 10.0f;
bool isJumping = false; // ====================================== Do this later - Probably replace with an enum ====================================
bool onLadder = false; // This will determine if the up and down keys work
bool isFullscreen = false;
bool done = false;

std::vector<sprite*> sprites; // This may be causing some problems I'm not sure
sprite* tileSprite;
Mix_Chunk* jumpEffect;
float dt = 0.0f;

int tileMap[27][20] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 3, 2, 0, 4, 0, 0, 0, 3, 4, 0, 0, 0, 0, 3 },
						{ 0, 0, 0, 0, 0, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1 },
						{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	 					{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 3, 0, 0, 2, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 3, 2, 0, 4, 0, 2, 0, 1, 1, 0, 0, 0, 0, 4, 0, 3, 0, 0 },
						{ 0, 0, 1, 2, 1, 1, 1, 2, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0 },
						{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0 },
						{ 0, 3, 0, 2, 0, 4, 0, 2, 0, 0, 0, 2, 0, 3, 4, 0, 2, 0, 3, 0 },
						{ 0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0 },
						{ 0, 0, 4, 0, 3, 0, 0, 2, 0, 0, 0, 2, 0, 4, 0, 0, 2, 0, 0, 0 },
						{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };

void drawTileMap()
{
	float spriteW = screenW / 20;
	float spriteH = screenH / 27;
	float spriteX = 0.0f;
	float spriteY = 0.0f;
	int count = 0;
	for (int i = 0; i <= 26; i++)
	{
		//std::cout << std::endl;
		for (int j = 0; j <= 19; j++)
		{
			//std::cout << std::to_string(tileMap[i][j]);
			if (tileMap[i][j] == 1)
			{
				// This is a platform
				tileSprite = new sprite("./assets/block.png", spriteX, spriteY, spriteW, spriteH, ren);
				tileSprite->type = sprite::platform;
				sprites.push_back(tileSprite);
			}
			else if (tileMap[i][j] == 2)
			{
				// This is a ladder
				tileSprite = new sprite("./assets/ladder.png", spriteX, spriteY, spriteW, spriteH, ren);
				sprites.push_back(tileSprite);
			}
			else if (tileMap[i][j] == 3)
			{
				// This is an egg
				tileSprite = new sprite("./assets/egg.png", spriteX, spriteY, spriteW, spriteH, ren);
				tileSprite->type = sprite::egg;
				sprites.push_back(tileSprite);
			}
			else if (tileMap[i][j] == 4)
			{
				// This is the grain
				tileSprite = new sprite("./assets/grain.png", spriteX, spriteY, spriteW, spriteH, ren);
				sprites.push_back(tileSprite);
			}

			if (j == 19) //Final block therefore it needs resetting to 0
			{
				spriteX = 0.0f;
				spriteY += spriteH;
			}
			else // If not the last block then just move along one
			{
				spriteX += spriteW;
			}
		}
		
	}
	//std::cout << std::endl;
	//std::cout << std::to_string(sprites.size());
}

float getDeltaTime()
{
	auto t1 = Clock::now();
	auto t2 = Clock::now();
	float nano = (float)((t2 - t1).count());
	float dt = nano / 1000000000;
	//std::cout << "Delta t2-t1: "
	//	<< dt
	//	<< " nanoseconds" << std::endl;
	return dt;
}

bool boundaryCollide(sprite* object)
{
	float playerMinX = player->rect.x;
	float playerMinY = player->rect.y;
	float playerMaxX = playerMinX + player->rect.w;
	float playerMaxY = playerMinY + player->rect.h;

	float objectMinX = object->rect.x;
	float objectMinY = object->rect.y;
	float objectMaxX = objectMinX + object->rect.w;
	float objectMaxY = objectMinY + object->rect.h;
	if ((((playerMinX > objectMinX) && (playerMinX < objectMaxX)) && ((playerMaxY > objectMinY) && (playerMaxY < objectMaxY))) ||
		(((playerMaxX > objectMinX) && (playerMaxX < objectMaxX)) && ((playerMaxY > objectMinY) && (playerMaxY < objectMaxY ))))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void checkCollision(sprite* object)
{

	switch (object->type)
	{
		case sprite::platform:
			// This should check that the bottom of the player is between the top/bottom of the block and also the sides 
			if (boundaryCollide(object))
			{
				//player->rect.y = object->rect.y - player->rect.h; // Triggered mode engaged
				player->isGrounded = true;
				gravity = 0.0f;
				//std::cout << "Collide";
			}
			break;
		case sprite::egg:
			if (boundaryCollide(object))
			{
				player->playerScore++;
				std::cout << "Score: " << player->playerScore << std::endl;
				// Destroy object
				// Increase score
			}
			break;
		default:
			break;
	}
}


void handleInput()
{
	//Event-based input handling
	//The underlying OS is event-based, so **each** key-up or key-down (for example)
	//generates an event.
	//  - https://wiki.libsdl.org/SDL_PollEvent
	//In some scenarios we want to catch **ALL** the events, not just to present state
	//  - for instance, if taking keyboard input the user might key-down two keys during a frame
	//    - we want to catch based, and know the order
	//  - or the user might key-down and key-up the same within a frame, and we still want something to happen (e.g. jump)
	//  - the alternative is to Poll the current state with SDL_GetKeyboardState

	SDL_Event event; //somewhere to store an event

	//NOTE: there may be multiple events per frame
	while (SDL_PollEvent(&event)) //loop until SDL_PollEvent returns 0 (meaning no more events)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			done = true; //set donecreate remote branch flag if SDL wants to quit (i.e. if the OS has triggered a close event,
							//  - such as window close, or SIGINT
			break;

			//keydown handling - we should to the opposite on key-up for direction controls (generally)
		case SDL_KEYDOWN:
			//Keydown can fire repeatable if key-repeat is on.
			//  - the repeat flag is set on the keyboard event, if this is a repeat event
			//  - in our case, we're going to ignore repeat events
			//  - https://wiki.libsdl.org/SDL_KeyboardEvent
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{
					//hit escape to exit
					case SDLK_ESCAPE: done = true;

					case SDLK_d: 
						// Move player right 
						player->playerState = sprite::movingRight;
						moveX = 10.0f; // This doesn't work when set to 1 or 5 for some reason
						break;

					case SDLK_a:
						// Move square left
						player->playerState = sprite::movingLeft;
						moveX = -10.0f; // This works when -1 
						break;

					case SDLK_SPACE:
						//moveY = -10.0f; // This is negative because the origin is top left so negative goes up
						Mix_PlayChannel(-1, jumpEffect, 0);
						player->playerState = sprite::jumping;
						player->rect.y -= 40.0f;
						break;

					case SDLK_w:
						//if (onLadder)
						player->playerState = sprite::climbUp;
						{
							moveY = -10.0f;
						}
						break;

					case SDLK_s:
						player->playerState = sprite::climbDown;
						moveY = 10.0f;
						break;

					case SDLK_b:
						isFullscreen = !isFullscreen; // Invert each time on keypress
						break;
				}
			break;


		case SDL_KEYUP:
			//Keydown can fire repeatable if key-repeat is on.
			//  - the repeat flag is set on the keyboard event, if this is a repeat event
			//  - in our case, we're going to ignore repeat events
			//  - https://wiki.libsdl.org/SDL_KeyboardEvent
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{
					//hit escape to exit
					case SDLK_ESCAPE: done = true;

					case SDLK_d:
						player->playerState = sprite::idle;
						moveX = 0.0f;
						break;

					case SDLK_a:
						player->playerState = sprite::idle;
						moveX = 0.0f;
						break;

					case SDLK_w:
						player->playerState = sprite::idle;
						moveY = 0.0f;
						break;

					case SDLK_s:
						player->playerState = sprite::idle;
						moveY = 0.0f;
						break;
				}
			break;
		}
	}
}
// end::handleInput[] 

// tag::updateSimulation[]
void updateSimulation(double simLength = 0.02) //update simulation with an amount of time to simulate for (in seconds)
{
	dt = getDeltaTime();
	player->movement();
	player->rect.x += moveX * simLength * moveSpeed;
	//player->rect.y += moveY * simLength * moveSpeed;    // These have been moved into the if statement
	//player->rect.y += gravity * simLength * moveSpeed;  // below but I'll keep them around anyway

	for (auto object : sprites)
	{
		checkCollision(object);
	}

#pragma region On Ladder Move Check
	if (onLadder)
	{
		gravity = 0.0f;
		player->rect.y += moveY * simLength * moveSpeed; // This should work for now
	}
	else
	{
		player->rect.y += gravity * simLength * moveSpeed; // This will need chagning up a bit to account for player jumping and stuff
	}
#pragma endregion

#pragma region Platform collision
	// If the bottom of the player (rect.y + rect.h) > top of platform (rect.y) that's a collision.
	// Test for collision with the platform
	if ((player->rect.y + player->rect.h) >= screenH - 1)
	{
		gravity = 0.0f;
		//std::cout << "Collision Detected" << std::endl;
	}
	else
	{
		gravity = 10.0f;
	}
#pragma endregion

#pragma region On Ladder Collision
	// Test for on ladder
	// if right of player (rext.x + rect.w) < ladder right (rect.x + rect.w) && left (rect.x) > ladder left (rect.x)
	// then it's within the ladder bounds.
	/*if ((player->rect.x + player->rect.w) < (ladderRect.x + ladderRect.w) && (player->rect.x > ladderRect.x) && (player->rect.y + player->rect.h) < (ladderRect.y + ladderRect.h + 1.0f)) // The 1.0f allows the player to be 1 px higher and then the logic works. == Should fix this later but cba rn 25/4/16
	{
		onLadder = true;
		//gravity = 0.0f;
	}
	else
	{
		onLadder = false;
		//gravity = 10.0f;
	}*/
#pragma endregion


	// Check if player is colliding with the egg at all
	// if player right is between egg left and right OR player left is between egg left and right
	// The top must also be above the bottom of the egg or the bottom below the top of the egg, so that
	// it is vertically in the correct place too
	/*if (((((player->rect.x + player->rect.w) > eggRect.x) &&									//
		(player->rect.x + player->rect.w) < (eggRect.x + eggRect.w)) ||							// This is probably overly complicated
		(((player->rect.x) > eggRect.x) && (player->rect.x) < (eggRect.x + eggRect.w))) &&		// It seems to work for now although the 
		(((player->rect.y) < (eggRect.y + eggRect.h)) ||										// vertical stuff might be broken. I will come 
		((player->rect.y + player->rect.h) > eggRect.y)))										// back and fix this later
	{																							//
		std::cout << "Collide" << std::endl;
	}*/
}

void render()
{
		//First clear the renderer
		SDL_RenderClear(ren);

		//Draw the texture
		for (auto object : sprites)
		{
			object->render(ren);
		}

		//Draw the text
		SDL_RenderCopy(ren, messageTexture, NULL, &message_rect);

		player->render(ren);
		//Update the screen
		SDL_RenderPresent(ren);
		SDL_RenderSetLogicalSize(ren, screenW, screenH);
		if (isFullscreen)
		{
			SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
		}
		else
		{
			SDL_SetWindowFullscreen(win, NULL);
		}
}

void cleanExit(int returnValue)
{
	if (messageTexture != nullptr) SDL_DestroyTexture(messageTexture);
	if (jumpEffect != nullptr) Mix_FreeChunk(jumpEffect);
	if (tex != nullptr) SDL_DestroyTexture(tex);
	if (ren != nullptr) SDL_DestroyRenderer(ren);
	if (win != nullptr) SDL_DestroyWindow(win);
	SDL_Quit();
	exit(returnValue);
}

// based on http://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world/
int main( int argc, char* args[] )
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDL initialised OK!\n";

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cout << "SDL_Mixer init error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDP_Mixer Initialised OK!" << std::endl;
	//create window
	win = SDL_CreateWindow("SDL Hello World!", 100, 100, screenW, screenH, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	//error handling
	if (win == nullptr)	
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDL CreatedWindow OK!\n";

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr)
	{
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	player = new sprite("./assets/red_square.png", 150.0f, 150.0f, 24.0f, 32.0f, ren);
	player->playerScore = 0;

	// This is formatted in the same way to the default code from John as it makes sense and I want to stick to it.
	jumpEffect = Mix_LoadWAV("./assets/jump.wav");
	if (jumpEffect == nullptr)
	{
		std::cout << "Mix_LoadWAV Error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}

	if( TTF_Init() == -1 )
	{
		std::cout << "TTF_Init Failed: " << TTF_GetError() << std::endl;
		cleanExit(1);
	}

	TTF_Font* sans = TTF_OpenFont("./assets/Hack-Regular.ttf", 96);
	if (sans == nullptr)
	{
		std::cout << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
		cleanExit(1);
	}
	SDL_Color White = { 255, 255, 255 };
	SDL_Color Purple = { 165, 0, 220 };
	messageSurface = TTF_RenderText_Solid(sans, "Score: ", Purple);
	messageTexture = SDL_CreateTextureFromSurface(ren, messageSurface);
	message_rect.x = 0;
	message_rect.y = 0;
	message_rect.w = 180;
	message_rect.h = 60;

	drawTileMap();
	for (auto thing : sprites)
	{
		//std::cout << std::to_string(thing->rect.x) << " " << std::to_string(thing->rect.y) << " " << std::to_string(thing->rect.w) << " " << std::to_string(thing->rect.h) << std::endl;
	}

	while (!done) //loop until done flag is set)
	{
		handleInput(); // this should ONLY SET VARIABLES

		updateSimulation(); // this should ONLY SET VARIABLES according to simulation

		render(); // this should render the world state according to VARIABLES

		SDL_Delay(20); // unless vsync is on??
	}

	cleanExit(0);
	return 0;
}
