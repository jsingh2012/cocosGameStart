#include "GameScene.h"
#include "Definitions.h"
#include "MainMenuScene.h"
#include "GameTile.h"
#include "GameScene.h"
#include "Player.h"
#include "string.h"

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
    cocos2d::Vec2 position;
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
    node grid[15][15];
    static Grid *instance;
    int rowCount;
    int colCount;
    int maxTypeOfTiles;
    GameScene *Scene;
    
public:
    int moveLeft;
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
        grid[9][9].posX = INT16_MAX;
        grid[9][9].posY = INT16_MAX;

        //checkAllMatches();
    }
    
    static int getNewID()
    {
        return ++node::id;
    }
    
    void printGrid()
    {
        for(int i = (rowCount - 1) ; i >= 0; i--)
        {
            for(int j = 0; j < colCount; j++)
            {
                if(grid[i][j].deleted == true ) {
                    printf( "   (%2d)[%d,%d] ",   grid[i][j].tileId, grid[i][j].posX, grid[i][j].posY);
                    // printf( "(%d)%2d", grid[i][j].tileId, grid[i][j].type);
                }
                else {
                    printf( "%d (%2d)[%d,%d] ", grid[i][j].type, grid[i][j].tileId, grid[i][j].posX, grid[i][j].posY);
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
        
        tilePtrOne->tileId = tilePtrTwo->tileId;
        tilePtrOne->type = tilePtrTwo->type;
        
        // Create the actions
        CCFiniteTimeAction* actionMoveOne = CCMoveTo::create(0.2, tilePtrTwo->position);
        
        auto endActionOne = CallFunc::create([tilePtrTwo, tilePtrOne]() {
            tilePtrOne->busy = false;
            tilePtrOne->tile->SetPosition(tilePtrTwo->position);
            tilePtrOne->tile = tilePtrTwo->tile;
        });
        
        tilePtrOne->tile->sprite->runAction( CCSequence::create(actionMoveOne, endActionOne, NULL) );
        
        tilePtrTwo->tileId = tmpTileId;
        tilePtrTwo->type = tmpType;

        
        tilePtrTwo->busy = true;
        CCFiniteTimeAction* actionMoveTwo = CCMoveTo::create(0.2, tilePtrTwo->position);
        auto endActionTwo = CallFunc::create([tilePtrOne, tilePtrTwo, tmpSprite, this]() {
            tilePtrTwo->busy = false;
            tilePtrTwo->tile->SetPosition(tilePtrOne->position);
            tilePtrTwo->tile = tmpSprite;
            printf("After Swap\n");
            printGrid();
        });
        
        tilePtrTwo->tile->sprite->runAction( CCSequence::create(actionMoveTwo, endActionTwo, NULL) );
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
                    LightBlue->setPosition(Point(visibleSize.width/2 + origin.x/2 + 10 + (j * 19),
                                                 visibleSize.height/5 + origin.y     + (i * 19)));
                    LightBlue->setScaleY(TILE_Y_SCALE);
                    LightBlue->setScaleX(TILE_X_SCALE);
                    Scene->addChild(LightBlue);

                }
                else
                {
                    auto GrayBlue = Sprite::create("GrayBlueTile.png");
                    GrayBlue->setPosition(Point(visibleSize.width/2 + origin.x/2 + 10 + (j * 19),
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

                grid[i][j].tile->SetPosition(Point(visibleSize.width/2 + origin.x/2 + 10 + (j * 19),
                                             visibleSize.height/5 + origin.y     + (i * 19)));
                
                grid[i][j].tile->SetScale(1.1, 1.1);
                Scene->addChild(grid[i][j].tile->GetSprite());
                grid[i][j].position = Point(visibleSize.width/2 + origin.x/2 + 10 + (j * 19),
                                            visibleSize.height/5 + origin.y     + (i * 19));
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
            node->deleted = true;
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
                if(grid[i][j].deleted == true)
                    count++;	
            }
        }
        printf("countOfEmptySpaces %d\n", count);
        return count;
    }
    
    void moveTilesDownWard()
    {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        printf("moveTilesDownWard Start \n");
        printGrid();
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                if(i == (rowCount - 1) && grid[i][j].deleted == true)
                {
                    printf("new tile created");
                    grid[i][j].type = std::rand() % maxTypeOfTiles + 1;
                    grid[i][j].tileId = getNewID();
                    grid[i][j].tile = new GameTile( grid[i][j].type, grid[i][j].tileId);
                    
                    grid[i][j].tile->SetPosition(Point(visibleSize.width/2 + origin.x/2 + 10 + (j * 19),
                                                       visibleSize.height/5 + origin.y     + (i * 19)));
                    
                    grid[i][j].tile->SetScale(1.1, 1.1);
                    Scene->addChild(grid[i][j].tile->GetSprite());
                  
                    grid[i][j].deleted = false;
                    continue;
                }				
                if(grid[i][j].deleted == true && grid[i+1][j].deleted == false)
                {
                    node *tilePtrUp = &grid[i+1][j];
                    node *tilePtrDown = &grid[i][j];
                    
                    tilePtrDown->type = tilePtrUp->type;
                    tilePtrDown->tileId = tilePtrUp->tileId;
                    tilePtrDown->tile = tilePtrUp->tile;
                    tilePtrUp->deleted = true;
                    tilePtrDown->deleted = false;
                    
                    tilePtrDown->busy = true;
                    
                    
                    CCFiniteTimeAction* actionMoveDown = CCMoveTo::create(0.11, tilePtrDown->position);
                    
                    auto actionDone = CallFunc::create([tilePtrUp, tilePtrDown ]() {
                        tilePtrDown->busy = false;
                    });
                    
                    tilePtrDown->tile->sprite->runAction(CCSequence::create(actionMoveDown, actionDone, NULL));
                }
            }
        }
    }
    
    void correctPositions()
    {
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
               // grid[i][j].tile->position = grid[i][j].tile->new_position;
            }
        }
    }
};

