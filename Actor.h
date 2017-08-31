#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include "Compiler.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor: public GraphObject //Base class for all simulation actors
{
public:
    enum State{
        healthy,dead,stun
    };
    
    Actor(int imageID,int startX, int startY, StudentWorld* world, int colony, Direction startDirection = right, int depth = 0, double size = 0.25)   //Simple constructor
    : GraphObject(imageID, startX, startY, startDirection, depth), m_world(world),m_moved(false), m_paralyze(0), m_status(healthy),m_image(imageID), m_colony(colony) {}
    
    virtual ~Actor() {}
    
    virtual void doSomething() = 0; //Has each actor complete an action
    virtual bool blockAnt() = 0;    //Returns true if an actor will block ant
    virtual bool canStunPoison() = 0;   //Return true if it is capable of being stunned or poisoned
    virtual bool canMove() = 0; //Checks if actor is capable of moving spaces
    virtual bool canSmell() = 0;    //Check if it is of a pheromone Actor
    
    int whatColony() const {return m_colony;}   //What colony does this relate to (anthill, ants, pheromones)
    
    void adjustBitten(bool ins) //Has the actor been bitten before?
    {
        m_bit = ins;
    }
    
    int whoAmI() const {return m_image;}  //Who is this actor based on their image
    
    StudentWorld* getWorld() const {return m_world;}    //StudentWorld each actor linked to

    void adjustHitpoint(int hp) //Change HP
    {
        m_hitpoint = hp;
    }
    
    int whatHP() const {return m_hitpoint;}    //Returns hitpoints of actor

    bool movedThisTime() const {return m_moved;}    //Returns whether an actor already did something during a turn
    
    void adjustMoved(bool ans)  //Change whether an actor has done something during a tick
    {
        m_moved = ans;
    }
    
    void adjustParalysis(int ins)   //Adjusts how long it's stunned
    {
        m_paralyze = ins;
    }
    
    int whatParal() const {return m_paralyze;} //How much longer it is stunned?
    
    void adjustState(State ins) //Changes state it is in
    {
        m_status = ins;
    }
    
    State getState() const {return m_status;}    //What state is it in?
    
    bool isBit() {return m_bit;}    //Has it been bitten before? ants MOVE TO MOVING???
    
    bool eatFood(int eatMax);   //Actor will eat a maximum of eatMax food
    
private:
    int m_hitpoint, m_paralyze, m_image, m_colony;
    StudentWorld* m_world;
    bool m_moved, m_bit;
    State m_status;
};

class Moving:public Actor   //Insect class
{
public:
    Moving(int imageID,int startX, int startY, StudentWorld* world, int colony, int depth = 0, Direction startDirection = right)
    :Actor(imageID,startX,startY,world,colony,startDirection, depth) {}
    
    virtual ~Moving() {}
    
    virtual void doSomething() = 0;
    
    virtual bool blockAnt() {return false;}
    
    virtual bool canStunPoison() {return true;}
    
    virtual bool canMove(){return true;}
    
    virtual bool canSmell(){return false;}
    
    GraphObject::Direction randomDirection();   //Generates random direction to head in
    void moveDirection(Direction dir, int x, int y);    //Moves actor in direction dir
    bool blockMovement(int x, int y)  const;   //Check if movement to (x,y) is blocked
    void randomBite(Actor* bittenInsect, int damage);   //Randomly bite another insect
    bool checkHealth(); //Check on HP and paralysis in beginning for each moving character
    void attemptMove(Direction dir, int& x, int& y);    //Adjusts x and y to potentially move in dir
};

class Stationary:public Actor
{
public:
    Stationary(int imageID,int startX, int startY, StudentWorld* world,int depth = 2, int colony = 4,Direction startDirection = right)
    :Actor(imageID,startX,startY,world, colony, startDirection,depth) {}
    
    virtual ~Stationary() {}
    
    virtual void doSomething() = 0;
    
    virtual bool blockAnt() {return false;}
    
    virtual bool canStunPoison()  {return false;}
    
    virtual bool canMove() {return false;}
    
    virtual bool canSmell()  {return false;}
};

class Pebble:public Stationary
{
public:
    Pebble(int startX, int startY, StudentWorld* world)
    : Stationary(IID_ROCK, startX, startY,world,1) {}
    
    virtual ~Pebble() {}
    
    virtual void doSomething() {}
    
