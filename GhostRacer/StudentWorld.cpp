#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    //initialize ghost racer and determine number of border lines to add
    player = new GhostRacer(this);
    souls2save = 2 * getLevel() + 5;
    bonus = 5000;
    int N = VIEW_HEIGHT/SPRITE_HEIGHT;
    
    //add the yellow borderlines
    for(int j = 0; j <= N-1; j++){
        BorderLine* leftBL = new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, j*SPRITE_HEIGHT, this, player);
        actorVec.push_back(leftBL);
        BorderLine* rightBL = new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, j*SPRITE_HEIGHT, this, player);
        actorVec.push_back(rightBL);
    }
    
    //determine number of white border lines
    int M = VIEW_WIDTH / (4*SPRITE_HEIGHT);
    
    //add the white borderlines
    for(int j = 0; j <= M-1; j++){
        BorderLine* leftBL = new BorderLine(IID_WHITE_BORDER_LINE, leftMiddleX, j*4*SPRITE_HEIGHT, this, player);
        actorVec.push_back(leftBL);
        BorderLine* rightBL = new BorderLine(IID_WHITE_BORDER_LINE, rightMiddleX, j*4*SPRITE_HEIGHT, this, player);
        actorVec.push_back(rightBL);
        lastWhiteBLy = j*4*SPRITE_HEIGHT;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //get action from player
    player->doSomething();
    
    //iterate through every actor and give them a chance to do something if they are alive
    //at each actor check that the player is alive
    vector<Actor*>::iterator it;
    for(it = actorVec.begin(); it != actorVec.end(); it++){
        Actor* actorPtr = *it;
        
        if( actorPtr->isAlive())
        {
            //get action from actor
            actorPtr->doSomething();
            //check player is still alive
            if(!player->isAlive()){
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            //check if the player won the level
            if (souls2save == 0){
                increaseScore(bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        
        }
        
    }
    
    //delete dead actors
    for(it = actorVec.begin(); it != actorVec.end();){
        Actor* actorPtr = *it;
        
        if(!actorPtr->isAlive()){
            delete actorPtr;
            actorVec.erase(it);
        } else{
            it++;
        }
        
    }
    
    //adjust the data keeping track of the last white border line
    int vertSpeedBL = -4 - player->getVSpeed();
    lastWhiteBLy += vertSpeedBL;
    
    //ADD NEW ACTORS
    addNewBL();
    addOilSlick();
    addHolyWaterGoodie();
    addLostSoul();
    addHumanPed();
    addZombiePed();
    addZombieCab();
    
    //UPDATE DISPLAY TEXT
    updateDisplayText();
    bonus--;
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addNewBL(){
    int delta_y = new_border_y - lastWhiteBLy;
    
    //add new yellow border lines
    if (delta_y >= SPRITE_HEIGHT){
        BorderLine* leftBL = new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y, this, player);
        actorVec.push_back(leftBL);
        BorderLine* rightBL = new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y, this, player);
        actorVec.push_back(rightBL);
    }
    
    //add new white borderlines
    if (delta_y >= 4*SPRITE_HEIGHT){
        BorderLine* leftBL = new BorderLine(IID_WHITE_BORDER_LINE, leftMiddleX, new_border_y, this, player);
        actorVec.push_back(leftBL);
        BorderLine* rightBL = new BorderLine(IID_WHITE_BORDER_LINE, rightMiddleX, new_border_y, this, player);
        actorVec.push_back(rightBL);
        //adjust data holding position of last white borderline's y value
        lastWhiteBLy = new_border_y;
    }
    
}

void StudentWorld::addOilSlick(){
    //roll chance to add oil slick
    int chanceOilSlick = max(150-(getLevel()*10), 40);
    int chance = randInt(0, chanceOilSlick-1);
    
    //add the oil slick to a random x on the road
    if(chance == 0){
        int rand_x = getRandXonRoad();
        OilSlick* newOilSlick = new OilSlick(IID_OIL_SLICK, rand_x, VIEW_HEIGHT, this, player);
        actorVec.push_back(newOilSlick);
    }
}

void StudentWorld::addHolyWaterGoodie(){
    //roll chance for goodie
    int chanceHolyWater = 100 + (10*getLevel());
    int chance = randInt(0, chanceHolyWater-1);
    if(chance == 0){
        //add the holy water goodie to a random x on the road
        int rand_x = getRandXonRoad();
        HolyWaterGoodie* newHWG = new HolyWaterGoodie(IID_HOLY_WATER_GOODIE, rand_x, VIEW_HEIGHT, this, player);
        actorVec.push_back(newHWG);
    }
}

void StudentWorld::addLostSoul(){
    //roll chance of lost soul
    int ChanceOfLostSoul = 100;
    int chance = randInt(0, ChanceOfLostSoul-1);
    if(chance == 0){
        //add lost soul to random x on road
        int rand_x = getRandXonRoad();
        LostSoulGoodie* newLSG = new LostSoulGoodie(IID_SOUL_GOODIE, rand_x, VIEW_HEIGHT, this, player);
        actorVec.push_back(newLSG);
    }
}

void StudentWorld::addHumanPed(){
    //roll chance of human ped being added
    int chanceHumanPed = max(200 - (getLevel() * 10), 30);
    int chance = randInt(0, chanceHumanPed-1);
    if (chance == 0){
        //add human ped ANYWHERE on screen
        int randXAnywhere = randInt(0, VIEW_WIDTH);
        HumanPedestrian* newHP = new HumanPedestrian(IID_HUMAN_PED, 0, randXAnywhere, VIEW_HEIGHT, 2, this, player);
        actorVec.push_back(newHP);
    }
}

void StudentWorld::addZombiePed(){
    //roll chance of zombie ped
    int chanceZombiePed = max(100 - (getLevel()*10), 20);
    int chance = randInt(0, chanceZombiePed-1);
    if (chance == 0){
        //add zombie ped ANYWHERE on screen
        int randXAnywhere = randInt(0, VIEW_WIDTH);
        ZombiePedestrian* newZP = new ZombiePedestrian(randXAnywhere, VIEW_HEIGHT, this, player);
        actorVec.push_back(newZP);
    }
}

void StudentWorld::addZombieCab(){
    //roll chance to attempt to add zombie cab
    int chanceVehicle = max(100 - (getLevel() * 10), 20);
    int chance = randInt(0, chanceVehicle - 1);
    if (chance == 0){
        
        //choose the lane to attempt to add the zombie cab to first
        //also initialize necessary variables
        int curlane = randInt(1,3);
        int chosenlane = -1;
        int startY = 0;
        int startX = 0;
        int startVSpeed = 0;
        bool laneAlreadyChosen = false;
        
            //check lane 1
            if (!laneAlreadyChosen && curlane == 1){
                
                if(chooseThisLane(LEFT_EDGE, leftMiddleX, startY, startVSpeed, laneAlreadyChosen))
                    chosenlane = 1;
                //check lane 2 if not already picked
                if(!laneAlreadyChosen)
                    if(chooseThisLane(leftMiddleX,rightMiddleX, startY, startVSpeed, laneAlreadyChosen))
                        chosenlane = 2;
                //check lane 3 if not already picked
                if(!laneAlreadyChosen)
                    if(chooseThisLane(rightMiddleX, RIGHT_EDGE, startY, startVSpeed, laneAlreadyChosen))
                        chosenlane = 3;
    
            }
        
        //check lane 2
        if (!laneAlreadyChosen && curlane == 2){
            if(chooseThisLane(leftMiddleX,rightMiddleX, startY, startVSpeed, laneAlreadyChosen))
                chosenlane = 2;
            //check lane 3 if not already picked
            if(!laneAlreadyChosen)
                if(chooseThisLane(rightMiddleX, RIGHT_EDGE, startY, startVSpeed, laneAlreadyChosen))
                    chosenlane = 3;
            //check lane 1 if not already picked
            if(!laneAlreadyChosen)
                if(chooseThisLane(LEFT_EDGE, leftMiddleX, startY, startVSpeed, laneAlreadyChosen))
                    chosenlane = 1;
            
        }
        
        //check lane 3
        if (!laneAlreadyChosen && curlane == 3){
            if(chooseThisLane(rightMiddleX, RIGHT_EDGE, startY, startVSpeed, laneAlreadyChosen))
                chosenlane = 3;
            //check lane 2 if not already picked
            if(!laneAlreadyChosen)
                if(chooseThisLane(leftMiddleX,rightMiddleX, startY, startVSpeed, laneAlreadyChosen))
                    chosenlane = 2;
            //check lane 1 if not already picked
            if(!laneAlreadyChosen)
                if(chooseThisLane(LEFT_EDGE, leftMiddleX, startY, startVSpeed, laneAlreadyChosen))
                    chosenlane = 1;
            
        }
        
        //if no lane picked don't add a zombie cab
        if (chosenlane == -1)
            return;
        
        //if lane picked add the zombie cab to the center of that lane
        if (chosenlane == 2){
            //center of lane 2
            startX = ROAD_CENTER;
        }else if (chosenlane == 1){
            //center of lane 1
            startX = ROAD_CENTER - ROAD_WIDTH/3;
        } else if (chosenlane == 3){
            //center of lane 3
            startX = ROAD_CENTER + ROAD_WIDTH/3;
        }
        
        //add the zombie cab to the lane picked
        ZombieCab* newZC = new ZombieCab(startX, startY, this, player, startVSpeed);
        actorVec.push_back(newZC);
        //double check that zombie cab does not immediately collide with player, if it does remove it
        if(newZC->collidesWith(*player))
            actorVec.pop_back();
    }
}

bool StudentWorld::chooseThisLane(int leftX, int rightX, int& startY, int& startVSpeed, bool& laneAlreadyChosen){
    //check if the zombie cab should choose this lane
    int dist = VIEW_HEIGHT;
    Actor* closestCWActorToBottom = CWClosestActorBetweenAbove(leftX, rightX, 0, dist);
    //checks based on the closest actor above where the zombie cab would be
    if ((closestCWActorToBottom == nullptr) || (closestCWActorToBottom->getY() > VIEW_HEIGHT/3)){
        //in this case we are adding zombie cab to the bottom of the lane
        //so set values accordingly
        startY = SPRITE_HEIGHT/2;
        startVSpeed = player->getVSpeed() + randInt(2, 4);
        laneAlreadyChosen = true;
        return true;
    }
    dist = VIEW_HEIGHT;
    Actor* closestCWActorToTop = CWClosestActorBetweenBelow(leftX, rightX, VIEW_HEIGHT, dist);
    //checks based on closest actor below where the zombie cab would be
        if ((closestCWActorToTop == nullptr) || (closestCWActorToTop->getY() < VIEW_HEIGHT * 2/3)){
            //in this case we are adding zombie cab to the top of the lane so set
            //values accordingly
            startY = VIEW_HEIGHT - SPRITE_HEIGHT/2;
            startVSpeed = player->getVSpeed() - randInt(2, 4);
            laneAlreadyChosen = true;
            return true;
        }
    //else we didnt choose a lane
    return false;
}

int StudentWorld::getRandXonRoad(){
    //get a random x on the road
    int rand_x = randInt(LEFT_EDGE, RIGHT_EDGE);
    return rand_x;
}

void StudentWorld::addActor(Actor* actorPtr){
    //add actor to actor vector
    actorVec.push_back(actorPtr);
}

Actor* StudentWorld::CWClosestActorBetweenAbove(int left, int right, int y, int& distance){
    //if zombie cab somehow not in lane, return
    if ((left == -1) || (right == -1))
        return nullptr;
        
    //declare vars needed
    vector<Actor*>::iterator it;
    Actor* actorPtr;
    Actor* closestActor = nullptr;
    int new_distance;
    
    //checks each collision worthy actor between the left and right x values and aboe
    //the y value, sets distance to the closest such actor found and sets cloest actor
    //to that actor
    for(it = actorVec.begin(); it != actorVec.end(); it++){
        actorPtr = *it;
        if ((actorPtr->isCollisionWorthy()) && (actorPtr->getX() >= left) && (actorPtr->getX() < right) && (actorPtr->getY() > y)){
            new_distance = actorPtr->getY() - y;
            if (new_distance <= distance){
                distance = new_distance;
                closestActor = actorPtr;
            }
        }
    }
    
    //check if the player is the closest actor between x limits and above y value
    int playerX = player->getX();
    if(playerX >= left && playerX < right){
    new_distance = player->getY() - y;
        //replace distance value if player is closest and return player pointer
    if (new_distance <= distance){
        distance = new_distance;
        closestActor = player;
        }
        
    }
    
    return closestActor;
}

Actor* StudentWorld::CWClosestActorBetweenBelow(int left, int right, int y, int& distance){
    //if zombie cab somehow not in lane, return
    if ((left == -1) || (right == -1))
        return nullptr;
        
    //declare vars
    vector<Actor*>::iterator it;
    Actor* actorPtr;
    Actor* closestActor = nullptr;
    int new_distance;
    
    //checks each collision worthy actor that is between the left and right x limits
    //and below the y value, sets the distance that they are from the y value
    //and sets closest actor to be a pointer to that actor
    for(it = actorVec.begin(); it != actorVec.end(); it++){
        actorPtr = *it;
        if ((actorPtr->isCollisionWorthy()) && (actorPtr->getX() >= left) && (actorPtr->getX() < right) && (actorPtr->getY() < y)){
            new_distance = y - actorPtr->getY();
            if (new_distance <= distance){
                distance = new_distance;
                closestActor = actorPtr;
            }
        }
    }
    
    //check if the player is the closest actor between the x values and below the y value
    int playerX = player->getX();
    if(playerX >= left && playerX < right){
    new_distance = y - player->getY();
        //if the player is the closest, set distance to the new distance and return player pointer
    if (new_distance <= distance){
        distance = new_distance;
        closestActor = player;
        }
    }
    return closestActor;
}

Actor* StudentWorld::doHWPcollision(Actor* HWP){
    //helper function for holy water projectile that checks collisions for it
    vector<Actor*>::iterator it;
    Actor* actorPtr;
    
    //check if the holy water projectile collides with the actor and the actor is
    //collsion worthy, return the first actor to do so
    for(it = actorVec.begin(); it != actorVec.end(); it++){
        actorPtr = *it;
        if ((HWP->collidesWith(*actorPtr)) && (actorPtr->isCollisionWorthy()) && actorPtr != HWP)
            return actorPtr;
    }
    //else return nullptr since no collision
    return nullptr;
}

void StudentWorld::updateDisplayText(){
    //update the display text
    string statusText = "Score: " + to_string(getScore()) + "  " +
                        "Lvl: " + to_string(getLevel()) + "  " +
                        "Souls2Save: " + to_string(souls2save) + "  " +
                        "Lives: " + to_string(getLives()) + "  " +
                        "Health: " + to_string(player->getHealth()) + "  " +
                        "Sprays: " + to_string(player->getSprays()) + "  " +
                        "Bonus: " + to_string(bonus);
    //uses game world set game stat text
    setGameStatText(statusText);
}

void StudentWorld::cleanUp()
{
    
    delete player;
    
    //iterate through actor vec and delete each actor
    vector<Actor*>::iterator it;
    for(it = actorVec.begin(); it != actorVec.end(); it++){
        delete *it;
    }
    
    //clear the actor vector
    actorVec.clear();
}

StudentWorld::~StudentWorld(){
    cleanUp();
}


