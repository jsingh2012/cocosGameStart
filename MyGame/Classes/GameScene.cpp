#include "GameScene.h"
#include "Definitions.h"
#include "MainMenuScene.h"
#include "GameTile.h"
#include "GameScene.h"

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
    
public:
    static int id;
    bool deleted = false;
    int posX;
    int posY;
    int type;
    int tileId;
    bool busy = false;
    GameTile *tile;
    node()
    {
        tileId = id++;
    }
    node(int x,int y, int _type)
    {
        tileId = id++;
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
        grid[99][99].posX = INT16_MAX;
        grid[99][99].posY = INT16_MAX;

        checkAllMatches();
    }
    
    void printGrid()
    {
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                if(grid[i][j].type == 0) {
                    printf( "[%d,%d]%2d ", i,j, grid[i][j].type);
                    // printf( "(%d)%2d", grid[i][j].tileId, grid[i][j].type);
                }
                else {
                    printf( "[%d,%d]%2d ", i,j, grid[i][j].type);
                    //printf( "(%d)%2d",  grid[i][j].tileId, grid[i][j].type);
                }
            }
            printf("\n");
        }
    }
    
    int getTileOn(int direction, int forTileId)
    {
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                if(grid[i][j].tileId == forTileId)
                {
                    switch (direction) {
                        case MOVE_LEFT:
                            if(j - 1 >= 0)
                                return grid[i][j-1].tileId;
                            break;

                        case MOVE_RIGHT:
                            if(j + 1 < colCount)
                                return grid[i][j+1].tileId;;
                            break;

                        case MOVE_UP:
                            if(i - 1 >= 0 )
                                return grid[i + 1][j].tileId;;
                            break;

                        case MOVE_DOWN:
                            if(i - 1 >= 0)
                                return grid[i - 1][j].tileId;
                            break;
                    }
                }
                   
            }
        }
        return INT16_MAX;
    }
    
    void swapTiles(int tileOne, int tileTwo)
    {
        node *tilePtrOne;
        node *tilePtrTwo;
        printf("Swapping %d, %d",tileOne, tileTwo);
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                if(grid[i][j].tileId == tileOne)
                {
                    tilePtrOne = &grid[i][j];
                }
                if(grid[i][j].tileId == tileTwo)
                {
                    tilePtrTwo = &grid[i][j];
                }
            }
        }
        printf("BeforeSwap\n");
        printGrid();
        auto tmpTileId = tilePtrOne->tileId;
        auto tmpType = tilePtrOne->type;
        auto tmpSprite = tilePtrOne->tile;
        auto tmpX = tilePtrOne->posX;
        auto tmpY = tilePtrOne->posY;
        auto posOne = tilePtrOne->tile->position;
        auto posTwo = tilePtrTwo->tile->position;
        
        tilePtrOne->tileId = tilePtrTwo->tileId;
        tilePtrOne->type = tilePtrTwo->type;
        tilePtrOne->posX = tilePtrTwo->posX;
        tilePtrOne->posY = tilePtrTwo->posY;
        
        // Create the actions
        CCFiniteTimeAction* actionMoveOne = CCMoveTo::create(0.2, posTwo);
        
        auto endActionOne = CallFunc::create([tilePtrTwo, tilePtrOne, posTwo]() {
            tilePtrOne->busy = false;
            tilePtrOne->tile->SetPosition(posTwo);
            tilePtrOne->tile = tilePtrTwo->tile;
        });
        
        tilePtrOne->tile->sprite->runAction( CCSequence::create(actionMoveOne, endActionOne, NULL) );
        
        tilePtrTwo->tileId = tmpTileId;
        tilePtrTwo->type = tmpType;
        tilePtrTwo->posX = tmpX;
        tilePtrTwo->posY = tmpY;
        
        tilePtrTwo->busy = true;
        CCFiniteTimeAction* actionMoveTwo = CCMoveTo::create(0.2, posOne);
        auto endActionTwo = CallFunc::create([posOne, tilePtrTwo, tmpSprite]() {
            tilePtrTwo->busy = false;
            tilePtrTwo->tile->SetPosition(posOne);
            tilePtrTwo->tile = tmpSprite;
        });
        
        tilePtrTwo->tile->sprite->runAction( CCSequence::create(actionMoveTwo, endActionTwo, NULL) );
    

        printf("AfterSwap\n");
        printGrid();
    }
    
    
    void removeFromGrid(int tileId)
    {
        node *tilePtr;
        printf("Removing tile %d\n", tileId);
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                if(grid[i][j].tileId == tileId)
                {
                    tilePtr = &grid[i][j];
                }
            }
        }
        
        CCFiniteTimeAction* shrink = CCScaleTo::create(0.5, 0);
        tilePtr->busy = true;  auto endActionOne = CallFunc::create([tilePtr]() {
            tilePtr->busy = false;
            printf("Skrinked %d %d\n", tilePtr->posX, tilePtr->posY);
        });
        
        tilePtr->tile->sprite->runAction( CCSequence::create(shrink, endActionOne, NULL) );
    }
    
    bool isActionsOver()
    {
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
               if(grid[i][j].busy)
                   return false;
            }
        }
        return true;
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
    
    
    void fillTheGridWithTiles()
    {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                grid[i][j].tile = new GameTile( grid[i][j].type, grid[i][j].tileId);

                grid[i][j].tile->SetPosition(Point(visibleSize.width/2 + origin.x/2 + 7 + (j * 19),
                                             visibleSize.height/5 + origin.y     + (i * 19)));
                
                grid[i][j].tile->SetScale(0.1, 0.1);
                Scene->addChild(grid[i][j].tile->GetSprite());
            }
        }
    }
    
    void checkAllMatches()
    {
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                bool matched = false;
                std::vector <node*> matchesNodes;
                //printf("START getMatchAtPos at := %2d %2d := \n", i, j);
                matchesNodes = getMatchAtPos(i, j);
                for (auto&& node : matchesNodes)
                {
                    printf("%d ", node->type);
                    matched = true;
                }
                if(matched)
                {
                    printf("\n");
                    destoryMatchedNodes(matchesNodes);
                    printGrid();
                    printf("\n");
                    fillEmptySpaces();
                    printGrid();
                    printf("\n");
                }
                matched = false;
            }
        }
        
    }
    
    std::vector <node*>  getBestMatch()
    {
        std::vector <node*> matchingNodes;
        std::vector <node*> maxMatchingNodes;
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                bool matched = false;
                matchingNodes.clear();
                matchingNodes = getMatchAtPos(i, j);
                if(matchingNodes.size() > maxMatchingNodes.size())
                {
                    maxMatchingNodes = matchingNodes;
                }
            }
        }
        printf("maxMatchingNodes size %l\n", maxMatchingNodes.size());
        return maxMatchingNodes;
    }
    
    void markActive(node* ptr)
    {
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
               if(&grid[i][j] == ptr)
                   ptr->busy = true;
            }
        }
    }
    
    std::vector  <node*> getMatchAtPos(int posX, int posY)
    {
        int maxLeft = 0;
        int maxRight = 0;
        int maxUp = 0;
        int maxDown = 0;
        int matchingTiles = 0;
        std::vector  <node*> matchedNode;
        for(int i = posX - 1; i >= 0; i--)
        {
            if(grid[i][posY].deleted == true)
                continue;
            
            if(grid[posX][posY] == grid[i][posY])
                maxUp++;
            else
                break;
        }
        
        for(int i = posX + 1; i < colCount; i++)
        {
            if(grid[i][posY].deleted == true)
                continue;

            if(grid[posX][posY] == grid[i][posY])
                maxDown++;
            else
                break;
        }
        
        for(int j = posY - 1; j >= 0; j--)
        {
            if(grid[posX][j].deleted == true)
                continue;

            if(grid[posX][posY] == grid[posX][j])
                maxLeft++;
            else
                break;
        }
        
        for(int j = posY + 1; j < rowCount; j++)
        {
            if(grid[posX][j].deleted == true)
                continue;
            
            if(grid[posX][posY] == grid[posX][j])
                maxRight++;
            else
                break;
        }
        
        if((maxLeft + maxRight + 1) >= 3){
            printf("Match in horizontal Direction at %2d,%2d := %d, %d\n", posX, posY, maxLeft , maxRight);
            matchingTiles = maxLeft + maxRight + 1;
            for(int i = posY - maxLeft; i <= posY + maxRight; i++)
            {
                matchedNode.push_back(&grid[posX][i]);
            }
        }
        if((maxUp + maxDown + 1) >= 3) {
            printf("Match in Vertical Direction at %2d,%2d := %d, %d \n", posX, posY, maxUp, maxDown);
            matchingTiles += maxUp + maxDown + 1;
            
            for(int i = posX - maxUp; i <= posX + maxDown; i++)
            {
                printf("%d %d\n", i, posY);
                matchedNode.push_back(&grid[i][posY]);
            }
        }
        return matchedNode;
    }
    
    void destoryMatchedNodes(std::vector <node*> matchedNodes)
    {
        for(auto&&  node: matchedNodes)
            node->type = 0;
    }
    
    void fillEmptySpaces()
    {
        while(countOfEmptySpaces() > 0)
        {
            moveTilesDownWard();
        }		
    }
    
    int countOfEmptySpaces()
    {
        int count = 0;
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                if(grid[i][j].type == 0)
                    count++;	
            }
        }
        return count;
    }
    
    void moveTilesDownWard()
    {
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                if(i == 0 && grid[i][j].type == 0)
                {
                    grid[i][j].type = std::rand() % maxTypeOfTiles + 1;
                    continue;
                }				
                if(grid[i][j].type == 0)
                {
                    grid[i][j].type = grid[i-1][j].type;
                    grid[i-1][j].type = 0;
                }
                
            }
        }
    }
};

