#include "GameScene.h"
#include "AudioEngine.h"


USING_NS_CC;
using namespace experimental;

int GameScene::level = 0;

Scene* GameScene::createScene()
{
	return GameScene::create();
}

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool GameScene::init()
{
	++GameScene::level;
	if (!Scene::init())
		return false;
	if (!readFile())
		return false;
	backGroundLayer = LayerColor::create(Color4B::WHITE,2048.f,1536.f);
	sceneryLayer = Layer::create();
	playerLayer = Layer::create();
	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Player/player.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Scenery/Scenery.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Mob/Mob.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Item/Item.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Dragon/dragon.plist");
	AudioEngine::preload("sound/beep2.ogg");
	AudioEngine::preload("sound/main_title.ogg");
	switch (GameScene::level)
	{
	case 1:
		AudioEngine::preload("sound/drum1.ogg");
		break;
	case 2:
		AudioEngine::preload("sound/drum2.ogg");
		break;
	case 3:
		AudioEngine::preload("sound/drum3.ogg");
		break;
	}
	sun = Sprite::createWithSpriteFrameName("sun.png");
	sun->setScale(0.4);
	sun->setPosition(Vec2(150, 600));
	cloud1 = Sprite::createWithSpriteFrameName("cloud.png");
	cloud1->setScale(0.3);
	cloud1->setPosition(Vec2(200, 600));
	cloud2 = Sprite::createWithSpriteFrameName("cloud.png");
	cloud2->setScale(0.25);
	cloud2->setPosition(Vec2(600, 650));
	schedule(CC_SCHEDULE_SELECTOR(GameScene::sunAndcloudScaleUpdate),2.0f);
	backGroundLayer->addChild(sun);
	backGroundLayer->addChild(cloud1);
	backGroundLayer->addChild(cloud2);
	/*���ڲ��Եĵ�һ�������顪����ʼ*/
	auto sceneryblock = new SceneryBlock();
	sceneryblock->ground = Sprite::createWithSpriteFrameName("ground.png");
	groundWidth = sceneryblock->ground->getContentSize().width;
	sceneryblock->ground->setAnchorPoint(Vec2(0, 1));
	sceneryblock->ground->setPosition(Vec2(0, 55));
	sceneryLayer->addChild(sceneryblock->ground);
	sceneryBlocks.pushBack(sceneryblock);
	/*���ڲ��Եĵ�һ�������顪������*/
	if (sceneryblock->ground == nullptr) 
	{
		problemLoading("Scenery/ground.png");
		return false;
	}
	player = Player::createPlayer("play_stop.png",backGroundLayer);
	if (player == nullptr) {
		problemLoading("Player/player_stop.png");
		return false;
	}
	playerLayer->addChild(player);
	player->setScale(0.25);
	player->setAnchorPoint(Vec2(0.5, 0));
	player->setPosition(Vec2(100, 50.f));
	scheduleUpdate();
	this->addChild(backGroundLayer);
	this->addChild(sceneryLayer);
	this->addChild(playerLayer);
	AudioEngine::play2d("sound/main_title.ogg");
	return true;
}

