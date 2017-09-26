#include "GameTile.h"

USING_NS_CC;

GameTile::GameTile(const std::string& image, int tileId)
{
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
            printf("Enter %f, %f\n", pos.x, pos.y);
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
        printf("Diff in posX := %f  diff in posY := %f\n", pos1.x - pos2.x, pos1.y - pos2.y);
        
        if(abs(pos1.x - pos2.x) > 30 || abs(pos1.y - pos2.y) > 30)
            return;
        if(abs(pos1.x - pos2.x) > abs(pos1.y - pos2.y))
        {
            if(pos1.x - pos2.x > 0)
            {
                printf("Tile %d moved left\n", this->tileId);
            }
            else
            {
                 printf("Tile %d moved right\n", this->tileId);
            }
        }
        else
        {
            if(pos1.y - pos2.y > 0)
            {
                printf("Tile %d moved down\n", this->tileId);
            }
            else
            {
                printf("Tile %d moved up\n", this->tileId);
            }
            
        }
        
    }
    printf("End %f, %f\n", pos1.x, pos1.y);
    printf("touchEvent %d\n", this->tileId);
}