static Grid *myGrid;
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
    
    instance = this;
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto backGroundSprite = Sprite::create("relaxBackGround.png");
    backGroundSprite->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    
    this->addChild(backGroundSprite);
    
    myGrid = new Grid(10, 8, this);
    myGrid->fillTheGrid(6);
    myGrid->createGridBackGround();
    myGrid->printGrid();
    myGrid->fillTheGridWithTiles();
    return true;
}

void GameScene::GoToMainMenuScene(cocos2d::Ref *sender){
    auto scene = MainMenuScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}

GameScene* GameScene::getInstance()
{
    return instance;
}

void GameScene::swapListener(int tileId, int direction)
{
    printf("swapListener \n");
    if(!myGrid->isActionsOver())
        return;
     printf("swapListener Grid is free\n");
    int swapWith;
    printf("swapListener %d %d\n",tileId, direction);
    switch(direction)
    {
        case MOVE_LEFT:
            swapWith = myGrid->getTileOn(MOVE_LEFT, tileId);
            if(swapWith == INT16_MAX)
                return;
            break;
        case MOVE_RIGHT:
            swapWith = myGrid->getTileOn(MOVE_RIGHT, tileId);
            if(swapWith == INT16_MAX)
                return;
            break;
        case MOVE_UP:
            swapWith = myGrid->getTileOn(MOVE_UP, tileId);
            if(swapWith == INT16_MAX)
                return;
            break;
        case MOVE_DOWN:
            swapWith = myGrid->getTileOn(MOVE_DOWN, tileId);
            if(swapWith == INT16_MAX)
                return;
            break;
    }

    myGrid->swapTiles(swapWith, tileId);
    instance->schedule(schedule_selector(GameScene::waitForSwapToComplete), 0.1);
}

void GameScene::waitForSwapToComplete(float dt)
{
    printf("Waiting for action to complete\n");
    if(myGrid->isActionsOver())
    {
        instance->unschedule(schedule_selector(GameScene::waitForSwapToComplete));
        printf("Waiting is over\n");
        instance->schedule(schedule_selector(GameScene::hideMatchedTiles), 0.1);
    }
}

void GameScene::hideMatchedTiles(float dt)
{
    if(!myGrid->isActionsOver())
        return;
    std::vector <node*> maxMatchingNodes = myGrid->getBestMatch();
    
    for (auto&& node : maxMatchingNodes)
    {
        printf("%d ", node->type);
        myGrid->markActive(node);
        myGrid->removeFromGrid(node->tileId);
        node->deleted = true;
    }
    instance->unschedule(schedule_selector(GameScene::hideMatchedTiles));
}

void GameScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