    virtual bool blockAnt()  {return true;}
};

class Food:public Stationary
{
public:
    Food(int startX,int startY, StudentWorld* world, int hitpoint = 6000)
    :Stationary(IID_FOOD, startX, startY, world)
    {
        adjustHitpoint(hitpoint);
    }
    
    virtual ~Food() {}
    
    virtual void doSomething() {}
};

class Pheromone:public Stationary
{
public:
    Pheromone(int imageID, int startX, int startY, StudentWorld* world, int colony)
    :Stationary(imageID, startX, startY, world,2, colony)
    {
        adjustHitpoint(256);
    }
    
    virtual ~Pheromone() {}
    
    virtual void doSomething()
    {
        adjustHitpoint(whatHP() - 1);
        if(whatHP() == 0)
            adjustState(dead);
    }
    
    virtual bool canSmell() {return true;}
};

class WaterPool:public Stationary
{
public:
    WaterPool(int startX, int startY, StudentWorld* world)
    :Stationary(IID_WATER_POOL, startX, startY, world) {}
    
    virtual ~WaterPool() {}
    
    virtual void doSomething();
};

class Poison:public Stationary
{
public:
    Poison(int startX, int startY, StudentWorld* world)
    :Stationary(IID_POISON, startX, startY, world) {}
    
    virtual ~Poison() {}
    
    virtual void doSomething();
};


class Anthill:public Stationary
{
public:
    Anthill(int startX, int startY, StudentWorld* world,int colony, Compiler* instr)
    :Stationary(IID_ANT_HILL, startX, startY, world, 2, colony), m_instr(instr)
    {
        adjustHitpoint(8999);
    }
    
    virtual ~Anthill() {}
    
    virtual void doSomething();
    
    void giveBirth();
    
private:
    int m_ants;
    Compiler* m_instr;
};

class Ant:public Moving
{
public:
    Ant(int imageID, int startX, int startY, int colony, StudentWorld* world, Compiler* instr)
    :Moving(imageID, startX, startY, world, colony, 1), m_instr(instr), m_change(false), m_ic(0), m_stun(false), m_blocked(false), lastRand(0), heldFood(0)
    {
        adjustHitpoint(1500);
        setDirection(randomDirection());
    }
    
    virtual ~Ant() {}
    
    virtual void doSomething();
    
    void runCommand(const Compiler::Command& c);    //Run commands given by .bug file
    
    bool whereEnemy(int x, int y);  //Checks if an ant's enemy is at location (x,y)

private:
    int m_ic, lastRand, heldFood;
    Compiler* m_instr;
    bool m_stun, m_blocked, m_change;
};

class Grasshopper:public Moving
{
public:
    Grasshopper(int imageID, int startX, int startY, StudentWorld* world)
    :Moving(imageID, startX, startY, world, 4)
    {
        setDirection(randomDirection());
        setRandomDistance();
    }
    
    virtual ~Grasshopper() {}
    
    virtual void doDiffStuff() = 0;
    
    virtual void doSomething();
    
    virtual bool canStunPoison()  {return false;}
    
    void setRandomDistance()    //Random distance that grasshopper types move
    {
        m_distance = randInt(2, 10);
    }
    
    int getDistance() const {return m_distance;}   //Distance grasshopper types have to go
    
    void adjustDistance(int dis)    //Change distance that grasshopper types have to go
    {
        m_distance = dis;
    }
    
    bool goesToSleep()
    {
        if( randInt(1, 10) <= 5)
            return true;
        return false;
    }
    
    void grassWalk();   //Walks
    
private:
    int m_distance;
};

class AdultGrasshopper:public Grasshopper
{
public:
    AdultGrasshopper(int startX, int startY, StudentWorld* world)
    :Grasshopper(IID_ADULT_GRASSHOPPER, startX, startY, world)
    {
        adjustHitpoint(1600);
    }
    
    virtual void doDiffStuff();
};

class BabyGrasshopper:public Grasshopper
{
public:
    BabyGrasshopper(int startX, int startY, StudentWorld* world)
    :Grasshopper(IID_BABY_GRASSHOPPER, startX, startY, world)
    {
        adjustHitpoint(500);
    }
    
    virtual ~BabyGrasshopper() {}
    
    virtual void doDiffStuff();
    
    virtual bool canStunPoison() {return true;}
};
#endif // ACTOR_H_
