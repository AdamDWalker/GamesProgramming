#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "sprite.h"
#include "vector.h"

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

// ========================================================== These are test stuff so I should rename and clean these up at some point ================================================================
SDL_Surface *testSurface; //
SDL_Texture *testTexture; // These are for the player 

// ========================================================== These are for the blue platform =============================================
SDL_Surface *floorSurface;
SDL_Texture *floorTexture;
SDL_Rect floorRect;

// ========================================================= These are for the green ladder ===================================================
SDL_Surface *ladderSurface;
SDL_Texture *ladderTexture;
SDL_Rect ladderRect;

// ========================================================= These are for the egg ===================================================
SDL_Surface *eggSurface;
SDL_Texture *eggTexture;
SDL_Rect eggRect;

sprite* player;

const int screenW = 800;
const int screenH = 600;

float moveX = 0.0f;
float moveY = 0.0f;
float moveSpeed = 25.0f;
float gravity = 10.0f;
bool isJumping = false; // ====================================== Do this later - Probably replace with an enum ====================================
bool onLadder = false; // This will determine if the up and down keys work
bool done = false;

std::vector<sprite*> sprites; // This may be causing some problems I'm not sure
sprite* platform;

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
				platform = new sprite("./assets/blue_square.jpg", spriteX, spriteY, spriteW, spriteH, ren);
				//platform.render(ren);
				//std::cout << std::to_string(spriteX) << " " << std::to_string(spriteY) << std::endl;
				sprites.push_back(platform);
			}
			else if (tileMap[i][j] == 2)
			{
				// This is a ladder
				platform = new sprite("./assets/green_square.png", spriteX, spriteY, spriteW, spriteH, ren);
				//platform.render(ren);
				//std::cout << std::to_string(spriteX) << " " << std::to_string(spriteY) << std::endl;
				sprites.push_back(platform);
			}
			else if (tileMap[i][j] == 3)
			{
				// This is an egg
				platform = new sprite("./assets/egg.png", spriteX, spriteY, spriteW, spriteH, ren);
				//platform.render(ren);
				//std::cout << std::to_string(spriteX) << " " << std::to_string(spriteY) << std::endl;
				sprites.push_back(platform);
			}

			// Do stuff THEN update positions
			// ...
			// ...

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

void checkCollision(sprite player, sprite object)
{
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
						moveX = -10.0f; // This works when -1 
						break;

					case SDLK_SPACE:
						//moveY = -10.0f; // This is negative because the origin is top left so negative goes up
						player->rect.y -= 40.0f;
						break;

					case SDLK_w:
						//if (onLadder)
						{
							moveY = -10.0f;
						}
						break;

					case SDLK_s:
						moveY = 10.0f;
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
						moveX = 0.0f;
						break;

					case SDLK_a:
						moveX = 0.0f;
						break;

					case SDLK_w:
						moveY = 0.0f;
						break;

					case SDLK_s:
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
	player->movement();
	player->rect.x += moveX * simLength * moveSpeed;
	//player->rect.y += moveY * simLength * moveSpeed;    // These have been moved into the if statement
	//player->rect.y += gravity * simLength * moveSpeed;  // below but I'll keep them around anyway

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
}

void cleanExit(int returnValue)
{
	if (messageTexture != nullptr) SDL_DestroyTexture(messageTexture);
	if (tex != nullptr) SDL_DestroyTexture(tex);
	if (testTexture != nullptr) SDL_DestroyTexture(testTexture); // ===================================== Name change needed here ==================================
	if (floorTexture != nullptr) SDL_DestroyTexture(floorTexture); // =================================== This needs moving too =================================
	if (ladderTexture != nullptr) SDL_DestroyTexture(ladderTexture); // =================================== This needs moving too =================================
	if (eggTexture != nullptr) SDL_DestroyTexture(eggTexture); // =================================== This needs moving too =================================
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

	//create window
	win = SDL_CreateWindow("SDL Hello World!", 100, 100, screenW, screenH, SDL_WINDOW_SHOWN);

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

	player = new sprite("./assets/red_square.png", 150.0f, 150.0f, 30.0f, 30.0f, ren);

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
