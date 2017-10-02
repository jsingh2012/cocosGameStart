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
    int score;
    int moves;
    int levelNo;
};

class LevelsData{
    static struct level *levels;
    static LevelsData *Instance;
    
public:
    static LevelsData* getInstance();
    struct level getLevelData(int no);
    
    static void init();
    
};

#endif /* defined(____LevelsData__) */
