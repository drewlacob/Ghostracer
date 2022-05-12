#include "Actor.h"
#include "StudentWorld.h"
#include <math.h>
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//ACTOR IMPLEMENTATIONS

Actor::Actor(int imageID, double startX, double startY, int startDirection, double size, unsigned int depth, double vspeed, double hspeed, bool cw, StudentWorld* sw, GhostRacer* GR)
:GraphObject(imageID, startX, startY, startDirection, size, depth)
{
    m_alive = true;
    m_collisionWorthy = cw;
    m_verticalSpeed = vspeed;
    m_horizontalSpeed = hspeed;
    m_sw = sw;
    m_GRptr = GR;
};

Actor::~Actor(){
    
};

void Actor::move(){
    //general algorithm that moves actors down the screen
    double newVertSpeed;
    double currentSpeed = getVSpeed();
    double grSpeed = m_GRptr->getVSpeed();
    newVertSpeed = currentSpeed - grSpeed;
    
    double newHorizSpeed = getHSpeed();
    //new x and y positions to move to
    double newY = getY() + newVertSpeed;
    double newX = getX() + newHorizSpeed;
    
    //uses graph object moveTo
    moveTo(newX, newY);
    //uses checkOffScreen seen below
    checkOffScreen();
};

void Actor::checkOffScreen(){
    //if the actor is off the screen set it to not alive
  if (getX() < 0 || getY() < 0)
      m_alive = false;
  if(getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
      m_alive = false;
};

bool Actor::collidesWith(Actor& other){
    //collision algorithm from project spec
    double delta_x = abs(getX() - other.getX());
    double delta_y = abs(getY() - other.getY());
    double radius_sum = getRadius() + other.getRadius();
    
    if ((delta_x < radius_sum*0.25) && (delta_y < radius_sum*0.6))
        return true;
    
    return false;
};

//END ACTOR IMPLEMENTATIONS


//GHOSTRACER IMPLEMENTATIONS
GhostRacer::GhostRacer(StudentWorld* sw)
:AliveActor(IID_GHOST_RACER, 128, 32, 90, 4, 0, 0, 0, true, sw, this, 100)
{
    m_numHolySprays = 10;
};

GhostRacer::~GhostRacer(){
    
};

void GhostRacer::doSomething(){
    //step 1, immediately return if ghost racer is dead
    if(getHealth() <= 0 || !isAlive())
    return;
    
    int ch;
    
    //step 2, check if hitting left wall of 405
    if (getX() <= LEFT_EDGE){
        
        if(getDirection() > 90){
            takeDamage(10);
        //since we hit, rebound, play sounds, and adjust player position
        setDirection(82);
        
        getSW()->playSound(SOUND_VEHICLE_CRASH);
        
        GRMovementAlg();
        return;
        }
    }
    
    //step 3, check if hitting right wall of 405
    if (getX() >= RIGHT_EDGE){
        
        if(getDirection() < 90){
            takeDamage(10);
        //since we hit, rebound, play sounds, and adjust player position
        setDirection(98);
        
        getSW()->playSound(SOUND_VEHICLE_CRASH);
        
        GRMovementAlg();
        return;
        }
    }
    
    if (getSW()->getKey(ch)) {
        // user hit a key during this tick!
        switch (ch)
        {
        case KEY_PRESS_LEFT:
        //... move Ghost Racer counterclockwise ...;
                if (getDirection() < 114){
                    setDirection(getDirection() + 8);
                    GRMovementAlg();
                    return;
                }
        break;
        case KEY_PRESS_RIGHT:
        //... move Ghost Racer clockwise...;
                if (getDirection() > 66){
                    setDirection(getDirection() - 8);
                    GRMovementAlg();
                    return;
                }
        break;
        case KEY_PRESS_SPACE:
        //... add spray in front of Ghost Racer...;
                if(m_numHolySprays > 0){
                    //calculate the x and y position for the holy water projectile
                    double dirInRadians = getDirection() * 3.1415926/180;
                    int delta_x = SPRITE_HEIGHT * cos(dirInRadians);
                    int delta_y = SPRITE_HEIGHT * sin(dirInRadians);
                    int x = getX() + delta_x;
                    int y = getY() + delta_y;
                    //create the new projectile and add to actorVec
                    HolyWaterProjectile* newHWP = new HolyWaterProjectile(IID_HOLY_WATER_PROJECTILE, x, y, getDirection(), getSW(), this);
                    getSW()->addActor(newHWP);
                    //play sounds, decrease spray
                    getSW()->playSound(SOUND_PLAYER_SPRAY);
                    m_numHolySprays--;
                    //run movement alg as normal
                    GRMovementAlg();
                    return;
                }
                
        break;
        case KEY_PRESS_UP:
                //increase speed of the ghost racer
                if (getVSpeed() < 5){
                    setVSpeed(getVSpeed() + 1);
                    GRMovementAlg();
                    return;
                }
        break;
            case KEY_PRESS_DOWN:
                //decrease speed of the ghostracer
                if(getVSpeed() > -1){
                    setVSpeed(getVSpeed() - 1);
                    GRMovementAlg();
                    return;
                }
        break;
    }
    }
    //even if the player inputted nothing, run movement algorithm so player moves
    GRMovementAlg();
}
    
void GhostRacer::GRMovementAlg(){
    //movement algorithm for ghost racer as per project spec
    double direction = getDirection();
    double dirInRad = direction * PI/180;
    double delta_x = cos(dirInRad) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
};

void GhostRacer::takeDamage(int amount){
    setHealth(getHealth() - amount);
    //take damage, if health less than or equal zero the player is dead
    if (getHealth() <= 0){
        getSW()->playSound(SOUND_PLAYER_DIE);
        setAlive(false);
    }
    
};

void GhostRacer::hitOilSlick(){
    //tells the ghost racer it hit an oilslick and to spin
    int curDir = getDirection();
    int adjust = randInt(5, 20);
    int chance = randInt(0, 1);
    if(chance == 0)
        setDirection(curDir - adjust);
    else
        setDirection(curDir + adjust);
    
    //limiters on direction from spin
    if (getDirection() < 60)
        setDirection(60);
    if(getDirection() > 120)
        setDirection(120);
}

void GhostRacer::heal(int amt){
    //heal the ghost racer up to 100 hitpoints
    setHealth(getHealth() + amt);
    if(getHealth() > 100)
        setHealth(100);
}


//END GHOSTRACER IMPLEMENTATIONS


//BORDERLINE IMPLEMENTATIONS
BorderLine::BorderLine(int imageID, int startX, int startY, StudentWorld* sw, GhostRacer* GR)
:Actor(imageID, startX, startY, 0, 2, 2, -4, 0, false, sw, GR)
{
};

void BorderLine::doSomething(){
    move();
    return;
};

BorderLine::~BorderLine(){
};



//OIL SLICK IMPLEMENTATIONS

OilSlick::OilSlick(int imageID, int startX, int startY, StudentWorld* sw, GhostRacer* GR)
:Actor(IID_OIL_SLICK, startX, startY, 0, randInt(2, 5), 2, -4, 0, false, sw, GR)
{
};

OilSlick::~OilSlick(){
};

void OilSlick::doSomething(){
    move();
    
    //if the move method determines it is off the screen
    //then it will set alive to false
    //if it is not alive then IMMEDIATELY return before
    //checking to interact with ghostracer
    //assumption from spec
    if(isAlive() == false)
        return;
    
    //if it collides with ghostracer, tell the ghost racer it hit oil slick
    if(collidesWith(*getGR())){
        getSW()->playSound(SOUND_OIL_SLICK);
        getGR()->hitOilSlick();
    }
    
};


//HEALING GOODIE IMPLEMENTATIONS

HealingGoodie::HealingGoodie(int imageID, int startX, int startY, StudentWorld* sw, GhostRacer* GR)
:Actor(IID_HEAL_GOODIE, startX, startY, 0, 1, 2, -4, 0, false, sw, GR)
{
};

HealingGoodie::~HealingGoodie(){
};

void HealingGoodie::doSomething(){
    //move down screen
    move();
    if(isAlive() == false)
        return;
    //if collides will ghostracer, heal it and remove the healing goodie
    if(collidesWith(*getGR())){
        getGR()->heal(10);
        setAlive(false);
        getSW()->playSound(SOUND_GOT_GOODIE);
        getSW()->increaseScore(250);
    }
}

//HOLY WATER GOODIE IMPLEMENTATIONS

HolyWaterGoodie::HolyWaterGoodie(int imageID, int startX, int startY, StudentWorld* sw, GhostRacer* GR)
:Actor(IID_HOLY_WATER_GOODIE, startX, startY, 90, 2, 2, -4, 0, false, sw, GR)
{
};

HolyWaterGoodie::~HolyWaterGoodie(){
};

void HolyWaterGoodie::doSomething(){
    //move the goodie and check off screen
    move();
    if(isAlive() == false)
        return;
    //if collides with ghost racer, increase holy water sprays
    if(collidesWith(*getGR())){
        getGR()->increaseHolyWaterGoodies(10);
        setAlive(false);
        getSW()->playSound(SOUND_GOT_GOODIE);
        getSW()->increaseScore(50);
    }
}


//LOST SOUL GOODIE IMPLEMENTATIONS

LostSoulGoodie::LostSoulGoodie(int imageID, int startX, int startY, StudentWorld* sw, GhostRacer* GR)
:Actor(IID_SOUL_GOODIE, startX, startY, 0, 4, 2, -4, 0, false, sw, GR)
{
};

LostSoulGoodie::~LostSoulGoodie(){
};

void LostSoulGoodie::doSomething(){
    //move the lost soul down the screen
    move();
    if(isAlive() == false)
        return;
    //if collides with ghost racer, save the soul
    if(collidesWith(*getGR())){
        getSW()->soulSaved();
        setAlive(false);
        getSW()->playSound(SOUND_GOT_SOUL);
        getSW()->increaseScore(100);
    }
    //rotate it
    setDirection(getDirection()-10);
}

//ALIVE ACTOR IMPLEMENTATIONS

AliveActor::AliveActor(int imageID, double startX, double startY, int startDirection, double size, unsigned int depth, double vspeed, double hspeed, bool cw, StudentWorld* sw, GhostRacer* GR, int hp)
:Actor(imageID, startX, startY, startDirection, size, depth, vspeed, hspeed, cw, sw, GR)
{
    m_hitpoints = hp;
}

AliveActor::~AliveActor(){
};

void AliveActor::takeDamage(int amount){
    //take damage, if health at or below 0 actor is dead
    setHealth(getHealth() - amount);
    if (getHealth() <= 0)
        setAlive(false);
    //generalised for human ped, will be redefined for child classes
    if (isAlive())
        getSW()->playSound(SOUND_PED_HURT);
    else
        getSW()->playSound(SOUND_PED_DIE);
}

//HUMAN PEDESTRIAN IMPLEMENTATION

HumanPedestrian::HumanPedestrian(int imageID, int dir, int startX, int startY, int size, StudentWorld* sw, GhostRacer* GR)
:AliveActor(imageID, startX, startY, dir, size, 0, -4, 0, true, sw, GR, 2)
{
    m_movementPlanDistance = 0;
}

HumanPedestrian::~HumanPedestrian(){
}

void HumanPedestrian::doSomething(){
    if (!isAlive())
        return;
    //end the level if ghost racer hits a human ped
    if(collidesWith(*getGR())){
        getGR()->setAlive(false);
    }
    
    //move and check if off screen, remove if off
    move();
    if (!isAlive())
        return;
    
    //decrement movement plan and pick a new one if the old one is done
    m_movementPlanDistance--;
    if (m_movementPlanDistance > 0)
        return;
    
    pickNewMovementPlan();
}

void HumanPedestrian::pickNewMovementPlan(){
    //pick new movement plan algorithm from project spec
    int randHSpeed = randInt(1, 3);
    int chanceNegative = randInt(0, 1);
    if (chanceNegative == 0)
        randHSpeed = -randHSpeed;
    setHSpeed(randHSpeed);
    
    m_movementPlanDistance = randInt(4, 32);
    
    if (randHSpeed < 0)
        setDirection(180);
    else
        setDirection(0);
}

//ZOMBIE PEDESTRIAN IMPLEMENTATION, CHILD OF HUMAN PED

ZombiePedestrian::ZombiePedestrian(int startX, int startY, StudentWorld* sw, GhostRacer* GR)
:HumanPedestrian(IID_ZOMBIE_PED, 0, startX, startY, 3, sw, GR)
{
    m_ticks = 0;
}

ZombiePedestrian::~ZombiePedestrian(){
}

void ZombiePedestrian::doSomething(){
    if (!isAlive())
        return;
    //if ghost racer hits, gr takes damage and zombie ped also takes damage
    if(collidesWith(*getGR())){
        getGR()->takeDamage(5);
        takeDamage(2);
        return;
    }
    
    //gets the x and y difference of the ghost racer and zombie ped
    int delta_x = abs(getX() - getGR()->getX());
    int delta_y = getY() - getGR()->getY();
    
    //if the zombie ped is directly above the ghostracer, turn towards it
    if ((delta_x <= 30) && (delta_y > 0)){
        setDirection(270);
        if ((getGR()->getX() - getX()) > 0)
            setHSpeed(1);
        else if (getGR()->getX() - getX() < 0)
            setHSpeed(-1);
        else
            setHSpeed(0);
        m_ticks--;
        if(m_ticks <= 0){
            getSW()->playSound(SOUND_ZOMBIE_ATTACK);
            m_ticks = 20;
        }
    }
    //move the zombie ped
    move();
    if (!isAlive())
        return;
    //decrement the movement plan or pick a new movement plan if last one is done
    if(getMovementPlanDistance() > 0){
        decMPD();
        return;
    }
    pickNewMovementPlan();
}

void ZombiePedestrian::takeDamage(int amount){
    //handles zombie ped taking damage
    setHealth(getHealth() - amount);
    if(getHealth() <= 0){
        setAlive(false);
        getSW()->playSound(SOUND_PED_DIE);
        //if the damage source to kill was not a collision from ghostracer
        //(if it was a holy water projectile to kill it)
        //then there is a 1 in 5 to spawn a healing goodie
        if (!collidesWith(*getGR())){
            int chance = randInt(1, 5);
            if (chance == 1){
                HealingGoodie* newHG = new HealingGoodie(IID_HEAL_GOODIE, getX(), getY(), getSW(), getGR());
                getSW()->addActor(newHG);
            }
        }
        //no matter the source of the kill increase the score
        getSW()->increaseScore(150);
    } else
        getSW()->playSound(SOUND_PED_HURT);
}


//ZOMBIE CAB IMPLEMENTATIONS

ZombieCab::ZombieCab(int startX, int startY, StudentWorld* sw, GhostRacer* GR, int VSpeed)
:HumanPedestrian(IID_ZOMBIE_CAB, 90, startX, startY, 4, sw, GR)
{
    setVSpeed(VSpeed);
    setHealth(3);
    damagedGR = false;
}

ZombieCab::~ZombieCab(){
}

void ZombieCab::doSomething(){
    if(!isAlive())
        return;
    //check collision with ghostracer
    if(collidesWith(*getGR())){
        if(!damagedGR){
            
            getSW()->playSound(SOUND_VEHICLE_CRASH);
            getGR()->takeDamage(20);
            int delta_x = getGR()->getX() - getX();
            //ping off the left or right depending on which side ghost racer hit it from
            if (delta_x >= 0){
                setHSpeed(-5);
                setDirection(120 + randInt(0, 19));
            } else{
                setHSpeed(5);
                setDirection(60 - randInt(0, 19));
            }
            //already damaged gr, prevents insta kills on player
            damagedGR = true;
        }
    }
    //move the zombie cab
    move();
    if (!isAlive())
        return;
    
    //determine the lane that the ghostracer is in
    int ZCLaneLeft;
    int ZCLaneRight;
    int distance = VIEW_HEIGHT;
    //calls private helper function to find the lane
    getLane(ZCLaneLeft, ZCLaneRight);
    Actor* closestActor;
    
    //if moving faster than the gr and there is an actor above within 96 units
    //decrease the speed of the zombie cab by 0.5
    if(getVSpeed() > getGR()->getVSpeed()){
        
        //uses student world helper function to find the closest actor between
        //given x values and above given y
        closestActor = getSW()->CWClosestActorBetweenAbove(ZCLaneLeft, ZCLaneRight, getY(), distance);
        
        if ((closestActor != nullptr) && (distance < 96)){
            setVSpeed(getVSpeed() - 0.5);
            return;
        }
    }
    
    //if moving slower or equal than the gr and there is an actor below within 96 units
    //increase the speed of the zombie cab by 0.5
    if(getVSpeed() <= getGR()->getVSpeed()){
        //uses student world helper function to find the closest actor between
        //given x values and above given y
        closestActor = getSW()->CWClosestActorBetweenBelow(ZCLaneLeft, ZCLaneRight, getY(), distance);
        
        if ((closestActor != nullptr) && (closestActor != getGR()) && (distance < 96)){
            setVSpeed(getVSpeed() + 0.5);
            return;
            
        }
    }
    
    //decrease movement plan length, return if plan not yet over
    decMPD();
    if (getMovementPlanDistance() > 0)
        return;
    
    //get a new movement plan distance and a new speed
    setMovementPlanDistance(randInt(4, 32));
    setVSpeed(getVSpeed() + randInt(-2, 2));
}

void ZombieCab::getLane(int& ZCLaneLeft, int& ZCLaneRight){
    //helper function that finds the x coords of the lane that the zombie cab is in
    //uses reference to return 2 values at once
    ZCLaneLeft = -1;
    ZCLaneRight = -1;
    
    //check the lane that the zombie cab is in based on x values
    if ((getX() >= LEFT_EDGE) && (getX() < leftMiddleX)){
        ZCLaneLeft = LEFT_EDGE;
        ZCLaneRight = leftMiddleX;
    } else if ((getX() >= leftMiddleX) && (getX() < rightMiddleX)){
        ZCLaneLeft = leftMiddleX;
        ZCLaneRight = rightMiddleX;
    } else if ((getX() >= rightMiddleX) && (getX() < RIGHT_EDGE)){
        ZCLaneLeft = rightMiddleX;
        ZCLaneRight = RIGHT_EDGE;
    }
}

void ZombieCab::takeDamage(int amount){
    //reduce health, if less than or equal to 0 then 1 in 5 chance to spawn oil slick at location
    setHealth(getHealth() - amount);
    if (getHealth() <= 0){
        setAlive(false);
        getSW()->playSound(SOUND_VEHICLE_DIE);
        int chance = randInt(1, 5);
        if (chance == 1){
            //spawn the oil slick at 20% chance
            OilSlick* newOS = new OilSlick(IID_OIL_SLICK, getX(), getY(), getSW(), getGR());
            getSW()->addActor(newOS);
        }
        //no matter if oil slick was spawned increase score
        getSW()->increaseScore(200);
        return;
    }
    //else the zombie cab didnt die so play vehicle hurt sound instead
    getSW()->playSound(SOUND_VEHICLE_HURT);
}

//HOLY WATER PROJECTILE IMPLEMENTATIONS

HolyWaterProjectile::HolyWaterProjectile(int imageID, int startX, int startY, int startDir, StudentWorld* sw, GhostRacer* GR)
:Actor(IID_HOLY_WATER_PROJECTILE, startX, startY, startDir, 1, 1, 0, 0, false, sw, GR)
{
    m_maxTravelDistance = 160;
}

HolyWaterProjectile::~HolyWaterProjectile(){
}

void HolyWaterProjectile::doSomething(){
    if(!isAlive())
        return;
    
    //check if the projectile hit an object
    Actor* actorHit = nullptr;
    //uses student world helper function to return a pointer to an actor if one was hit
    actorHit = getSW()->doHWPcollision(this);
    if (actorHit != nullptr){
        //if actor hit, deal damage and remove the projectile
        actorHit->takeDamage(1);
        setAlive(false);
        return;
    }
    
    //move the projectile and reduce travel distance left
    moveForward(SPRITE_HEIGHT);
    m_maxTravelDistance -= SPRITE_HEIGHT;
    
    checkOffScreen();
    
    //if it has gone max distance, it will be removed
    if (m_maxTravelDistance <= 0)
        setAlive(false);
}