void GameScene::update(float delta)
{
	if (player->dead) gameEnd();
	controlMob(delta);//���ƹ����ƶ�
	checkCollision(delta);//�����ײ
	/*��������������ʼ*/
	if (player->weapon != nullptr)
	{
		if (player->powerReleased&&player->weapon->getPositionY() <= 50.f)//�����䵽��������ʧ
		{
			//player->weapon->runAction(Sequence::create(Blink::create(1.0f, 3), RemoveSelf::create(), nullptr));
			player->removeChild(player->weapon, false);
			sceneryLayer->removeChild(player->weapon);
			player->weapon = nullptr;
			player->powerReleased = false;
		}
		else if (player->weapon->accumulating)//������������
		{
			player->weapon->accumulate(delta);
			player->accumulateBar->setPercentage(player->accumulateBar->getPercentage() + delta*100 / 5.0f);
		}
	}
	/*����������������*/
	if (player->getPositionY()==50.f&& player->getActionByTag(MOVEACTION) == nullptr)
		return;//��ɫ�ڵ����ϲ���û���߶�
	fallDown(player, delta,1);//���ƽ�ɫ����
	/*��ͼ���ɫ�ƶ�������ʼ*/
	if (player->getActionByTag(MOVEACTION) == nullptr) return;//��ɫû���ƶ����ͼҲ�����ƶ�
	auto visibleSize = Director::getInstance()->getVisibleSize();
	float dist = player->getPositionX() - Director::getInstance()->getVisibleOrigin().x;
	if (dist < visibleSize.width / 3&&player->getActionByTag(MOVELEFT)!=nullptr)
	{
		sceneryLayer->runAction(MoveBy::create(delta, Vec2(delta*player->moveSpeed, 0)));
		player->runAction(MoveBy::create(delta, Vec2(delta * player->moveSpeed, 0)));
		player->pushDist -= delta * player->moveSpeed;
	}
	else if (dist > visibleSize.width * 2 / 3&& player->getActionByTag(MOVERIGHT) != nullptr)
	{
		sceneryLayer->runAction(MoveBy::create(delta, Vec2(-delta * player->moveSpeed, 0)));
		player->runAction(MoveBy::create(delta, Vec2(-delta * player->moveSpeed, 0)));
		player->pushDist += delta * player->moveSpeed;
	}
	if (player->getPositionX() + visibleSize.width/3+player->pushDist+20> sceneryBlocks.back()->ground->getPositionX() + sceneryBlocks.back()->ground->getContentSize().width)
	{//��ɫλ�ü�������֮һ��Ļ������ڵ�ǰ��ͼ��߽��򴴽��µ�ͼ��
		creatSceneryBlock();//�����µ�ͼ��
	}
	/*��ͼ���ɫ�ƶ���������*/
}

void GameScene::checkCollision(float delta)
{
	auto rectPlayer = player->getBoundingBox();
	rectPlayer.setRect(rectPlayer.getMinX() + player->pushDist, rectPlayer.getMinY(),
		 rectPlayer.size.width, rectPlayer.size.height);
	int blockNum = (player->getPositionX() + player->pushDist) / groundWidth;
	if (blockNum < 0 || blockNum>amountOfBlocksNum) return;
	if (blockNum == amountOfBlocksNum && player->weapon!=nullptr&&player->weapon->itemCode == 10)
		nextLevel();
	/*��ɫ��ⷽ�����ײ������ʼ*/
	checkBlocksCollision(player, rectPlayer, blockNum, 1);
	/*��ɫ��ⷽ�����ײ��������*/
	/*�����Ʒ����ײ������ʼ*/
	if (!sceneryBlocks.at(blockNum)->items.empty())
	{
		int n = sceneryBlocks.at(blockNum)->items.size();
		Rect rectItem;
		for (int i = 0; i < n; ++i)
		{
			rectItem = sceneryBlocks.at(blockNum)->items.at(i)->getBoundingBox();
			if (rectItem.intersectsRect(rectPlayer))
			{
				if(player->itemSlot.size()>=player->itemCapacity)
				{/*��ʾ��Ʒ������*/ }
				else
				{/*������ʾ�����Ʒ*/
					auto item = sceneryBlocks.at(blockNum)->items.at(i);
					pickUpItem(item);
					sceneryBlocks.at(blockNum)->items.erase(i);
				}
				return;
			}
		}
	}
	/*�����Ʒ����ײ��������*/
}

