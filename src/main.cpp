#include <iostream>
#include <vector>
#include <algorithm>
#include <string>


#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
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

// ========================================================== These are test stuff so I should rename and clean these up at some point ================================================================
SDL_Surface *testSurface; //
SDL_Texture *testTexture; // These are for the player 
SDL_Rect testRect;		  //

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

sprite player("./assets/red_square.png", 150, 150, 30, 30, ren);

float moveX = 0.0f;
float moveY = 0.0f;
float moveSpeed = 25.0f;
float gravity = 10.0f;
bool isJumping = false; // ====================================== Do this later - Probably replace with an enum ====================================
bool onLadder = false; // This will determine if the up and down keys work
bool done = false;

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
						// Move square right
						moveX = 10.0f; // This doesn't work when set to 1 or 5 for some reason
						break;

					case SDLK_a:
						// Move square left
						moveX = -10.0f; // This works when -1 
						break;

					case SDLK_SPACE:
						//moveY = -10.0f; // This is negative because the origin is top left so negative goes up
						testRect.y -= 40.0f;
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
/*
// This is early in the program so that objects can be created here and the function called in main.
// Hopefully this should give them a good place to group together too and still work properly :/
void createObjects()
{
	sprite player("./assets/red_square.png", 150.0f, 150.0f, 30.0f, 30.0f, ren);
}*/

// tag::updateSimulation[]
void updateSimulation(double simLength = 0.02) //update simulation with an amount of time to simulate for (in seconds)
{

	testRect.x += moveX * simLength * moveSpeed;
	//testRect.y += moveY * simLength * moveSpeed;    // These have been moved into the if statement
	//testRect.y += gravity * simLength * moveSpeed;  // below but I'll keep them around anyway

#pragma region On Ladder Move Check
	if (onLadder)
	{
		gravity = 0.0f;
		testRect.y += moveY * simLength * moveSpeed; // This should work for now
	}
	else
	{
		testRect.y += gravity * simLength * moveSpeed; // This will need chagning up a bit to account for player jumping and stuff
	}
#pragma endregion

#pragma region Platform collision
	// If the bottom of the player (rect.y + rect.h) > top of platform (rect.y) that's a collision.
	// Test for collision with the platform
	if ((testRect.y + testRect.h) >= floorRect.y)
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
	if ((testRect.x + testRect.w) < (ladderRect.x + ladderRect.w) && (testRect.x > ladderRect.x) && (testRect.y + testRect.h) < (ladderRect.y + ladderRect.h + 1.0f)) // The 1.0f allows the player to be 1 px higher and then the logic works. == Should fix this later but cba rn 25/4/16
	{
		onLadder = true;
		//gravity = 0.0f;
	}
	else
	{
		onLadder = false;
		//gravity = 10.0f;
	}
#pragma endregion


	// Check if player is colliding with the egg at all
	// if player right is between egg left and right OR player left is between egg left and right
	// The top must also be above the bottom of the egg or the bottom below the top of the egg, so that
	// it is vertically in the correct place too
	if (((((testRect.x + testRect.w) > eggRect.x) &&									//
		(testRect.x + testRect.w) < (eggRect.x + eggRect.w)) ||							// This is probably overly complicated
		(((testRect.x) > eggRect.x) && (testRect.x) < (eggRect.x + eggRect.w))) &&		// It seems to work for now although the 
		(((testRect.y) < (eggRect.y + eggRect.h)) ||									// vertical stuff might be broken. I will come 
		((testRect.y + testRect.h) > eggRect.y)))										// back and fix this later
	{																					//
		std::cout << "Collide" << std::endl;
	}
}

void render()
{

		//First clear the renderer
		SDL_RenderClear(ren);

		//Draw the texture
		SDL_RenderCopy(ren, floorTexture, NULL, &floorRect);
		SDL_RenderCopy(ren, ladderTexture, NULL, &ladderRect);
		SDL_RenderCopy(ren, eggTexture, NULL, &eggRect);
		//player.render(ren, player.texture, NULL, &player.rect);
		//SDL_RenderCopy(ren, player.texture, NULL, &player.rect);
		//SDL_RenderCopy(ren, testTexture, NULL, &testRect);

		//Draw the text
		SDL_RenderCopy(ren, messageTexture, NULL, &message_rect);

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
	win = SDL_CreateWindow("SDL Hello World!", 100, 100, 600, 600, SDL_WINDOW_SHOWN);

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

	std::string imagePath = "./assets/blue_square.jpg";
	surface = IMG_Load(imagePath.c_str());
	if (surface == nullptr){
		std::cout << "SDL IMG_Load Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	floorTexture = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);
	if (floorTexture == nullptr){
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	// ===================================================== PLAYER STUFF HERE ====================================================
	std::string imgPath = "./assets/red_square.png";
	testSurface = IMG_Load(imgPath.c_str());
	if (testSurface == nullptr) {
		std::cout << "SDL IMG_Load Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	testTexture = SDL_CreateTextureFromSurface(ren, testSurface);
	SDL_FreeSurface(testSurface);
	if (testTexture == nullptr) {
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	//sprite player("./assets/red_square.png", 150.0f, 150.0f, 30.0f, 30.0f, ren);
	//createObjects();

	// ==================================================== PLAYER STUFF ENDS HERE ================================================
	std::string imPath = "./assets/green_square.png";
	ladderSurface = IMG_Load(imPath.c_str());
	if (ladderSurface == nullptr) {
		std::cout << "SDL IMG_Load Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	ladderTexture = SDL_CreateTextureFromSurface(ren, ladderSurface);
	SDL_FreeSurface(ladderSurface);
	if (ladderTexture == nullptr) {
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	std::string Path = "./assets/egg.png";
	eggSurface = IMG_Load(Path.c_str());
	if (eggSurface == nullptr) {
		std::cout << "SDL IMG_Load Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	eggTexture = SDL_CreateTextureFromSurface(ren, eggSurface);
	SDL_FreeSurface(eggSurface);
	if (eggTexture == nullptr) {
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
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
	messageSurface = TTF_RenderText_Solid(sans, "Get Chuckie Egged m9", Purple);
	messageTexture = SDL_CreateTextureFromSurface(ren, messageSurface);
	message_rect.x = 0;
	message_rect.y = 0;
	message_rect.w = 180;
	message_rect.h = 60;

	testRect.x = 150;
	testRect.y = 150;
	testRect.w = 30;
	testRect.h = 30;

	floorRect.x = 0;
	floorRect.y = 450;
	floorRect.w = 600;
	floorRect.h = 50;

	ladderRect.x = 250;
	ladderRect.y = 150;
	ladderRect.w = 60;
	ladderRect.h = 300;

	eggRect.x = 350;
	eggRect.y = 400;
	eggRect.w = 40;
	eggRect.h = 50;

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
