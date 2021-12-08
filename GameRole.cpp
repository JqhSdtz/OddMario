#include "GameRole.h"

GameRole::GameRole()
{
	/*初始化血条——开始*/
	auto bar0 = Sprite::createWithSpriteFrameName("bar.png");
	bloodBar = ProgressTimer::create(bar0);
	bloodBar->setType(ProgressTimer::Type::BAR);
	bloodBar->setMidpoint(Vec2(0, 0.5));
	bloodBar->setBarChangeRate(Vec2(1, 0));
	bloodBar->setPercentage(100);
	addChild(bloodBar);
	/*初始化血条——结束*/
	isInTheAir = true;
}

bool GameRole::getIsInTheAir()
{
	return isInTheAir;
}

void GameRole::setInTheAir(bool isInTheAir)
{
	this->isInTheAir = isInTheAir;
}

void GameRole::getHurt(int damage)
{
	hp -= damage;
	if (hp > oriHp) hp = oriHp;
	bloodBar->setPercentage(hp * 100 / oriHp);
	if (hp <= 0)
		dead = true;
}

void GameRole::jump()
{
	if (getActionByTag(JUMPACTION) != nullptr) return;
	if (getPositionY() > 50.f && isInTheAir) return;
	fallOrigin += jumpHeight;
	auto jumpBy = JumpBy::create(1.0f, Vec2(0, 0), jumpHeight, 1);
	jumpBy->setTag(JUMPACTION);
	runAction(jumpBy);
}