void GameScene::creatSceneryBlock()
{
	switch (GameScene::level)
	{
	case 1:
		AudioEngine::play2d("sound/drum1.ogg");
		break;
	case 2:
		AudioEngine::play2d("sound/drum2.ogg");
		break;
	case 3:
		AudioEngine::play2d("sound/drum3.ogg");
		break;
	}
	int blockNum = (player->getPositionX() + player->pushDist) / groundWidth+1;
	/*�½���ͼ����桪����ʼ*/
	auto sceneryblock = new SceneryBlock();
	sceneryblock->ground = Sprite::createWithSpriteFrameName("ground.png");
	sceneryblock->ground->setAnchorPoint(Vec2(0, 1));
	sceneryblock->ground->setPosition(sceneryBlocks.back()->ground->getPositionX() + groundWidth,55.f );
	sceneryLayer->addChild(sceneryblock->ground);
	/*�½�ͨ����ʾ�ơ�����ʼ*/
	if (blockNum == amountOfBlocksNum)
	{
		auto board = Sprite::createWithSpriteFrameName("board.png");
		board->setAnchorPoint(Vec2(0.5, 0));
		board->setPosition(Vec2(sceneryBlocks.back()->ground->getPositionX() + groundWidth+300, 50));
		sceneryLayer->addChild(board);
	}
	/*�½�ͨ����ʾ�ơ�������*/
	/*�½���ͼ����桪������*/
	int *p = gameInfo[blockNum],i=0;
	/*��ӷ��顪����ʼ*/
	if (p[i] != 0)
	{
		int n = p[i];
		++i;
		Sprite *blockNew;
		for (int j = 0; j < n; ++j)
		{
			if (p[i] >= 1 && p[i] <= 5)
			{
				switch (p[i])
				{
				case 1:
					blockNew = Sprite::createWithSpriteFrameName("block.png");
					break;
				case 2:
					blockNew = Sprite::createWithSpriteFrameName("double_blocks.png");
					break;
				case 3:
					blockNew = Sprite::createWithSpriteFrameName("blocks.png");
					break;
				case 4:
					blockNew = Sprite::createWithSpriteFrameName("double_blocks.png");
					blockNew->setRotation(-90);
					break;
				case 5:
					blockNew = Sprite::createWithSpriteFrameName("blocks.png");
					blockNew->setRotation(-90);
					break;
				}
				blockNew->setScale(0.6);
				blockNew->setPosition(Vec2(sceneryblock->ground->getPositionX() + p[i + 1], p[i + 2]));
				blockNew->setAnchorPoint(Vec2(0, 0));
				sceneryblock->blocks.pushBack(blockNew);
				sceneryLayer->addChild(blockNew);
			}
			i += 3;
		}
	}
	/*��ӷ��顪������*/
	/*��ӹ������ʼ*/
	if (p[i] != 0)
	{
		float posX = (blockNum + 1)*groundWidth;
		int n = p[i];
		++i;
		for (int j = 1; j <= n; ++j)
		{
			if (p[i] >= 1 && p[i] <= 6 && p[i + 1] >= 1 && p[i + 1] <= 10)
			{
				auto mobNew = Mob::createMob(p[i], p[i + 1]);
				mobNew->setPosition(Vec2(sceneryblock->ground->getPositionX() + p[i + 2], mobNew->fallOrigin));
				mobNew->setAnchorPoint(Vec2(0.5, 0));
				sceneryblock->mobs.pushBack(mobNew);
				sceneryLayer->addChild(mobNew);
			}
			i += 3;
		}
	}
	/*��ӹ��������*/
	sceneryBlocks.pushBack(sceneryblock);
}

void GameScene::sunAndcloudScaleUpdate(float delta)
{
	if (sun->getScale() <= 0.4)
	{
		sun->runAction(ScaleBy::create(2.0f, 1.6f));
		sun->runAction(FadeTo::create(2.0f, 255));
	}
	else
	{
		sun->runAction(ScaleBy::create(2.0f, 0.625f));
		sun->runAction(FadeTo::create(2.0f, 100));
	}
	int speed1 = random(300, 600);
	int dir1, dir2;
	int speed2 = random(300, 600);
	if (cloud1->getPositionX() <= 0)
		dir1 = 1;
	else if (cloud1->getPositionX() >= 1024)
		dir1 = -1;
	else
		dir1 = (speed1 % 2) ? 1 : -1;
	cloud1->runAction(MoveBy::create(2.0f, Vec2(dir1*speed1, 0)));
	if (cloud2->getPositionX() <= 0)
		dir2 = 1;
	else if (cloud2->getPositionX() >= 1024)
		dir2 = -1;
	else
		dir2 = (speed2 % 2) ? 1 : -1;
	cloud2->runAction(MoveBy::create(2.0f, Vec2(dir2*speed2, 0)));
}

