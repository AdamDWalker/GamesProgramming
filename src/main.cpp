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
#include "text.h"

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
text* scoreCount;

const int screenW = 800; // These are just the initial sizes of the screen, it can
const int screenH = 600; // be resized by hand or put into fullscreen mode on keypress

float dt = 0.0f; // This needs a rethink later since it doesn't work at the moment - 9:30pm
float moveX = 0.0f;			//
float moveY = 0.0f;			// These will probably be replaced with a vector object at some point
float moveSpeed = 25.0f;	// The vector should make things easier to manipulate around.
float gravity = 10.0f;		//
bool onLadder = false; // This will determine if the up and down keys work
bool isFullscreen = false; // This is obvious. Why are you reading me for help?
bool done = false; // Is the game still running? This powers the while loop in main.

std::vector<sprite*> sprites; // This is storing the sprites for the tilemap.
sprite* tileSprite; // This is for the sprite objects being created and added to the sprites vector in drawTileMap().
text* loading;

// ======================== Sound Effect stuff can go here =====================
Mix_Chunk* jumpEffect;
// =========================== End of Sound Effects ============================



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

// This runs through the tilemap and creates a sprite object for each
// Position that requires on. These objects are added to a vector and
// those vector elements are ran through in render to display on screen
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
				tileSprite->type = sprite::ladder;
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
				tileSprite->type = sprite::grain;
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

// This is not functional at the moment as it causes problems
// When dt from this is used it stops right movement and 
// Makes left movement incredibly slow.
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

// This takes an object and checks if the player is colliding with it
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
	if ((((playerMinX > objectMinX) && (playerMinX < objectMaxX)) && ((playerMaxY >= objectMinY -1) && (playerMaxY <= objectMaxY +1))) ||
		(((playerMaxX > objectMinX) && (playerMaxX < objectMaxX)) && ((playerMaxY >= objectMinY -1) && (playerMaxY <= objectMaxY +1))))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// This is called in updateSimulation() and is used for collision stuff
// It is mostly about the outcomes for collisions rather than the boundary 
// Things itself, that gets passed to boundaryCollide().
void checkCollision(sprite* object, int count)
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

				player->playerScore += 200; // 200 points for an egg
				scoreCount->setScore(ren, player->playerScore);
				std::cout << "Score: " << player->playerScore << std::endl;
				sprites.erase(sprites.begin() + count);

			}
			break;

		case sprite::ladder:
			if ((player->rect.x > object->rect.x) && ((player->rect.x + player->rect.w) < (object->rect.x + object->rect.w)) &&
				((player->rect.y + player->rect.h) > object->rect.y) && (player->rect.y + player->rect.h) < ((object->rect.y + object->rect.h)))
			//if(boundaryCollide(object))
			{
				onLadder = true;
				//std::cout << "ON LADDER" << std::endl;
			}
			//onLadder = false;
			break;
		case sprite::grain:
			if (boundaryCollide(object))
			{
				player->playerScore += 50; // 50 Points for grain
				scoreCount->setScore(ren, player->playerScore);
				std::cout << "Score: " << player->playerScore << std::endl;
				sprites.erase(sprites.begin() + count);
			}
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
						player->playerState = sprite::climbing;
						gravity = 0.0f;
						moveY = -10.0f;
						break;

					case SDLK_s:
						player->playerState = sprite::climbing;
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
						player->playerState = sprite::climbingIdle;
						gravity = 10.0f;
						moveY = 0.0f;
						break;

					case SDLK_s:
						player->playerState = sprite::climbingIdle;
						gravity = 10.0f;
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

	player->animFrameBuffer++;

	player->rect.x += moveX * moveSpeed * simLength;

	int deletedSpritesCount = 0;
	for (int i = 0; i < sprites.size() - deletedSpritesCount; i++)//auto object : sprites)
	{
		checkCollision(sprites[i], i);
	}

#pragma region On Ladder Move Check
	if (onLadder)
	{
		gravity = 0.0f;
		player->rect.y += moveY * simLength * moveSpeed; // This should work for now
	}
	else
	{
		player->rect.y += gravity * simLength * moveSpeed / 2; // This will need chagning up a bit to account for player jumping and stuff
	}
#pragma endregion

#pragma region Platform collision
	// Check that the player doesnt fall out of the screen
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

	onLadder = false;
}

void render()
{
		//First clear the renderer
		SDL_RenderClear(ren);
		if(loading->isShowing)
			loading->render(ren);


		//Draw the texture
		for (auto object : sprites)
		{
			object->render(ren);
		}

		//Draw the text
		//SDL_RenderCopy(ren, messageTexture, NULL, &message_rect);
		scoreCount->render(ren);

		player->render(ren, true);
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

void Load()
{
	SDL_RenderClear(ren);

	SDL_RenderCopy(ren, loading->textTexture, NULL, &loading->textRect);

	SDL_RenderPresent(ren);

	SDL_Delay(500); 
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

	// Initialise the audio mixer and also explained the variables.
	// 44100 - Common hertz level for these things
	// MIX_DEFAULT_FORMAT - Like the SDL_INIT_EVERYTHING, some init settings
	// 2 - Audio Channels for stereo play
	// 2048 - Sample size, can affect lag time for effect to play
	// != 0 - Same as other init stuff, not sure why, just needs it.
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
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

	player = new sprite("./assets/spritesheet.png", 150.0f, 150.0f, 24.0f, 32.0f, ren);
	player->srcRect.x = 0.0f;
	player->srcRect.y = 96.0f;
	player->srcRect.w = 24.0f;
	player->srcRect.h = 32.0f;

	player->playerScore = 0;

	player->bufferMax = 5;

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
	
	loading = new text(ren, "Loading...", 200, 200, 400, 130, sans, White);
	Load();
	/*messageSurface = TTF_RenderText_Solid(sans, "Score: ", Purple);
	messageTexture = SDL_CreateTextureFromSurface(ren, messageSurface);
	message_rect.x = 0;
	message_rect.y = 0;
	message_rect.w = 180;
	message_rect.h = 60;*/

	scoreCount = new text(ren, player->playerScore, 0, 0, 180, 60, sans, Purple);

	drawTileMap();

	loading->isShowing = false;

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
