#pragma once
#include "cocos2d.h"

#define MOVELEFT		0
#define MOVERIGHT		1
#define MOVEACTION		2
#define JUMPACTION		3
#define MOVEDOWN		4
#define SPEEDACTION		5

USING_NS_CC;

class GameRole:public Sprite
{
protected:
	ProgressTimer			*bloodBar;
	bool					isInTheAir;
	int						oriHp;
	int						hp;
	int						power;
public:
	GameRole();
	bool					dead;
	float					fallOrigin;
	float					moveSpeed;
	float					jumpHeight;
	bool					getIsInTheAir();
	void					setInTheAir(bool isInTheAir);
	void					getHurt(int damage);
	void					jump();
};

