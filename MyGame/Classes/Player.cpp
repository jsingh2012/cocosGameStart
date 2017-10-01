//
//  Player.cpp
//  
//
//  Created by jasingh on 10/1/17.
//
//

#include "Player.h"

Player *Player::Instance = nullptr;

Player* Player::getPlayer()
{
    
    if(Instance != nullptr)
        return Instance;
    
    Instance =  new Player;
    Instance->lifes = 5;
    Instance->name = "Jatinder";
    Instance->playerId = 17552;
    Instance->totalScore = 0;
    Instance->currentLevel = 1;
    
    return Instance;
}
