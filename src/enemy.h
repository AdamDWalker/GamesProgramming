#include "sprite.h"

class enemy :
	public sprite
{
public:
	enemy(std::string imgPath, float rectX, float rectY, float rectW, float rectH, SDL_Renderer *ren);
	~enemy();

	enum enemyState {moveLeft, moveRight};
	enemyState state;
	void moveEnemy(float target1X, float target1Y, float target2X, float target2Y);
	void animateEnemy();
	float speed = 1.0f;

};