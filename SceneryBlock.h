#pragma once
#include "cocos2d.h"
#include "Mob.h"
#include "GameItem.h"
USING_NS_CC;

class SceneryBlock:public Node
{
public:
	Sprite				*ground;
	Vector<Sprite*>		blocks;
	Vector<Mob*>		mobs;
	Vector<GameItem*>	items;
	SceneryBlock();
};

