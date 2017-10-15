//
//  CloudStorage.h
//  
//
//  Created by jasingh on 10/14/17.
//
//

#ifndef ____CloudStorage__
#define ____CloudStorage__

#include <stdio.h>
#include <HttpClient.h>

class CloudStorage
{
public:
    std::string levelData;
    std::string GetDataFromCloud();
    void ReadTheData(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    int sendDataToCloud();
    
};

#endif /* defined(____CloudStorage__) */