static Grid *myGrid;
static Player *player;
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
    
    //MAX
    myGrid = new Grid(11, 8, this);
    myGrid->fillTheGrid(6);
    myGrid->createGridBackGround();
    myGrid->printGrid();
    myGrid->fillTheGridWithTiles();
    myGrid->moveLeft = 10;
    
    player = Player::getPlayer();
    
    CreateTopBar();
    return true;
}

void GameScene::CreateTopBar()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto topLeft = Sprite::create("score.png");
    topLeft->setPosition(Point(visibleSize.width/1.4 + origin.x + 10, visibleSize.height/1.05 + origin.y - 10));
    topLeft->setScaleX(2);
    topLeft->setScaleY(1.5);
    this->addChild(topLeft, 10);
    
    //First way to create a LabelTTF by TTF files
    const std::string scoreCount = "Score : "+ StringUtils::toString(Player::getPlayer()->totalScore);
    const std::string lifeCount  = "Life : "+ StringUtils::toString(Player::getPlayer()->lifes);
    const std::string movesCount = StringUtils::toString(myGrid->moveLeft);
    
    
    scoreLabel = Label::createWithTTF(scoreCount,"fonts/Marker Felt.ttf", 9);
    scoreLabel->setPosition(Point(visibleSize.width/1.4 + origin.x + 10, visibleSize.height/1.05 + origin.y - 10));
    scoreLabel->setColor( Color3B( 25,   176, 78));
    this->addChild(scoreLabel, 11);
    
    auto topRight = Sprite::create("score.png");
    topRight->setPosition(Point(visibleSize.width/4 + origin.x, visibleSize.height/1.05 + origin.y - 10));
    topRight->setScaleX(2);
    topRight->setScaleY(1.5);
    this->addChild(topRight, 10);
    
    //First way to create a LabelTTF by TTF files
    lifeLabel = Label::createWithTTF(lifeCount,"fonts/Marker Felt.ttf", 9);
    lifeLabel->setPosition(Point(visibleSize.width/4 + origin.x, visibleSize.height/1.05 + origin.y - 10));
    lifeLabel->setColor( Color3B( 25,   176, 78));
    this->addChild(lifeLabel, 11);
    
    auto topCenter = Sprite::create("score.png");
    topCenter->setPosition(Point(visibleSize.width/2 + origin.x + 2, visibleSize.height/1.05 + origin.y - 10));
    topCenter->setScaleX(0.9);
    topCenter->setScaleY(2.2);
    this->addChild(topCenter, 10);
    
    MovesLabel = Label::createWithTTF(movesCount,"fonts/Marker Felt.ttf", 9);
    MovesLabel->setPosition(Point(visibleSize.width/2 + origin.x + 2, visibleSize.height/1.05 + origin.y - 10));
    MovesLabel->setColor( Color3B( 25,   176, 78));
    this->addChild(MovesLabel, 11);
    
}

void GameScene::UpdateTopBar()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    //First way to create a LabelTTF by TTF files
    const std::string scoreCount = "Score : "+ StringUtils::toString(Player::getPlayer()->totalScore);
    const std::string lifeCount  = "Life : "+ StringUtils::toString(Player::getPlayer()->lifes);
    const std::string movesCount = StringUtils::toString(myGrid->moveLeft);
    
    scoreLabel->setString(scoreCount);
    lifeLabel->setString(lifeCount);
    MovesLabel->setString(movesCount);
    if(myGrid->moveLeft <= 5)
        MovesLabel->setColor(Color3B( 255,   10, 10));
    
    if(myGrid->moveLeft == 0 && Player::getPlayer()->totalScore < 10000)
    {
        auto topRight = Sprite::create("you_lose.png");
        topRight->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        topRight->setScaleX(2);
        topRight->setScaleY(1.5);
        this->addChild(topRight, 10);
    }
    
    else if(Player::getPlayer()->totalScore >= 10000)
    {
        auto topRight = Sprite::create("you_win.png");
        topRight->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        topRight->setScaleX(2);
        topRight->setScaleY(1.5);
        this->addChild(topRight, 10);
    }
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
    myGrid->moveLeft--;
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
    instance->unschedule(schedule_selector(GameScene::hideMatchedTiles));
    bool matched = false;

    do{
        std::vector <node*> maxMatchingNodes = myGrid->getBestMatch();
        matched = false;
        for (auto&& node : maxMatchingNodes)
            {
                printf("%d ", node->type);
                myGrid->markActive(node);
                myGrid->removeFromGrid(node->tileId);
                node->deleted = true;
                matched = true;
                player->totalScore += 10;
                UpdateTopBar();
            }
    } while(matched == true);
    
    if(matched == false)
    {
        instance->schedule(schedule_selector(GameScene::startTheFall), 0.01);
    }
}

void GameScene::startTheFall(float dt)
{
    //instance->unschedule(schedule_selector(GameScene::startTheFall));
    static  bool funtion = true;
    printf("Start the startTheFall\n");
    if(!myGrid->isActionsOver())
        return;
    myGrid->printGrid();
    myGrid->correctPositions();
    if(myGrid->countOfEmptySpaces() > 0)
    {
        myGrid->moveTilesDownWard();
    }
    else
    {
        myGrid->correctPositions();
        instance->unschedule(schedule_selector(GameScene::startTheFall));
        UpdateTopBar();
    }
    
}

void GameScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
