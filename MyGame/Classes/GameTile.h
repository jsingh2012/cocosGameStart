#ifndef __GAME_TILE_H__
#define __GAME_TILE_H__

#include "cocos2d.h"

class GameTile : public cocos2d::Sprite
{
public:
    int tileId;
    cocos2d::Sprite* sprite;
    cocos2d::Vec2 position;
    GameTile(int type, int tileId);
    GameTile* CreateTile(const std::string& image);
    void SetPosition( const cocos2d::Vec2& position);
    void SetScale(float x, float y);
    cocos2d::Sprite* GetSprite();
    void addEvents();
    void touchEvent(cocos2d::Touch* touch);
    void removeSprite();
};

#endif // __GAME_TILE_H__