void GameScene::controlMob(float delta)
{
	int blockNum = (player->getPositionX() + player->pushDist) / groundWidth;
	if (blockNum < 0) return;
	int startNum=blockNum, endNum=blockNum;
	if (blockNum > 0) --startNum;
	if (blockNum + 1 < sceneryBlocks.size()) ++endNum;//blockNum��0��ʼ�ƣ�sceneryBlocks.size()��1��ʼ��
	int code;
	if(player->weapon!=nullptr)
		code = player->weapon->itemCode;
	auto rectPlayer = player->getBoundingBox();
	rectPlayer.setRect(rectPlayer.getMinX() + player->pushDist, rectPlayer.getMinY(),
		rectPlayer.size.width, rectPlayer.size.height);
	for (int num = startNum; num <=endNum; ++num)//���ƽ�ɫ���ڵ�ͼ���ǰ��������ͼ��
	{
		if (num >= sceneryBlocks.size()) break;
		if (!sceneryBlocks.at(num)->mobs.empty())
		{
			int n = sceneryBlocks.at(num)->mobs.size();
			for (int i = 0; i < n; ++i)//����ÿ������
			{
				if (i >= sceneryBlocks.at(num)->mobs.size()) break;
				auto mob = sceneryBlocks.at(num)->mobs.at(i);
				auto rectMob = mob->getBoundingBox();
				rectMob.setRect(rectMob.getMinX(), rectMob.getMinY(),
					rectMob.size.width, rectMob.size.height + 13);
				/*���ƹ�������������ʼ*/
				if (mob->dead)
				{
					int itemCode = mob->carryedItemCode;
					float posX = mob->getPositionX(), posY = mob->getPositionY();
					mob->runAction(Sequence::create(Blink::create(1.0f, 3),RemoveSelf::create(),nullptr));
					sceneryBlocks.at(num)->mobs.erase(i);
					if (itemCode != 0)
					{
						auto item = GameItem::createGameItem(itemCode);
						item->setPosition(Vec2(posX, posY));
						sceneryBlocks.at(num)->items.pushBack(item);
						sceneryLayer->addChild(item);
					}
				}
				/*���ƹ���������������*/
				/*�����ܵ�����������ʼ*/
				if (mob->isStamped)//���ﱻ��
				{
					if (mob->stopTime > 0)
						mob->stopTime-=delta;
					else
						mob->isStamped = false;
					continue;
				}
				//�����ܵ���������
				if (player->weapon != nullptr&&player->powerReleased)
				{
					auto rectWeapon = player->weapon->getBoundingBox();
					float diffx = 50;
					float diffy = 100;
					if (code == 1 || code == 3 || code == 5)//����������
						rectWeapon.setRect(rectWeapon.getMinX() - diffx*1.5, rectWeapon.getMinY()+player->getPositionY()-diffy,
							rectWeapon.size.width+diffx*3, rectWeapon.size.height);
					else//��������
					{
						if(player->isFlippedX())
							rectWeapon.setRect( player->getPositionX() + player->pushDist-diffx , rectWeapon.getMinY() + player->getPositionY() - diffy,
								diffx, diffy);
						else
							rectWeapon.setRect( player->getPositionX() + player->pushDist+diffx, rectWeapon.getMinY() + player->getPositionY() - diffy,
								diffx, diffy );
					}
					if (rectWeapon.intersectsRect(rectMob))
					{
						mob->getHurt(player->weapon->power);
						//player->powerReleased = false;
						if (code == 1 || code == 3 || code == 5)//������������������ʧ
						{
							player->removeChild(player->weapon, false);
							sceneryLayer->removeChild(player->weapon);
							player->weapon = nullptr;
						}
					}
				}
				/*�����ܵ�������������*/
				if(mob->mobCode!=6)
					checkBlocksCollision(mob, rectMob, num, 0);//������ͷ������ײ
				fallDown(mob, delta,0);//���ƹ�������
				/*���ƹ��﹥��������ʼ*/
				if (rectMob.intersectsRect(rectPlayer))//��ɫ��������
				{
					if (rectPlayer.getMinY() + 25 >= rectMob.getMaxY()
						&& player->getActionByTag(BOUNCEACTION) == nullptr
						&& !mob->attacking && !mob->isStamped)
					{//��ɫ�ȵ�����
						mob->getStamped(player->stamp());
						continue;
					}
					mob->attacking = true;//��ɫû�вȵ����������������
					mob->relativePosition = 0;//���λ����Ϊ0������ҵ�ǰλ�ÿ�ʼ����
					if (player->getPositionX() + player->pushDist < mob->getPositionX() && mob->isFlippedX())
						//��ɫ�ڹ�������ҹ��ﳯ��
						mob->runAction(FlipX::create(false));
					else if (player->getPositionX() + player->pushDist > mob->getPositionX() && !mob->isFlippedX())
						//��ɫ�ڹ����ұ��ҹ��ﳯ��
						mob->runAction(FlipX::create(true));
					player->getHurt(mob->attack(delta));
					continue;//���﹥����ʱ���ƶ�Ҳ����Ծ
				}
				else
					mob->attacking = false;
				/*���ƹ��﹥����������*/
				/*���ƹ����ƶ�������ʼ*/
				if (mob->mobCode == 6)//dragon
				{
					if (num != blockNum)
					{
						sceneryBlocks.at(blockNum)->mobs.pushBack(mob);
						sceneryBlocks.at(num)->mobs.erase(i);
					}
					controlDragon(mob, delta);
				}
				else if (mob->getPositionX() < num*groundWidth)//����ﵽ��ͼ����߽�
				{
					if (abs(mob->relativePosition - 1) >= mob->moveRange || num <= 0)
						//������ͼ���ƶ���Χ��õ�ͼ���ǵ�һ�飬�����ƶ�
					{
						mob->runAction(FlipX::create(true));//����Ĭ������
						mob->runAction(MoveBy::create(delta, Vec2(delta*mob->moveSpeed, 0)));
					}
					else
					{
						--mob->relativePosition;
						sceneryBlocks.at(num - 1)->mobs.pushBack(mob);
						sceneryBlocks.at(num)->mobs.erase(i);
						mob->runAction(MoveBy::create(delta, Vec2(-delta * mob->moveSpeed, 0)));
					}
				}
				else if (mob->getPositionX() > (num + 1)*groundWidth)//����ﵽ��ͼ�ұ߽�
				{
					if (abs(mob->relativePosition + 1) >= mob->moveRange || num >= sceneryBlocks.size() - 1)
						//������ͼ���ƶ���Χ��õ�ͼ�������һ�飬�����ƶ�
					{
						mob->runAction(FlipX::create(false));
						mob->runAction(MoveBy::create(delta, Vec2(-delta * mob->moveSpeed, 0)));
					}
					else
					{
						++mob->relativePosition;
						sceneryBlocks.at(num + 1)->mobs.pushBack(mob);
						sceneryBlocks.at(num)->mobs.erase(i);
						mob->runAction(MoveBy::create(delta, Vec2(delta*mob->moveSpeed, 0)));
					}
				}
				else//����û�е���߽�
				{
					if (mob->isFlippedX())//����Ĭ�����󣬷�ת������
						mob->runAction(MoveBy::create(delta, Vec2(delta*mob->moveSpeed, 0)));
					else
						mob->runAction(MoveBy::create(delta, Vec2(-delta * mob->moveSpeed, 0)));
				}
				/*���ƹ����ƶ���������*/
				/*���ƹ�����Ծ������ʼ*/
				if (mob->jumpDeltaTime > 0)
					mob->jumpDeltaTime -= delta;
				else
				{
					mob->jumpDeltaTime = mob->jumpInterval;
					mob->jump();
				}
				/*���ƹ�����Ծ��������*/
			}
		}
	}
	if (player->weapon != nullptr&&player->powerReleased && !(code == 1 || code == 3 || code == 5))
		player->powerReleased = false;
}

