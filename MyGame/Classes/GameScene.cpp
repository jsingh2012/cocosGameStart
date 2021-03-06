#include "GameScene.h"
#include "Definitions.h"
#include "MainMenuScene.h"
#include "GameTile.h"
#include "GameScene.h"
#include "Player.h"
#include "string.h"
#include "LevelsData.h"
#include "FileOperation.h"
#include "CloudStorage.h"

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
    int startX;
    int startY;
    GameScene *Scene;
    
public:
    int moveLeft;
    bool win = false;
    bool lose = false;
    Grid(const int _rowCount ,const int _colCount, int startX, int startY, GameScene* ptr)
    {
        // dynamically allocate memory of size M*N
        this->rowCount = _rowCount + startX;
        this->colCount = _colCount + startY;
        this->startX = startX;
        this->startY = startY;
        Scene = ptr;
    }

    void fillTheGrid(int maxDiffTileType)
    {
        maxTypeOfTiles = maxDiffTileType;
        //std::srand(std::time(0));
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        for(int i = (rowCount - 1) ; i >= startX; i--)
        {
            for(int j = startY; j < colCount; j++)
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
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        
        // Create the actions
        CCFiniteTimeAction* actionMoveOne = CCMoveTo::create(0.2, tilePtrTwo->position);
        
        CCFiniteTimeAction*  endActionOne = CallFunc::create([tilePtrTwo, tilePtrOne]() {
            tilePtrOne->busy = false;
            tilePtrOne->tile->SetPosition(tilePtrTwo->position);
            tilePtrOne->tile = tilePtrTwo->tile;
        });
        
        CCFiniteTimeAction* actionMoveTwo = CCMoveTo::create(0.2, tilePtrTwo->position);
        
        CCFiniteTimeAction*  endActionTwo = CallFunc::create([tilePtrOne, tilePtrTwo, tmpSprite]() {
            tilePtrTwo->busy = false;
            tilePtrTwo->tile->SetPosition(tilePtrOne->position);
            tilePtrTwo->tile = tmpSprite;
            printf("After Swap\n");
        });
        
        
        tilePtrOne->type = tilePtrTwo->type;
        tilePtrTwo->type = tmpType;
        
        printf("Check feasibility\n");
        printGrid();
        std::vector <node*> matchesNodesOne;
        matchesNodesOne = getMatchAtPos(tilePtrOne->posX, tilePtrOne->posY);
        
        printf("matchesNodesOne.size() %zu\n", matchesNodesOne.size());
        std::vector <node*> matchesNodesTwo;
        matchesNodesTwo = getMatchAtPos(tilePtrTwo->posX, tilePtrTwo->posY);
        printf("matchesNodesTwo.size() %zu\n", matchesNodesTwo.size());
        
        tilePtrTwo->busy = true;
        if(matchesNodesOne.size() == 0 && matchesNodesTwo.size() == 0)
        {
            tmpType = tilePtrOne->type;
            tilePtrOne->type = tilePtrTwo->type;
            tilePtrTwo->type = tmpType;
            printf("No Swap possible");
            tilePtrTwo->busy = false;
            tilePtrOne->busy = false;
            endActionOne = CCMoveTo::create(0.2, tilePtrOne->position);
            endActionTwo = CCMoveTo::create(0.2, tilePtrTwo->position);
        }
        else
        {
            tilePtrOne->tileId = tilePtrTwo->tileId;
            tilePtrTwo->tileId = tmpTileId;
        }
        
        tilePtrOne->tile->sprite->runAction( CCSequence::create(actionMoveOne, endActionOne, NULL) );
        tilePtrTwo->tile->sprite->runAction( CCSequence::create(actionMoveTwo, endActionTwo, NULL) );
    }
    
    
    void removeFromGrid(int tileId)
    {
        node *tilePtr;
        printf("Removing tile %d\n", tileId);
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        
        for(int i = startX ; i < rowCount; i++)
        {
            int rowEvenOdd = i % 2;
            for(int j = startY; j < colCount; j++)
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
    
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        printf("maxMatchingNodes size %zu\n", maxMatchingNodes.size());
        return maxMatchingNodes;
    }
    
    void markActive(node* ptr)
    {
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        if(grid[posX][posY].deleted == true)
            return matchedNode;
        for(int i = posX - 1; i >= startX; i--)
        {
            if(grid[i][posY].deleted == true)
                continue;
            
            if(grid[posX][posY] == grid[i][posY])
                maxUp++;
            else
                break;
        }
        
        for(int i = posX + 1; i <= colCount; i++)
        {
            if(grid[i][posY].deleted == true)
                continue;

            if(grid[posX][posY] == grid[i][posY])
                maxDown++;
            else
                break;
        }
        
        for(int j = posY - 1; j >= startY; j--)
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
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        for(int i = startX ; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
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
        for(int i = startX; i < rowCount; i++)
        {
            for(int j = startY; j < colCount; j++)
            {
               // grid[i][j].tile->position = grid[i][j].tile->new_position;
            }
        }
    }
};

static Grid *myGrid;
static Player *player;
static LevelsData *levels;
static level LevelData;
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
    cocos2d::Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
    levels = LevelsData::getInstance();
    player = Player::getPlayer();
    player->totalScore = 0;
    
    LevelData = LevelsData::getInstance()->getLevelData(player->currentLevel);
    printf("New level start %d\n",player->currentLevel);
    //MAX
    myGrid = new Grid(LevelData.row, LevelData.col, LevelData.startX, LevelData.startY, this);
    myGrid->fillTheGrid(6);
    myGrid->createGridBackGround();
    myGrid->printGrid();
    myGrid->fillTheGridWithTiles();
    myGrid->moveLeft = 10;

    CreateTopBar();
    instance->schedule(schedule_selector(GameScene::hideMatchedTiles), 0.1);
    
    FileOperation file;
    file.saveFile();// ();
    
    CloudStorage cloud;
    cloud.GetDataFromCloud();
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
    const std::string LevelNo = "Level " + StringUtils::toString(Player::getPlayer()->currentLevel);
    const std::string targetScore = "Taget score "+ StringUtils::toString(LevelData.targetScore);
    
    scoreLabel = Label::createWithTTF(LevelNo,"fonts/Marker Felt.ttf", 9);
    scoreLabel->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/1.05 + origin.y - 30));
    scoreLabel->setColor( Color3B( 250,   255, 78));
    this->addChild(scoreLabel, 11);
    
    scoreLabel = Label::createWithTTF(targetScore,"fonts/Marker Felt.ttf", 9);
    scoreLabel->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/1.05 + origin.y - 40));
    scoreLabel->setColor( Color3B( 250,   255, 78));
    this->addChild(scoreLabel, 11);
    
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
    
    if(myGrid->moveLeft == 0 && Player::getPlayer()->totalScore < LevelData.targetScore && myGrid->lose == false)
    {
        printf("You Lost level %d\n", player->currentLevel);
        auto topRight = Sprite::create("you_lose.png");
        topRight->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        topRight->setScaleX(2);
        topRight->setScaleY(1.5);
        this->addChild(topRight, 10);
        myGrid->lose = true;
        
        auto playItem = MenuItemImage::create( "ReplayButton.png", "ReplayButton.png", CC_CALLBACK_1(GameScene::ReplayLevel, this));
        playItem->setPosition( Point( visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 100) );
    
        auto menu = Menu::create( playItem, NULL );
        menu->setPosition( Point::ZERO );
        this->addChild( menu, 10);
    }
    
    else if(Player::getPlayer()->totalScore >= LevelData.targetScore && myGrid->win == false)
    {
        auto topRight = Sprite::create("you_win.png");
        topRight->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        topRight->setScaleX(2);
        topRight->setScaleY(1.5);
        this->addChild(topRight, 10);
        myGrid->win = true;
        printf("You win level %d\n", player->currentLevel);
        
        auto playItem = MenuItemImage::create( "NextButton.png", "NextButton.png", CC_CALLBACK_1(GameScene::MoveToNextLevel, this));
        playItem->setPosition( Point( visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 100) );
        
        auto menu = Menu::create( playItem, NULL );
        menu->setPosition( Point::ZERO );
        this->addChild( menu, 10);
    }
}

void GameScene::ReplayLevel(cocos2d::Ref *sender){
    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}

void GameScene::MoveToNextLevel(cocos2d::Ref *sender){
    player->currentLevel++;
    auto scene = GameScene::createScene();
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
    if(myGrid->win || myGrid->lose)
        return;
    
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

    
    myGrid->swapTiles(tileId, swapWith);
    instance->schedule(schedule_selector(GameScene::waitForSwapToComplete), 0.2);
    
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
            }
    } while(matched == true);
    
    if(matched == false)
    {
        if(myGrid->countOfEmptySpaces() > 0)
        {
            instance->schedule(schedule_selector(GameScene::startTheFall), 0.01);
        }
        else
        {
            myGrid->moveLeft--;
        }
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
        instance->schedule(schedule_selector(GameScene::hideMatchedTiles), 0.1);
    }
    
}

void GameScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
