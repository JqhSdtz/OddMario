#include "Mob.h"

USING_NS_CC;

Mob* Mob::createMob(int mobCode,int carryedItemCode)
{
	Mob *mob = new Mob();
	if (mob&&mob->initMob(mobCode, carryedItemCode))
	{
		mob->autorelease();
		return mob;
	}
	CC_SAFE_DELETE(mob);
	return nullptr;
}

bool Mob::initMob(int mobCode,int carryedItemCode)
{
	Vector<SpriteFrame*> mobActionList;
	Animation* mobMoveOnceAnimation;
	Animate* mobMoveOnceAnimate;
	switch (mobCode)
	{
	case 1:
		if (!initWithSpriteFrameName("chomper_01.png")) return false;
		setScale(0.3);
		mobActionList.reserve(3);
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("chomper_01.png"));
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("chomper_02.png"));
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("chomper_03.png"));
		mobMoveOnceAnimation = Animation::createWithSpriteFrames(mobActionList, 0.2f, 1);
		mobMoveOnceAnimate = Animate::create(mobMoveOnceAnimation);
		runAction(RepeatForever::create(mobMoveOnceAnimate));
		moveSpeed = 0;
		jumpHeight = 400;
		jumpInterval = 3;
		oriHp = 50;
		power = 20;
		break;
	case 2:
		if (!initWithSpriteFrameName("mob1_01.png")) return false;
		setScale(0.15);
		mobActionList.reserve(4);
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("mob1_01.png"));
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("mob1_02.png"));
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("mob1_03.png"));
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("mob1_04.png"));
		mobMoveOnceAnimation = Animation::createWithSpriteFrames(mobActionList, 0.2f, 1);
		mobMoveOnceAnimate = Animate::create(mobMoveOnceAnimation);
		runAction(RepeatForever::create(mobMoveOnceAnimate));
		moveSpeed = 160;
		jumpHeight = 350;
		jumpInterval = 2;
		moveRange = 2;
		oriHp = 30;
		power = 15;
		break;
	case 3:
		if (!initWithSpriteFrameName("mob2_01.png")) return false;
		setScale(0.2);
		mobActionList.reserve(3);
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("mob2_01.png"));
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("mob2_02.png"));
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("mob2_01.png"));
		mobMoveOnceAnimation = Animation::createWithSpriteFrames(mobActionList, 0.2f, 1);
		mobMoveOnceAnimate = Animate::create(mobMoveOnceAnimation);
		runAction(RepeatForever::create(mobMoveOnceAnimate));
		moveSpeed = 80;
		jumpHeight = 100;
		jumpInterval = 0.5;
		moveRange = 3;
		oriHp = 20;
		power = 10;
		break;
	case 4:
		if (!initWithSpriteFrameName("mob3.png")) return false;
		setScale(0.35);
		{
			auto moveBy0 = MoveBy::create(0.6f, Vec2(0, 50));
			auto delayTime = DelayTime::create(0.2f);
			auto moveBy1 = MoveBy::create(1.2f, Vec2(0, -80));
			auto moveBy2 = MoveBy::create(0.4f, Vec2(0, 30));
			auto action = Sequence::create(moveBy0, moveBy1, delayTime, moveBy2, nullptr);
			runAction(RepeatForever::create(action));
		}
		moveSpeed = 90;
		jumpHeight = 250;
		jumpInterval = 5;
		moveRange = 4;
		oriHp = 40;
		power = 25;
		break;
	case 5:
		if (!initWithSpriteFrameName("mob4_01.png")) return false;
		setScale(0.22);
		mobActionList.reserve(3);
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("mob4_01.png"));
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("mob4_02.png"));
		mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("mob4_01.png"));
		mobMoveOnceAnimation = Animation::createWithSpriteFrames(mobActionList, 0.2f, 1);
		mobMoveOnceAnimate = Animate::create(mobMoveOnceAnimation);
		runAction(RepeatForever::create(mobMoveOnceAnimate));
		moveSpeed = 45;
		jumpHeight = 150;
		jumpInterval = 4;
		moveRange = 1;
		oriHp = 60;
		power = 5;
		break;
	case 6://dragon
		if (!initWithSpriteFrameName("dragon_all_14.png")) return false;
		{
			char szName[9][20] = { "dragon_all_14.png","dragon_all_15.png", "dragon_all_16.png", "dragon_all_17.png",
				"dragon_all_18.png","dragon_all_19.png","dragon_all_20.png","dragon_all_21.png","dragon_all_22.png" };
			mobActionList.reserve(18);
			for(int i=0;i<9;++i)
				mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(szName[i]));
			for(int i=8;i>=0;--i)
				mobActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(szName[i]));
		}
		mobMoveOnceAnimation = Animation::createWithSpriteFrames(mobActionList, 0.2f, 1);
		mobMoveOnceAnimate = Animate::create(mobMoveOnceAnimation);
		runAction(RepeatForever::create(mobMoveOnceAnimate));
		isInTheAir = false;
		fallOrigin = Director::getInstance()->getVisibleSize().height-300;
		moveSpeed = 450;
		jumpHeight = 100;
		jumpInterval = 2.5;
		oriHp = 600;
		power = 50;
		break;
	}
	attacking = false;
	if(mobCode!=6)
		fallOrigin = Director::getInstance()->getVisibleSize().height;
	relativePosition = 0;
	attackInterval = 0;
	jumpDeltaTime = jumpInterval;
	hp = oriHp;
	//设置血条位置
	if (mobCode != 6)
	{
		bloodBar->setScaleX((1 - getScale())*0.8);
		bloodBar->setPosition(getPositionX() + getContentSize().width / 2, getPositionY() + getContentSize().height + 30);
	}
	else
	{
		bloodBar->setScaleX(0.5);
		bloodBar->setPosition(getPositionX() + getContentSize().width / 2, getPositionY() + getContentSize().height - 50);
	}
	this->carryedItemCode = carryedItemCode;
	this->mobCode = mobCode;
	dead = false;
	return true;
}

int	Mob::attack(float delta)
{
	if (attackInterval > 0)
	{
		attackInterval -= delta;
		return 0;
	}
	if (getActionByTag(MOBATTACKACTION) != nullptr)
		return 0;
	auto delayTime = DelayTime::create(0.1f);
	auto rotateBy0= RotateBy::create(0.1f,35.0f);
	auto rotateBy1= RotateBy::create(0.1f, -35.0f);
	if (isFlippedX())//怪物朝右
	{
		auto moveBy0 = MoveBy::create(0.1f, Vec2(50, 80));
		auto moveBy1 = MoveBy::create(0.05f, Vec2(-50, -80));
		auto attack = Sequence::create(moveBy0, rotateBy0, delayTime,
			rotateBy1, moveBy1,delayTime, nullptr);
		attack->setTag(MOBATTACKACTION);
		runAction(attack);
	}
	else
	{
		auto moveBy0 = MoveBy::create(0.1f, Vec2(-50, 80));
		auto moveBy1 = MoveBy::create(0.05f, Vec2(50, -80));
		auto attack = Sequence::create(moveBy0, rotateBy1, delayTime,
			rotateBy0, moveBy1,delayTime ,nullptr);
		attack->setTag(MOBATTACKACTION);
		runAction(attack);
	}
	attackInterval = 1.0f;
	return power;
}

void Mob::getStamped(int damage)
{
	isStamped = true;
	stopTime = 1.0f;
	getHurt(damage);
}

