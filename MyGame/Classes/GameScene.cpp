#include "GameScene.h"
#include "Definitions.h"
#include "MainMenuScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

class node
{
    static int id;
public:
    int posX;
    int posY;
    int type;
    node(){
        id++;
        type = 0;
    }
    node(int x,int y, int _type)
    {
        id++;
        posX = x;
        posY = y;
        type = _type;
    }
    
    bool operator==(node& rhs)const
    {
        if (this->type ==rhs.type)
            return true;
        return false;
    }
};

int node::id = 0;

class Grid
{
    node grid[100][100];
    static Grid *instance;
    int rowCount;
    int colCount;
    int maxTypeOfTiles;
    GameScene *Scene;
    
public:
    Grid(const int _rowCount ,const int _colCount, GameScene* ptr)
    {
        // dynamically allocate memory of size M*N
        this->rowCount = _rowCount;
        this->colCount = _colCount;
        Scene = ptr;
        
       
    }
    
    void fillTheGrid(int maxDiffTileType)
    {
        maxTypeOfTiles = maxDiffTileType;
        //std::srand(std::time(0));
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                grid[i][j].type = std::rand() % maxDiffTileType + 1;
                grid[i][j].posX = i;
                grid[i][j].posY = j;
            }
        }
    }
    
    void createGridBackGround()
    {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        
        for(int i = 0 ; i < rowCount; i++)
        {
            int rowEvenOdd = i % 2;
            for(int j = 0; j < colCount; j++)
            {
                int colEvenOdd = ( rowEvenOdd + j) % 2;
                
                if(colEvenOdd == 0)
                {
                    auto LightBlue = Sprite::create("LightBlue.png");
                    LightBlue->setPosition(Point(visibleSize.width/2 + origin.x/2 + 7 + (j * 19),
                                                 visibleSize.height/5 + origin.y     + (i * 19)));
                    LightBlue->setScaleY(TILE_Y_SCALE);
                    LightBlue->setScaleX(TILE_X_SCALE);
                    Scene->addChild(LightBlue);

                }
                else
                {
                    auto GrayBlue = Sprite::create("GrayBlueTile.png");
                    GrayBlue->setPosition(Point(visibleSize.width/2 + origin.x/2 + 7 + (j * 19),
                                                visibleSize.height/5 + origin.y +(i * 19)));
                    GrayBlue->setScaleY(TILE_Y_SCALE);
                    GrayBlue->setScaleX(TILE_X_SCALE);
                    Scene->addChild(GrayBlue);
                }
            }
        }
    }
};

void GameScene::CreateGridBackGround()
{

}
// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto backGroundSprite = Sprite::create("relaxBackGround.png");
    backGroundSprite->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    
    this->addChild(backGroundSprite);
    
    Grid myGrid(10, 8, this);
    myGrid.fillTheGrid(5);
    myGrid.createGridBackGround();
    return true;
}

void GameScene::GoToMainMenuScene(cocos2d::Ref *sender){
    auto scene = MainMenuScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}

void GameScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
