//
//  CloudStorage.cpp
//  
//
//  Created by jasingh on 10/14/17.
//
//

#include "CloudStorage.h"
#include "LevelsData.h"
#include <HttpClient.h>

std::string CloudStorage::GetDataFromCloud()
{
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl("https://ec2-54-186-234-18.us-west-2.compute.amazonaws.com/playerDeatils/levelData");
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(CloudStorage::ReadTheData, this) );
    request->setTag("GET test1");
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}

void CloudStorage::ReadTheData(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    printf("Data downloaded from the server\n");
    if (response && response->getResponseCode() == 200 && response->getResponseData()) {
        std::vector<char> *data = response->getResponseData();
        std::string ret(&(data->front()), data->size());
        LevelsData *levelData = LevelsData::getInstance();
        levelData->reloadData(ret);
        //CCLOG("%s", ("Response message: " + ret).c_str());
    }
    else {
        CCLOG("%s", ("Error " + std::to_string(response->getResponseCode()) + " in request").c_str()); //error
    }
}