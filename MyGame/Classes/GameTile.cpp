#include "GameTile.h"
#include "GameScene.h"
#include "Definitions.h"

USING_NS_CC;

GameTile::GameTile(int type, int tileId)
{
    std::string image;
    if(type == 1)
      image = "unlit-bomb.png";
    if(type == 2)
        image = "diplodocusBlue.png";
    if(type == 3)
       image = "tiger-head.png";
    if(type == 4)
        image = "gargoyle.png";
    if(type == 5)
       image = "morph-ball.png";
    if(type == 6)
        image = "magic-hat.png";
    
    this->sprite = Sprite::create(image);
    this->tileId = tileId;
    GameTile::addEvents();
}

GameTile* GameTile::CreateTile(const std::string& image)
{
    this->sprite = Sprite::create(image);
    GameTile::addEvents();
    return this;
}

void GameTile::SetPosition(const Vec2& position)
{
    if(this->sprite == nullptr)
        return;
    this->sprite->setPosition(position);
    //printf("SetPosition %f %f\n", this->position.x, this->position.y);
}

void GameTile::SetScale(float x, float y)
{
    if(this->sprite == nullptr)
        return;
    this->sprite->setScaleY(x);
    this->sprite->setScaleX(y);
}

cocos2d::Sprite* GameTile::GetSprite()
{
    return this->sprite;
}

void GameTile::addEvents()
{
    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        Vec2 pos = touch->getLocation();
        cocos2d::Rect rect = this->sprite->getBoundingBox();
        
        if(rect.containsPoint(pos))
        {
            GameTile::touchEvent(touch);
            return true; // to indicate that we have consumed it.
        }
        
        return false; // we did not consume this event, pass thru.
    };
    
    listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        GameTile::touchEvent(touch);
    };
    
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
}

void GameTile::removeSprite()
{
    this->sprite->removeFromParentAndCleanup(true);
}

void GameTile::touchEvent(cocos2d::Touch* touch)
{
    static bool flag = true;
    static Vec2 pos1;
    static Vec2 pos2;
    if(flag)
    {
        pos1 = touch->getLocation();
        flag = false;
    }
    else
    {
        pos2 = touch->getLocation();
        flag = true;
        //printf("Diff in posX := %f  diff in posY := %f\n", pos1.x - pos2.x, pos1.y - pos2.y);
        
        if(abs(pos1.x - pos2.x) > 40 || abs(pos1.y - pos2.y) > 40)
            return;
        if(abs(pos1.x - pos2.x) < 10 && abs(pos1.y - pos2.y) < 10)
            return;

        if(abs(pos1.x - pos2.x) > abs(pos1.y - pos2.y))
        {
            if(pos1.x - pos2.x > 0)
            {
                 GameScene::getInstance()->swapListener(this->tileId, MOVE_LEFT);
            }
            else
            {
                 GameScene::getInstance()->swapListener(this->tileId, MOVE_RIGHT);
            }
        }
        else
        {
            if(pos1.y - pos2.y > 0)
            {
                GameScene::getInstance()->swapListener(this->tileId, MOVE_DOWN);
            }
            else
            {
                GameScene::getInstance()->swapListener(this->tileId, MOVE_UP);
            }
            
        }
        
    }
    printf("touchEvent %d\n", this->tileId);
}
