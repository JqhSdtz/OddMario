#include "GuideScene.h"
#include "AudioEngine.h"

USING_NS_CC;
using namespace experimental;

Scene* GuideScene::createScene()
{
	return GuideScene::create();
}

bool GuideScene::init()
{
	if (!Scene::init())
	{
		return false;
	}
	auto background = LayerColor::create(Color4B::WHITE,1024,768);
	auto gameStartItem = MenuItemImage::create(
		"GuideScene/gameStartButton_white.png",
		"GuideScene/gameStartButton_black.png",
		CC_CALLBACK_1(GuideScene::gameStartCallback, this));
	auto gameIntroItem = MenuItemImage::create(
		"GuideScene/gameIntroButton_white.png",
		"GuideScene/gameIntroButton_black.png",
		CC_CALLBACK_1(GuideScene::gameIntroCallback, this));
	gameStartItem->setPosition(Vec2(0, 550));
	gameIntroItem->setPosition(Vec2(0, 250));
	auto menu = Menu::create(gameStartItem,gameIntroItem, nullptr);
	menu->setPosition(Vec2(512,0));
	background->addChild(menu);
	this->addChild(background);
	AudioEngine::preload("sound/beep1.ogg");
	return true;
}

void GuideScene::gameStartCallback(Ref* pSender)
{
	auto scene = GameScene::createScene();
	Director::getInstance()->pushScene(TransitionCrossFade::create(2.0f,scene));
}

void GuideScene::gameIntroCallback(Ref* pSender)
{
	AudioEngine::play2d("sound/beep1.ogg");
	auto introImage = Sprite::create("GuideScene/gameIntro.png");
	auto backWardItem = MenuItemImage::create(
		"GuideScene/backWard_white.png",
		"GuideScene/backWard_black.png",
		CC_CALLBACK_1(GuideScene::backWardCallback, this));
	introImage->setPosition(Vec2(512, 384));
	backWardItem->setPosition(Vec2(90, 720));
	auto menu = Menu::create(backWardItem, nullptr);
	menu->setPosition(Vec2::ZERO);
	introImage->setTag(0);
	menu->setTag(1);
	this->addChild(introImage);
	this->addChild(menu);
}

void GuideScene::backWardCallback(Ref* pSender)
{
	AudioEngine::play2d("sound/beep1.ogg");
	this->removeChildByTag(0);
	this->removeChildByTag(1);
}