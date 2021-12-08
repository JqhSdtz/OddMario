#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "cocos2d.h"
#include "GameItem.h"
#include "GameRole.h"

#define BOUNCEACTION	8
#define ACCMULTIPLE 2.5

USING_NS_CC;
class Player:public GameRole
{
private:
	Vector<SpriteFrame*>	ActionList;
	Action					*moveAnimate;
	char					*stopFrameName;
	EventListenerKeyboard	*listener = EventListenerKeyboard::create();
	void					creatAnimate();
public:
	Vector<GameItem*>		itemSlot;
	ProgressTimer			*accumulateBar;
	GameItem				*weapon;
	Layer					*backgroundLayer;
	int						itemCapacity;
	float					pushDist;
	bool					powerReleased;
	static Player*			createPlayer(const char *szName,Layer *background);
	void					initPlayer(Layer *background);
	void					onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void					onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void					playerMove(int dir);
	int						stamp();
	void					move(int dir);
	void					stop(int dir);
	void					update(float delta);
	void					acclerate(int dir);
};

#endif __PLAYER_H__