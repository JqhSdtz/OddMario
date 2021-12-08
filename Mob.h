#pragma once
#include "cocos2d.h"
#include "GameRole.h"

#define MOBATTACKACTION 7

USING_NS_CC;

class Mob:public GameRole
{
private:
	float			attackInterval;
public:
	int				mobCode;
	int				carryedItemCode;
	int				moveRange;
	int				relativePosition;
	float			stopTime;
	float			jumpInterval;
	float			jumpDeltaTime;
	bool			attacking;
	bool			isStamped;
	static Mob*		createMob(int mobCode,int carryedItemCode);
	bool			initMob(int mobCode,int carryedItemCode);
	int				attack(float delta);
	void			getStamped(int damage);
};

