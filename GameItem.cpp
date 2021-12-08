#include "GameItem.h"

USING_NS_CC;

GameItem* GameItem::createGameItem(int itemCode)
{
	GameItem *item = new GameItem();
	if (item&&item->initGameItem(itemCode))
	{
		item->autorelease();
		return item;
	}
	CC_SAFE_DELETE(item);
	return nullptr;
}

bool GameItem::initGameItem(int itemCode)
{
	this->itemCode = itemCode;
	switch (itemCode)
	{
	case 1:
		if (!initWithSpriteFrameName("arrow.png")) return false;
		break;
	case 2:
		if (!initWithSpriteFrameName("axe.png")) return false;
		break;
	case 3:
		if (!initWithSpriteFrameName("bomb.png")) return false;
		break;
	case 4:
		if (!initWithSpriteFrameName("bottle.png")) return false;
		break;
	case 5:
		if (!initWithSpriteFrameName("bullet.png")) return false;
		break;
	case 6:
		if (!initWithSpriteFrameName("hammer.png")) return false;
		break;
	case 7:
		if (!initWithSpriteFrameName("knife.png")) return false;
		break;
	case 8:
		if (!initWithSpriteFrameName("star.png")) return false;
		break;
	case 9:
		if (!initWithSpriteFrameName("sword.png")) return false;
		break;
	case 10:
		if (!initWithSpriteFrameName("key.png")) return false;
		break;
	}
	setScale(45.f / getContentSize().height);
	setAnchorPoint(Vec2(0.5, 0));
	accumulateTime = 0;
	return true;
}

void GameItem::applyToSprite(Sprite *target)
{
	this->target = target;
	float posX;
	if(target->isFlippedX())
	{
		posX = -target->getContentSize().width*0.2;
		runAction(FlipX::create(true));
	}
	else posX = target->getContentSize().width*1.2;
	float posY =  target->getContentSize().height*0.3;
	runAction(MoveTo::create(0.2f, Vec2(posX,posY)));
	runAction(ScaleBy::create(0.2f, 5.0f));
	removeFromParentAndCleanup(false);
	target->addChild(this);
}

int GameItem::attack()
{
	if (getActionByTag(ATTACKACTION) == nullptr)
	{
		int dir = 1;
		if (isFlippedX()) dir = -1;
		auto skewBy0 = SkewBy::create(0.1f, dir* 45.0f, -dir * 45.0f);
		auto delayTime = DelayTime::create(0.02f);
		auto skewBy1 = SkewBy::create(0.1f, -dir * 45.0f, 0);
		auto skewBy2 = SkewBy::create(0.1f, 0, dir*45.0f);
		auto attack = Sequence::create(skewBy0, delayTime, skewBy1, delayTime, skewBy2, nullptr);
		attack->setTag(ATTACKACTION);
		runAction(attack);
		if (accumulateTime < 1.0f)
			return 20;
		else
			return accumulateTime * 50;
	}
	return 0;
}

int GameItem::throwItem()
{
	int dir = 1;
	if (isFlippedX()) dir = -1;
	setScale(0.18f);
	float s = dir * accumulateTime * THROWSPEED;
	float h = THROWSPEED * accumulateTime;
	ccBezierConfig config;
	config.endPosition = Vec2(s, 50.f - getPositionY());
	config.controlPoint_1 = Vec2(s / 2, h);
	config.controlPoint_2 = Vec2(s / 2, h);
	runAction(BezierBy::create(accumulateTime / 2, config));
	return 100;
}

void GameItem::accumulate(float delta)
{
	if(accumulateTime<5.0f)
		accumulateTime += delta;
}

void GameItem::releasePower()
{
	accumulating = false;
	if(itemCode==1|| itemCode == 3 || itemCode == 5 )
		power=throwItem();
	else 
		power=attack();
	accumulateTime = 0;
}
