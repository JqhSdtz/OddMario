#include "Player.h"

USING_NS_CC;

bool rightKeyPressed = false;
bool leftKeyPressed = false;
float keyPressInterval = 0;

Player* Player::createPlayer(const char *szName,Layer *background)
{
	Player *player = new Player();
	if (player&&player->initWithSpriteFrameName(szName))
	{
		player->initPlayer(background);
		player->autorelease();
		return player;
	}
	CC_SAFE_DELETE(player);
	return nullptr;
}

void Player::initPlayer(Layer *background)
{
	stopFrameName = "play_stop.png";
	ActionList.reserve(2);
	ActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("play_01.png"));
	ActionList.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("play_02.png"));
	listener->onKeyPressed = CC_CALLBACK_2(Player::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(Player::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, background);
	//设置血条位置
	bloodBar->setScaleX(0.5);
	bloodBar->setPosition(getPositionX() + getContentSize().width / 2, getPositionY() + getContentSize().height + 30);
	/*初始化蓄力进度条——开始*/
	auto bar1 = Sprite::createWithSpriteFrameName("bar.png");
	accumulateBar = ProgressTimer::create(bar1);
	accumulateBar->setType(ProgressTimer::Type::BAR);
	accumulateBar->setMidpoint(Vec2(0, 0.5));
	accumulateBar->setBarChangeRate(Vec2(1, 0));
	auto visibleSize = Director::getInstance()->getVisibleSize();
	accumulateBar->setPosition(visibleSize.width / 2, visibleSize.height - 120);
	accumulateBar->setScaleX(0.6);
	accumulateBar->setPercentage(0);
	this->backgroundLayer = background;
	this->backgroundLayer->addChild(accumulateBar);
	/*初始化蓄力进度条——结束*/
	weapon = nullptr;
	moveSpeed = 200.f;
	jumpHeight = 250.f;
	fallOrigin = 0;
	itemCapacity = 8;
	pushDist = 0;
	oriHp = 150;
	hp = 150;
	power = 10;
	powerReleased = false;
	scheduleUpdate();
}

void Player::update(float delta)
{
	if (rightKeyPressed || leftKeyPressed)
		keyPressInterval += delta;
	else
		keyPressInterval = 0;
}

void Player::onKeyPressed(EventKeyboard::KeyCode keyCode, Event *event)
{
	/*按键盘左键向左移动——开始*/
	if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
	{
		if (!isFlippedX()&& weapon != nullptr)
		{
			weapon->setPositionX(-getContentSize().width*0.2);
			weapon->runAction(FlipX::create(true));
		}
		playerMove(MOVELEFT);
	}
	/*按键盘左键向左移动——结束*/

	/*按键盘右键向右移动——开始*/
	else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
	{
		if(isFlippedX()&& weapon != nullptr)
		{
			weapon->setPositionX(getContentSize().width*1.2);
			weapon->runAction(FlipX::create(false));
		}
		playerMove(MOVERIGHT);
	}
	/*按键盘右键向右移动——结束*/

	/*按键盘上键跳跃——开始*/
	else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
	{
		jump();
	}
	/*按键盘上键跳跃——结束*/

	/*按退格键丢弃武器——开始*/
	else if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE)
	{
		removeChild(weapon,true);
		weapon = nullptr;
	}
	/*按退格键丢弃武器——结束*/
}

void Player::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
		stop(MOVELEFT);
	else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
		stop(MOVERIGHT);
}

void Player::creatAnimate()
{
	auto moveOnceAnimation = Animation::createWithSpriteFrames(ActionList, 0.3f, 1);
	auto moveOnceAnimate = Animate::create(moveOnceAnimation);
	moveAnimate = RepeatForever::create(moveOnceAnimate);
}

void Player::playerMove(int DIRECTION)
{
	int RE_DIRECTION = DIRECTION == MOVELEFT ? MOVERIGHT : MOVELEFT;
	if (getActionByTag(RE_DIRECTION) != nullptr)
	{//若角色已在移动，则不用创建新的移动动作动画
		stopActionByTag(RE_DIRECTION);
	}
	else
	{
		if (getActionByTag(MOVEACTION) == nullptr)
		{
			creatAnimate();
			moveAnimate->setTag(MOVEACTION);
			runAction(moveAnimate);
		}
	}
	/*判断是否加速*/
	if (DIRECTION == MOVELEFT)
	{
		if (rightKeyPressed)
		{
			rightKeyPressed = false;
			keyPressInterval = 0;
		}
		if(!leftKeyPressed)
			leftKeyPressed = true;
	}
	else if (DIRECTION == MOVERIGHT)
	{
		if (leftKeyPressed)
		{
			leftKeyPressed = false;
			keyPressInterval = 0;
		}
		if (!rightKeyPressed)
			rightKeyPressed = true;
	}
	if (keyPressInterval!=0&&keyPressInterval <= 0.5f)
		acclerate(DIRECTION);
	else
		move(DIRECTION);
	keyPressInterval = 0;
}

int Player::stamp()
{
	stopActionByTag(JUMPACTION);
	if (isFlippedX())
	{
		auto jumpBy = JumpBy::create(0.6f, Vec2(-80, 50.f - getPositionY()), 100, 1);
		jumpBy->setTag(BOUNCEACTION);
		runAction(jumpBy);
	}
	else
	{
		auto jumpBy = JumpBy::create(0.6f, Vec2(80, 50.f - getPositionY()), 100, 1);
		jumpBy->setTag(BOUNCEACTION);
		runAction(jumpBy);
	}
	fallOrigin = getPositionY();
	return power;
}

void Player::move(int DIRECTION)
{
	int dir = 1;
	if (DIRECTION == MOVELEFT) dir = -1;
	if (!isFlippedX() && DIRECTION == MOVELEFT)
		runAction(FlipX::create(true));
	else if (isFlippedX() && DIRECTION == MOVERIGHT)
		runAction(FlipX::create(false));
	stopActionByTag(DIRECTION);
	auto moveDirection = RepeatForever::create(MoveBy::create(1.0f, Vec2(dir*moveSpeed, 0)));
	moveDirection->setTag(DIRECTION);
	runAction(moveDirection);
}

void Player::stop(int DIRECTION)
{
	stopActionByTag(DIRECTION);
	if (getActionByTag(SPEEDACTION) != nullptr)
	{
		stopActionByTag(SPEEDACTION);
		moveSpeed /= ACCMULTIPLE;
	}
	int RE_DIRECTION = DIRECTION == MOVELEFT ? MOVERIGHT : MOVELEFT;
	if (getActionByTag(RE_DIRECTION) == nullptr)
	{
		stopActionByTag(MOVEACTION);
		setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(stopFrameName));
	}
}

void Player::acclerate(int DIRECTION)
{
	moveSpeed *= ACCMULTIPLE;
	if (getActionByTag(MOVEACTION) != nullptr)
	{
		auto speed = Speed::create((ActionInterval*)getActionByTag(MOVEACTION), 2.0);
		speed->setTag(SPEEDACTION);
		runAction(speed);
	}
	move(DIRECTION);
}