bool GameScene::readFile()
{
	FILE *fp;
	if(GameScene::level==1)
		fp = fopen("gameInfo01.txt","r");
	else if(GameScene::level==2)
		fp = fopen("gameInfo02.txt", "r");
	else if(GameScene::level==3)
		fp = fopen("gameInfo03.txt", "r");
	if (fp == nullptr) return false;
	int r = 1, c = 0;
	while (fscanf(fp, "%d", &gameInfo[r][c]) != EOF)
	{
		if (gameInfo[r][c] == -1)
			++r, c = 0;
		else
			++c;
	}
	amountOfBlocksNum = r;
	return true;
}

void GameScene::checkBlocksCollision(GameRole *target, Rect rectTarget, int blockNum, int tag)
{
	bool flag = false;
	if (blockNum < 0) return;
	if (!sceneryBlocks.at(blockNum)->blocks.empty())
	{
		int n = sceneryBlocks.at(blockNum)->blocks.size();
		Rect rectBlock;
		for (int i = 0; i < n; ++i)
		{
			rectBlock = sceneryBlocks.at(blockNum)->blocks.at(i)->getBoundingBox();
			rectBlock.setRect(rectBlock.getMinX() + 5, rectBlock.getMinY(), rectBlock.size.width - 5, rectBlock.size.height - 12);
			if (!rectBlock.intersectsRect(rectTarget)) continue;
			flag = true;
			//if(tag==0)
				//CCLOG("%f", ((Mob*)target)->moveSpeed);
			if (target->getPositionY() > 50.f &&rectTarget.getMinY()+20  >= rectBlock.getMaxY())//��ɫ�������鶥��
			{
				target->setPositionY(rectBlock.getMaxY());
				if (target->getIsInTheAir() == true)
				{
					target->stopAllActionsByTag(MOVEDOWN);
					target->stopActionByTag(JUMPACTION);
					target->setInTheAir(false);
					target->fallOrigin = target->getPositionY();
				}
				continue;
			}
			target->setInTheAir(true);//��ɫ�������鵫û���������鶥��
			if (abs(rectTarget.getMaxY()-rectBlock.getMinY())<20)//��ɫ��������ײ�
			{
				target->stopActionByTag(JUMPACTION);//ֹͣ��Ծ
				target->fallOrigin = target->getPositionY();
				continue;
			}
			if (abs(rectTarget.getMaxX() - rectBlock.getMinX())<20)//��ɫ�����������
			{
				if (tag == 1)
					target->stopActionByTag(MOVERIGHT);//ֹͣ�����ƶ�
				else
					target->runAction(FlipX::create(false));
				continue;
			}
			if (abs(rectTarget.getMinX() - rectBlock.getMaxX())<20)//��ɫ���������Ҳ�
			{
				if(tag==1)
					target->stopActionByTag(MOVELEFT);//ֹͣ�����ƶ�
				else
					target->runAction(FlipX::create(true));
				continue;
			}
		}
		if(!flag)//û�������κη���
			target->setInTheAir(true);
	}
	else
		target->setInTheAir(true);
}

