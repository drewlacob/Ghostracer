#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;
class GhostRacer;

class Actor: public GraphObject{
    public:
        Actor(int imageID, double startX, double startY, int startDirection, double size, unsigned int depth, double vspeed, double hspeed, bool cw, StudentWorld* sw, GhostRacer* GR);
        virtual ~Actor();
        virtual void doSomething() = 0;
    
        //general functions used by actors that do not get redefined
        //also used by student world helper functions
        void move();
        void checkOffScreen();
        bool collidesWith(Actor& other);
    
        //accessors
        bool isAlive(){ return m_alive;}
        bool isCollisionWorthy(){ return m_collisionWorthy;}
        int getVSpeed() {return m_verticalSpeed;}
        int getHSpeed(){return m_horizontalSpeed;}
        StudentWorld* getSW(){return m_sw;}
        GhostRacer* getGR(){return m_GRptr;}
    
        //mutators
        void setAlive(bool alive){m_alive = alive;}
        void setVSpeed(double vspeed) {m_verticalSpeed = vspeed;}
        void setHSpeed(double hSpeed){m_horizontalSpeed = hSpeed;}
        virtual void takeDamage(int amount){return;};
    
    private:
        bool m_alive;
        bool m_collisionWorthy;
        double m_verticalSpeed;
        double m_horizontalSpeed;
        StudentWorld* m_sw;
        GhostRacer* m_GRptr;
};

//ALIVE ACTOR AND CHILD DECLARATIONS
class AliveActor: public Actor{
    public:
        AliveActor(int imageID, double startX, double startY, int startDirection, double size, unsigned int depth, double vspeed, double hspeed, bool cw, StudentWorld* sw, GhostRacer* GR, int hp);
        virtual ~AliveActor();
        virtual void doSomething() = 0;
        //accessors
        int getHealth() {return m_hitpoints;}
        //mutators
        void setHealth(int amt){m_hitpoints = amt;}
        virtual void takeDamage(int amount);
    
    private:
        int m_hitpoints;
};

class GhostRacer: public AliveActor{
    public:
        GhostRacer(StudentWorld* sw);
        virtual ~GhostRacer();
        virtual void doSomething();
        //mutators
        void takeDamage(int amount);
        void hitOilSlick();
        void heal(int amt);
        void increaseHolyWaterGoodies(int amt){m_numHolySprays += 10;}
        //accessors
        int getSprays() {return m_numHolySprays;}
    
    private:
        void GRMovementAlg();
    
        int m_numHolySprays;
};

class HumanPedestrian: public AliveActor{
    public:
        HumanPedestrian(int imageID, int dir, int startX, int startY, int size, StudentWorld* sw, GhostRacer* GR);
        virtual ~HumanPedestrian();
        virtual void doSomething();
        //mutators
        virtual void pickNewMovementPlan();
        virtual void decMPD(){m_movementPlanDistance--;}
        virtual void setMovementPlanDistance(int d){m_movementPlanDistance = d;}
        //accessors
        virtual int getMovementPlanDistance(){return m_movementPlanDistance;}
    private:
        int m_movementPlanDistance;
};

class ZombiePedestrian: public HumanPedestrian{
    public:
        ZombiePedestrian(int startX, int startY, StudentWorld* sw, GhostRacer* GR);
        virtual ~ZombiePedestrian();
        virtual void doSomething();
        virtual void takeDamage(int amount);
    private:
        int m_ticks;
};

class ZombieCab: public HumanPedestrian{
    public:
        ZombieCab(int startX, int startY, StudentWorld* sw, GhostRacer* GR, int VSpeed);
        virtual ~ZombieCab();
        virtual void doSomething();
        virtual void takeDamage(int amount);
    private:
        virtual void getLane(int& ZCLeftLane, int& ZCRightLane);
        bool damagedGR;
};

//BORDERLINE

class BorderLine: public Actor{
    public:
        BorderLine(int imageID, int startX, int startY, StudentWorld* sw, GhostRacer* GR);
        virtual ~BorderLine();
        virtual void doSomething();
};


//GOODIE TYPE DECLARATIONS

class OilSlick: public Actor{
    public:
        OilSlick(int imageID, int startX, int startY, StudentWorld* sw, GhostRacer* GR);
        virtual ~OilSlick();
        virtual void doSomething();
};

class HealingGoodie: public Actor{
    public:
        HealingGoodie(int imageID, int startX, int startY, StudentWorld* sw, GhostRacer* GR);
        virtual ~HealingGoodie();
        virtual void doSomething();
};

class HolyWaterGoodie: public Actor{
    public:
        HolyWaterGoodie(int imageID, int startX, int startY, StudentWorld* sw, GhostRacer* GR);
        virtual ~HolyWaterGoodie();
        virtual void doSomething();
};

class LostSoulGoodie: public Actor{
    public:
        LostSoulGoodie(int imageID, int startX, int startY, StudentWorld* sw, GhostRacer* GR);
        virtual ~LostSoulGoodie();
        virtual void doSomething();
};

//Holy Water Projectile Declarations

class HolyWaterProjectile: public Actor{
    public:
        HolyWaterProjectile(int imageID, int startX, int startY, int startDir, StudentWorld* sw, GhostRacer* GR);
        virtual ~HolyWaterProjectile();
        virtual void doSomething();
    private:
        int m_maxTravelDistance;
};

//CONSTANTS

const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
const int leftMiddleX = LEFT_EDGE + ROAD_WIDTH/3;
const int rightMiddleX = RIGHT_EDGE - ROAD_WIDTH/3;
const double PI = 3.14159265;
const double max_shift_per_tick = 4.0;
const int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;














#endif // ACTOR_H_
