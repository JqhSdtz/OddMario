#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__
#include "cocos2d.h"
#include "SceneryBlock.h"
#include "Player.h"
#include <stdio.h>

USING_NS_CC;

class GameScene :public Scene
{
private:
	LayerColor				*backGroundLayer;
	Layer					*sceneryLayer;
	Layer					*playerLayer;
	Player					*player;
	Vector<SceneryBlock *>	sceneryBlocks;
	Sprite					*sun;
	Sprite					*cloud1;
	Sprite					*cloud2;
	int						gameInfo[50][50];
	float					groundWidth;
	int						amountOfBlocksNum;
	void					checkCollision(float delta);
	void					checkBlocksCollision(GameRole *target,Rect rectTarget,int blockNum,int tag);
	void					creatSceneryBlock();
	void					pickUpItem(GameItem *item,int pos=-1);
	bool					readFile();
	void					controlDragon(Mob* dragon,float delta);
	void					gameEnd();
	void					nextLevel();
public:
	static int				level;
	static Scene*			createScene();
	virtual bool			init();
	void					update(float delta);
	void					onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void					onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void					sunAndcloudScaleUpdate(float delta);
	void					controlMob(float delta);
	void					fallDown(GameRole *target, float delta,int tag);
	void					deleteScene();
	CREATE_FUNC(GameScene);
};

#endif __GAMESCENE_H__