void GameScene::fallDown(GameRole *target, float delta,int tag)
{
	if (target->getPositionY() <= 50.f)//��ɫ�ƶ�����������λ������Ϊ����
	{
		target->setPositionY(50.f);
		target->fallOrigin = 50.f;
		if (!target->getIsInTheAir())
			target->setInTheAir(true);
	}
	if (target->getPositionY() > 50.f&&target->getIsInTheAir())
	{//��ɫ������Ծ״̬�����뿪���沢���ڿ���������
		if (tag == 1 && target->getActionByTag(JUMPACTION) != nullptr)
			return;
		float dist;
		if (target->fallOrigin == target->getPositionY())
		{
			dist = delta * target->moveSpeed * 100;
			if (target->moveSpeed == 0) dist = delta * 2000;
		}
		else dist = target->fallOrigin - target->getPositionY();
		if (tag != 1) dist *= 2;
		if (dist > 0)
		{
			float speed = target->moveSpeed;
			if (speed == 0) speed = 40;
			auto moveDown = MoveBy::create(delta, Vec2(0, -dist * delta*speed / 25));
			moveDown->setTag(MOVEDOWN);
			target->runAction(moveDown);
		}
	}
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode,Event* event)
{
	/*���ո������������ʼ*/
	if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		if (player->weapon != nullptr && !player->powerReleased&&player->weapon->itemCode!=10)
		{
			player->weapon->accumulating = true;
		}
	}
	/*���ո��������������*/

	/*�����ּ�ʹ����Ʒ������ʼ*/
	else if (keyCode >= EventKeyboard::KeyCode::KEY_1&&keyCode <= EventKeyboard::KeyCode::KEY_9)
	{
		int num = (int)keyCode - (int)EventKeyboard::KeyCode::KEY_0;
		if (player->itemSlot.size() >= num)
		{
			num = player->itemSlot.size() - num;
			int code = player->itemSlot.at(num)->itemCode;
			if (code != 4 && code != 8)
			{
				auto weaponNew = player->itemSlot.at(num);
				weaponNew->applyToSprite(player);
				if (player->weapon != nullptr)
				{
					auto itemNew = GameItem::createGameItem(player->weapon->itemCode);
					player->removeChild(player->weapon, true);
					sceneryLayer->addChild(itemNew);
					itemNew->setPosition(Vec2(player->getPositionX() + player->pushDist, player->getPositionY()));
					int blockNum = (player->getPositionX() + player->pushDist) / groundWidth;
					sceneryBlocks.at(blockNum)->items.pushBack(itemNew);
				}
				player->powerReleased = false;
				player->weapon = weaponNew;
			}
			else
			{
				if (code == 4)
					player->getHurt(-30);
				else if (code == 8)
					player->getHurt(-50);
				player->itemSlot.at(num)->removeFromParent();
			}
			player->itemSlot.erase(num);
			int size = player->itemSlot.size();
			for (int i = 0; i < size; ++i)//������Ʒ��
				player->itemSlot.at(i)->setPositionX(900 - 80 * i);
		}
	}
	/*�����ּ�ʹ����Ʒ��������*/
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		if (player->weapon != nullptr&&player->weapon->accumulating)
		{
			if (player->weapon->itemCode == 1 || player->weapon->itemCode == 3 || player->weapon->itemCode == 5)
			{
				player->weapon->setParent(nullptr);
				player->weapon->setPositionY(player->getPositionY()+10);
				player->weapon->setPositionX(player->getPositionX() + player->pushDist);
				//weapon->setPositionY(getPositionY());
				sceneryLayer->addChild(player->weapon);
			}
			player->accumulateBar->setPercentage(0);
			player->weapon->releasePower();
			player->powerReleased = true;
		}
	}
}

