#pragma once

#include "cocos2d.h"
#include "GameScene.h"

USING_NS_CC;

class GuideScene:public Scene
{
public:
	static Scene* createScene();
	virtual bool init();
	void gameStartCallback(Ref* pSender);
	void gameIntroCallback(Ref* pSender);
	void backWardCallback(Ref* pSender);
	CREATE_FUNC(GuideScene);
};

