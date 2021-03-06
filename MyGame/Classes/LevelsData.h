//
//  LevelsData.h
//  
//
//  Created by jasingh on 10/1/17.
//
//

#ifndef ____LevelsData__
#define ____LevelsData__

#include <stdio.h>

struct level
{
    int row;
    int col;
    int startX;
    int startY;
    int targetScore;
    int moves;
    int levelNo;
};

class LevelsData{
    static struct level *levels;
    static LevelsData *Instance;
    void loadTheData(std::string data);
    
public:
    static LevelsData* getInstance();
    struct level getLevelData(int no);
    void reloadData(std::string data);
    static void init();
    
};

#endif /* defined(____LevelsData__) */
