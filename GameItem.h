#ifndef __GAMEITEM_H__
#define __GAMEITEM_H__
#include "cocos2d.h"

#define ATTACKACTION	6
#define THROWSPEED		300

USING_NS_CC;

class GameItem:public Sprite
{
private:
	float				accumulateTime;
	Sprite				*target;
public:
	int					itemCode;
	bool				accumulating;
	int					power;
	static GameItem*	createGameItem(int itemCode);
	bool				initGameItem(int itemCode);
	void				applyToSprite(Sprite *target);
	int					attack();
	int					throwItem();
	void				accumulate(float delta);
	void				releasePower();
};

#endif __GAMEITEM_H__