//
//  Player.h
//  
//
//  Created by jasingh on 10/1/17.
//
//

#ifndef ____Player__
#define ____Player__

class Player
{
private:
    static Player *Instance;
public:
    std::string name;
    int currentLevel;
    int totalScore;
    int lifes;
    int playerId;
    
    static Player* getPlayer();
    
};
#endif /* defined(____Player__) */
