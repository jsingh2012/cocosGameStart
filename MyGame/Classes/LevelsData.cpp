//
//  LevelsData.cpp
//  
//
//  Created by jasingh on 10/1/17.
//
//

#include "LevelsData.h"
#include "Definitions.h"
#include "json11.hpp"
#include <fstream>

LevelsData* LevelsData::Instance = nullptr;
level* LevelsData::levels = new level[100];

LevelsData* LevelsData::getInstance()
{
    if(Instance == nullptr) {
        Instance = new LevelsData();
        init();
    }

    return Instance;
}

void LevelsData::init()
{
    std::string buf;
    std::string line;
    
    std::ifstream myfile (LEVELS_DATA_FILE);
    while (std::getline(myfile, line)) {
        buf += line + "\n";
        
    }
    
    std::string err;
    auto json = json11::Json::parse(buf, err);
    if (!err.empty()) {
        printf("Failed: %s\n", err.c_str());
    } else {
        printf("Result: %s\n", json.dump().c_str());
    }
    
    for(int i = 1; i <= 2; i++)
    {
        std::string data;
        json["data"][std::to_string(i)].dump(data);
        printf("Jsingh := %s\n", data.c_str());
        
        levels[i].row = json["data"][std::to_string(i)]["row"].int_value();
        levels[i].col = json["data"][std::to_string(i)]["col"].int_value();
        levels[i].moves = json["data"][std::to_string(i)]["moves"].int_value();
        levels[i].score = json["data"][std::to_string(i)]["targetScore"].int_value();
        levels[i].startX = json["data"][std::to_string(i)]["startX"].int_value();
        levels[i].startY = json["data"][std::to_string(i)]["startY"].int_value();
    }
}

level LevelsData::getLevelData(int no)
{
    return levels[no];
}