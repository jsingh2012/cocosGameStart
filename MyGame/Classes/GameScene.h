#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include <ctime>

class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);
    
    static GameScene* getInstance();
    
    void swapListener(int tileId, int direction);
    void setTileOf(int pos, int forTileId);
    void waitForSwapToComplete(float dt);
    void hideMatchedTiles(float dt);
    void startTheFall(float dt);
    
private:
    cocos2d::Label* scoreLabel;
    cocos2d::Label* lifeLabel;
    cocos2d::Label* MovesLabel;
    void GoToMainMenuScene(cocos2d::Ref *sender);
    void CreateGridBackGround();
    void CreateTopBar();
    void UpdateTopBar();
};

static GameScene* instance;
#endif // __GAME_SCENE_H__
