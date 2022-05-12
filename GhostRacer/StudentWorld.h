#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    virtual ~StudentWorld();
    virtual void soulSaved(){souls2save--;}
    virtual void updateDisplayText();
    virtual void addActor(Actor* actorPtr);
    //helpers for zombie cab
    //returns a pointer to the closest actor between the left and right and above the y value
    virtual Actor* CWClosestActorBetweenAbove(int left, int right, int y, int& distance);
    //returns a pointer to closest actor between left and right below the y value
    virtual Actor* CWClosestActorBetweenBelow(int left, int right, int y, int& distance);
    //helper for holy water projectile
    virtual Actor* doHWPcollision(Actor* HWP);
private:
    //helper functions
    virtual void addNewBL();
    virtual void addOilSlick();
    virtual void addHolyWaterGoodie();
    virtual void addLostSoul();
    virtual void addHumanPed();
    virtual void addZombiePed();
    virtual void addZombieCab();
    //helper for zombie cab to choose this lane or not
    virtual bool chooseThisLane(int leftX, int rightX, int& startY, int& startVSpeed, bool& laneAlreadyChosen);
    virtual int getRandXonRoad();
    
    //data members
    GhostRacer* player;
    std::vector<Actor*> actorVec;
    int souls2save;
    int bonus;
    double lastWhiteBLy;
};

#endif // STUDENTWORLD_H_