void GameScene::pickUpItem(GameItem* item,int pos)
{
	AudioEngine::play2d("sound/beep2.ogg");
	int size = player->itemSlot.size();
	for (int i = 0; i < size; ++i)//������Ʒ��
		player->itemSlot.at(i)->setPositionX(900 - 80 * i);
	float posX;
	if (pos == -1)
	{
		if (player->itemSlot.empty()) posX = 900;
		else posX = player->itemSlot.back()->getPositionX() - 80;
	}
	else
		posX = 900 - 80 * pos;
	item->runAction(MoveTo::create(0.5f, Vec2(posX, 700)));
	item->removeFromParentAndCleanup(false);
	backGroundLayer->addChild(item);
	player->itemSlot.pushBack(item);
}

void GameScene::controlDragon(Mob *dragon,float delta)
{
	int blockNum = dragon->getPositionX() / groundWidth;
	if (blockNum < 0)
	{
		dragon->runAction(FlipX::create(true));//����Ĭ������
		dragon->runAction(MoveBy::create(delta, Vec2(delta*dragon->moveSpeed, 0)));
	}
	else if (blockNum > amountOfBlocksNum)
	{
		dragon->runAction(FlipX::create(false));
		dragon->runAction(MoveBy::create(delta, Vec2(-delta * dragon->moveSpeed, 0)));
	}
	else
	{
		if (dragon->isFlippedX())//����Ĭ�����󣬷�ת������
			dragon->runAction(MoveBy::create(delta, Vec2(delta*dragon->moveSpeed, 0)));
		else
			dragon->runAction(MoveBy::create(delta, Vec2(-delta * dragon->moveSpeed, 0)));
	}
}

void GameScene::gameEnd()
{
	GameScene::level = 0;
	auto endImage = Sprite::create("GuideScene/gameOver.png");
	endImage->setPosition(Vec2(512, 384));
	this->addChild(endImage);
	runAction(Sequence::create(DelayTime::create(2.0f),
		CallFunc::create(CC_CALLBACK_0(GameScene::deleteScene, this)),nullptr));
}

void GameScene::deleteScene()
{
	Director::getInstance()->popScene();
}

void GameScene::nextLevel()
{
	++amountOfBlocksNum;
	auto scene = GameScene::createScene();
	Director::getInstance()->replaceScene(TransitionCrossFade::create(2.0f, scene));
}

//��ʱ���������λ�ã�������ͼ����޵�ת�Ƶ���һ����ͼ����
//��ʱ������ɫ�͹�����ײ���������﹥�����������Ź�������
//��ɫ������ʱ���������Լ���������һСȦ�Ƿ�͹�����ײ����������Χ�ڹ����ܵ��˺�
//�����������������������ƶ�������㣬����һ��ʱ�����ʧ���������������󹥻�������