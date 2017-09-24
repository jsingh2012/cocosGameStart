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
        
        checkAllMatches();
    }
    
    void printGrid()
    {
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                if(grid[i][j].type == 0) {
                    //printf( "[%2d,%2d] %2d", i,j, grid[i][j].type);
                     printf( "%2d", grid[i][j].type);
                }
                else {
                    //printf( "[%2d,%2d] %2d", i,j, grid[i][j].type);
                    printf( "%2d",  grid[i][j].type);
                }
            }
            printf("\n");
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
    
    
    void fillTheGridWithTiles()
    {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
        for(int i = 0 ; i < rowCount; i++)
        {
            for(int j = 0; j < colCount; j++)
            {
                Sprite* tile;
                if(grid[i][j].type == 1)
                    tile= Sprite::create("unlit-bomb.png");
                if(grid[i][j].type == 2)
                    tile = Sprite::create("diplodocusBlue.png");
                if(grid[i][j].type == 3)
                    tile = Sprite::create("tiger-head.png");
                if(grid[i][j].type == 4)
                    tile = Sprite::create("gargoyle.png");
                if(grid[i][j].type == 5)
                    tile = Sprite::create("morph-ball.png");
                if(grid[i][j].type == 6)
                    tile = Sprite::create("magic-hat.png");
                
                
                tile->setPosition(Point(visibleSize.width/2 + origin.x/2 + 7 + (j * 19),
                                             visibleSize.height/5 + origin.y     + (i * 19)));
                tile->setScaleY(0.1);
                tile->setScaleX(0.1);
                Scene->addChild(tile);
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
            if(grid[posX][posY] == grid[i][posY])
                maxUp++;
            else
                break;
        }
        
        for(int i = posX + 1; i < colCount; i++)
        {
            if(grid[posX][posY] == grid[i][posY])
                maxDown++;
            else
                break;
        }
        
        for(int j = posY - 1; j >= 0; j--)
        {
            if(grid[posX][posY] == grid[posX][j])
                maxLeft++;
            else
                break;
        }
        
        for(int j = posY + 1; j < rowCount; j++)
        {
            if(grid[posX][posY] == grid[posX][j])
                maxRight++;
            else
                break;
        }
        
        if((maxLeft + maxRight + 1) >= 3){
            printf("Match in horizontal Direction at %2d %2d := %d\n", posX, posY, (maxLeft + maxRight + 1));
            matchingTiles = maxLeft + maxRight + 1;
            for(int i = posY - maxLeft; i <= posY + maxRight; i++)
            {
                matchedNode.push_back(&grid[posX][i]);
            }
        }
        if((maxUp + maxDown + 1) >= 3) {
            printf("Match in Vertical Direction at %2d %2d := %d\n", posX, posY, (maxUp + maxDown + 1));
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
    myGrid.fillTheGrid(6);
    myGrid.createGridBackGround();
    myGrid.printGrid();
    myGrid.fillTheGridWithTiles();
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
