#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"

#include <cmath>
#include <string>

using namespace std;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

bool Actor::eatFood(int eatMax)
{
    Actor* foodptr = getWorld() -> isFoodHere(getX(), getY());  //Is this actor stnading on food?
    if (foodptr != nullptr) //If it's standing on food..
    {
        if( foodptr -> whatHP() > eatMax)   //and the there is more food than it can eat
        {
            adjustHitpoint(whatHP() + eatMax);  //Eat eatMax amount of food
            foodptr -> adjustHitpoint( (foodptr -> whatHP()) - eatMax); //Adjust HP of for remaining food
        }
        else
        {
            adjustHitpoint(whatHP() + (foodptr -> whatHP()) );  //Otherwise, eat all the food available
            foodptr->adjustState(dead); //Adjust the food to dead because its depleted
        }
        return true;    //Food was eaten, so grasshopper types might go to sleep
    }
    return false;
}

GraphObject::Direction Moving::randomDirection()
{
    int dir = randInt(0,3); //Choose random int to represent random direction
    switch (dir)
    {
        case 0:
            return GraphObject::up;
        case 1:
            return GraphObject::down;
        case 2:
            return GraphObject::left;
        case 3:
            return GraphObject::right;
    }
    return GraphObject::none;
}

void Moving::moveDirection(Direction dir, int x, int y)
{
    switch(dir) //Moves any actor capable of moving in direction dir
    {
        case up:
            if( y+1 > 0 && y+1 < 64 && x >= 0 && x<64)  //Checks if new location is within bounds
                moveTo(x,y+1);
            break;
        case down:
            if( y-1 >= 0 && y-1 < 63 && x >= 0 && x<64)
                moveTo(x, y-1);
            break;
        case left:
            if( x-1 >= 0 && x-1 < 64 && y >= 0 && y<64)
                moveTo(x-1, y);
            break;
        case right:
            if( x+1 > 0 && x+1 < 64 && y >= 0 && y<64)
                moveTo(x+1,y);
            break;
        case none:
            break;
    }
}

void Moving::attemptMove(Direction dir, int& x, int& y) //Adjusts original location x and y to potential new location copied into x and y, but doesn't actually move it yet
{
    switch(dir)
    {
        case up:
            if( y+1 > 0 && y+1 < 64 && x >= 0 && x<64 ) //Checks if new location is withiin bounds
                y = y + 1;
            break;
        case down:
            if( y-1 >= 0 && y-1 < 63 && x >= 0 && x<64 )
                y = y-1;
            break;
        case left:
            if( x-1 >= 0 && x-1 < 64 && y >= 0 && y<64)
                x = x-1;
            break;
        case right:
            if( x+1 > 0 && x+1 < 64 && y >= 0 && y<64 )
                x = x+1;
            break;
        case none:
            break;
    }
}

bool Moving::blockMovement(int x, int y) const //See if attempted move would be blocked by a pebble
{
    Actor* ap = getWorld()->whoThere(x,y);    //Returns pointer to potential pebble at location
    if (ap != nullptr)  //If something is there
        return ap->blockAnt();  //Return whether that thing is capable of blocking movement
    
    return false;   //Otherwise, nothing there to block
}

bool Moving::checkHealth()
{
    adjustHitpoint(whatHP() - 1);   //Lose one HP per movement
    
    if( whatHP() <= 0 ) //If HP is less than 0, it's dead
    {
        adjustState(dead);
        return false;
    }
    
    if (whatParal() > 0)    //Paralyzed
    {
        adjustParalysis(whatParal() - 1);   //Wait another tick
        return false;
    }
    
    return true;    //Is not dead or paralyzed so, can do something meaningful
}

void Moving::randomBite(Actor* biting, int damage)
{
    Actor* bittenInsect = getWorld() -> chooseRandomBite(biting); //Choose random insect for biting to bite
    if (bittenInsect != nullptr)    //If there is another insect to bite
    {
        bittenInsect->adjustHitpoint(bittenInsect->whatHP() - damage);  //Adjust HP of bitten insect
        biting -> adjustParalysis(biting->whatParal() + 2); //Biting insect is paralyzed
        if( bittenInsect -> whatColony() >= 0 && bittenInsect->whatColony() <4) //If bitten insect is an ant
        {
            bittenInsect->adjustBitten(true);   //Adjust its status to bitten
        }
        if(bittenInsect-> whatHP() <= 0)    //if insect is dead now
        {
            bittenInsect->adjustState(dead);
            return;
        }
        else if( !(bittenInsect->canStunPoison()) && randInt(1, 2) == 1)    //If bitten insect is adult grasshopper, there is a 1 in 2 damage of it also doing a random bite
        {
            randomBite(bittenInsect, 50);   //Bitten insect does random bite too
        }
    }
}

void Anthill::giveBirth()   //Anthill gives birth depending on which colony belongs to
{
    if(whatColony() == 0)
    {
        Actor* babyAnt = new Ant(IID_ANT_TYPE0, getX(), getY(), 0, getWorld(), m_instr);
        getWorld()->add2Tracker(babyAnt, getX(), getY());
    }
    else if(whatColony() == 1)
    {
        Actor* babyAnt = new Ant(IID_ANT_TYPE1, getX(), getY(), 1, getWorld(), m_instr);
        getWorld()->add2Tracker(babyAnt, getX(), getY());
    }
    else if(whatColony() == 2)
    {
        Actor* babyAnt = new Ant(IID_ANT_TYPE2, getX(), getY(), 2, getWorld(), m_instr);
        getWorld()->add2Tracker(babyAnt, getX(), getY());
    }
    else if(whatColony() == 3)
    {
        Actor* babyAnt = new Ant(IID_ANT_TYPE3, getX(), getY(), 3, getWorld(), m_instr);
        getWorld()->add2Tracker(babyAnt, getX(), getY());
    }
}


void Anthill::doSomething()
{
    adjustHitpoint(whatHP() - 1);
    if(whatHP() == 0)
    {
        adjustState(dead);
        return;
    }
    eatFood(10000); //Eats up to 10,000HP of food dropped on its location
    if(whatHP() >= 2000)    //If enough HP
    {
        giveBirth();    //Make another ant
        adjustHitpoint(whatHP()-1500);  //Adjust own HP
        getWorld()->adjustAntCount(whatColony(), 1);    //Adjust total any count
    }
}

void WaterPool::doSomething()
{
    getWorld() -> stunAll(getX(),getY());
}

void Poison::doSomething()
{
    getWorld()->poisonAll(getX(), getY());
}

bool Ant::whereEnemy(int x, int y) 
{
    if (getWorld()->findEnemy(getX(), getY(),whatColony()))
        return true;
    return false;
}

void Ant::runCommand(const Compiler::Command& c)
{
    if(c.opcode == Compiler::moveForward)   //Walk forward in direction
    {
        int desX = getX(), desY = getY();
        attemptMove(getDirection(), desX, desY);    //Attempt move
        if (blockMovement(desX, desY))  //If move blocked
            m_blocked = true;   //Ant was blocked
        else
        {
            moveDirection(getDirection(), getX(), getY());  //Otherwise, move it
            m_blocked = false;
            adjustBitten(false);
            adjustMoved(true);
            adjustState(healthy);   //Moved away, so can be stunned
        }
        m_change = true;
    }
    else if( c.opcode == Compiler::eatFood) //Eats heldFood
    {
        if(heldFood > 100)  //Holding more than 100 food
        {
            adjustHitpoint(whatHP() + 100); //Eat 100
            heldFood -= 100;    //Deplete held food by 100
        }
        else
        {
            adjustHitpoint(whatHP() + heldFood);    //Otherwise, eat remaining
            heldFood = 0;   //Then has no food
        }
        m_change = true;
    }
    else if(c.opcode == Compiler::dropFood) //Drops all held food
    {
        getWorld()->addFood(getX(), getY(),heldFood);   //Add food to spot
        heldFood = 0;   //No food left
        m_change = true;
    }
    else if (c.opcode == Compiler::bite)    //Randommly bite enemy ants or grasshoppers
    {
        randomBite(this, 15);
        m_change = true;
    }
    else if(c.opcode == Compiler::pickupFood)   //Pick up at most 400 food from location to hold at most 1800
    {
        Actor* foodptr = getWorld() -> isFoodHere(getX(), getY());
        if( foodptr != nullptr) //If there is food here
        {
            if (foodptr -> whatHP() > 400)  //and its more than 400 food
            {
                if(heldFood < 1400) //and you can take 400 food
                {
                    heldFood += 400;    //take it
                    foodptr -> adjustHitpoint(whatHP() - 400);  //adjust food hp left
                }
                else    //would max out your 1800 held food if you took 400
                {
                    foodptr -> adjustHitpoint(whatHP() - (1800 - heldFood));    //take up to max heldfood
                    heldFood = 1800;    //have max amount of food
                }
            }
            else    //400 or less food here
            {
                if(heldFood + foodptr->whatHP() <= 1800)    //can take all the food there
                {
                    heldFood += foodptr->whatHP();
                    foodptr->adjustState(dead);
                    return;
                }
                else    //would max out your 1800 held food if you took it all
                {
                    foodptr -> adjustHitpoint(whatHP() - (1800 - heldFood));
                    heldFood = 1800;
                }
            }
        }
        m_change = true;
    }
    else if(c.opcode == Compiler::faceRandomDirection)  //random direction to facce
    {
        setDirection(randomDirection());
        m_change = true;
    }
    else if (c.opcode == Compiler::rotateClockwise)
    {
        switch(getDirection())
        {
            case up:
                setDirection(right);
                break;
            case down:
                setDirection(left);
                break;
            case right:
                setDirection(down);
                break;
            case left:
                setDirection(up);
                break;
            case none:
                break;
        }
        m_change = true;
    }
    else if (c.opcode == Compiler::rotateCounterClockwise)
    {
        switch(getDirection())
        {
            case up:
                setDirection(left);
                break;
            case down:
                setDirection(right);
                break;
            case right:
                setDirection(up);
                break;
            case left:
                setDirection(down);
                break;
            case none:
                break;
        }
        m_change = true;
    }
    else if(c.opcode == Compiler::emitPheromone)    //Add pheromone to spot
    {
        Actor* smellptr = getWorld()->isSmelly(getX(), getY(), whatColony());
        if(smellptr != nullptr && smellptr -> whatHP() > 512)   //somethign there and adding 256 would max it out
            getWorld()->addPheromone(getX(), getY(), 768 - (smellptr->whatHP()), whatColony());
        else    //something there, but adding 256 wouldn't max it out OR nothing there
            getWorld()->addPheromone(getX(), getY(), 256, whatColony());
        m_change = true;
    }
    else if(c.opcode == Compiler::generateRandomNumber) //Random number
        lastRand = randInt(0,stoi(c.operand1) - 1);
    else if (c.opcode == Compiler::goto_command)
        m_ic = stoi(c.operand1);
    else if (c.opcode == Compiler::if_command)
    {
        if( stoi(c.operand1) == 9 && lastRand == 0)
            m_ic = stoi(c.operand2);
        else if( stoi(c.operand1) == 3 && heldFood > 0)
            m_ic = stoi(c.operand2);
        else if( stoi(c.operand1) == 4 && whatHP() <= 25)
            m_ic = stoi(c.operand2);
        else if( stoi(c.operand1) == 7 && whereEnemy(getX(), getY()))
            m_ic = stoi(c.operand2);
        else if (stoi(c.operand1) == 6 && getWorld()->isFoodHere(getX(), getY()) != nullptr)
            m_ic = stoi(c.operand2);
        else if (stoi(c.operand1) == 5 && !(getWorld()->whoThere(getX(), getY())->canMove()) && getWorld()->whoThere(getX(), getY())->whatColony() == whatColony() && getWorld()->whoThere(getX(), getY())->getState() != dead )    //standing on its anthill
            m_ic = stoi(c.operand2);
        else if(stoi(c.operand1) == 1)
        {
            int checkX=getX(),checkY=getY(), image;
            switch (whatColony())
            {
                case 0:
                    image = IID_PHEROMONE_TYPE0;
                    break;
                case 1:
                    image = IID_PHEROMONE_TYPE1;
                    break;
                case 2:
                    image = IID_PHEROMONE_TYPE2;
                    break;
                case 3:
                    image = IID_PHEROMONE_TYPE3;
                    break;
                default:
                    break;
            }
            attemptMove(getDirection(), checkX, checkY);
            Actor* smellPTR = getWorld()->isSmelly(checkX, checkY,whatColony());
            if (!blockMovement(checkX, checkY) && smellPTR != nullptr)
                m_ic = stoi(c.operand2);
        }
        else if(stoi(c.operand1) == 0)
        {
            int checkX=getX(),checkY=getY();
            attemptMove(getDirection(), checkX, checkY);
            if (whereEnemy(checkX, checkY) || getWorld()->isPoison(checkX, checkY))
                m_ic = stoi(c.operand2);
        }
        else if (stoi(c.operand1) == 2 && isBit())
            m_ic = stoi(c.operand2);
        else if (stoi(c.operand1) == 8 && m_blocked == true)
            m_ic = stoi(c.operand2);
    }
}

void Ant:: doSomething()
{
    if(!checkHealth())  //If you returned false on your health (means you are paralyzed or dead)
        return; //Do not do anything meaningful and just return
    
    Compiler::Command c;
    
    for( int i = 0; i <10; i++) //Does 10 commands per tick
    {
        if(! (m_instr -> getCommand(m_ic, c)) ) //Cannot get command, so dead
        {
            adjustState(dead);
            return;
        }
        int old_ic = m_ic;  //Temp holds the old instruction counter (ic) number
        runCommand(c);  //Runs command c
        if (m_ic == old_ic) //If its the same ic, then just increment it
            m_ic++;
        if(m_change == true)    //If you changed the status of the simulation
        {
            m_change = false;   //Start over the bool function and return
            return;
        }
    }
    return;
}

void Grasshopper::grassWalk()
{
    if (getDistance() == 0)    //Finished walking in one direction
    {
        setDirection(randomDirection());    //New random direction
        setRandomDistance();    //New random distance
    }
    
    int desX = getX(), desY = getY();
    attemptMove(getDirection(), desX, desY);    //Try to move in direction dir and copies potential location into desX, desY
    if (blockMovement(desX,desY))   //If that new destination is blocked
        adjustDistance(0);  //Stay in original location and adjust dist to move in new direction/distance
    else
    {
        moveDirection(getDirection(), getX(), getY());  //Not blocked, so actually move actor
        adjustDistance(getDistance() - 1);  //Move one, so distance decreased
        adjustState(healthy);   //If was on a pool, moved away so now it can be stunned again
        adjustMoved(true);  //Already moved, so can't be told to do something again
    }
    adjustParalysis(whatParal() + 2);   //Can't do something meaningful
}

void Grasshopper::doSomething()
{
    if (!checkHealth()) //If you returned false on your health (means you are paralyzed or dead)
        return; //do nothing else
    
    doDiffStuff();
    
    if (eatFood(200) && goesToSleep())  //Eat food, and if you go to sleep by chance
    {
        adjustParalysis(whatParal() + 2);   //Goes to sleep and does nothign else
        return;
    }
    
    grassWalk(); //Makes a meaningful walk if doesn't do anything else otherwise
}

void AdultGrasshopper::doDiffStuff()
{
    if (randInt(1,10) == 1) //1 out of 10 chance of jumping into radius 10
    {
        int flag = 0, newX, newY, r, randAng, oldX = getX(), oldY = getY();
        
        while (flag == 0)
        {
            r = randInt(1, 10); //Random radius to jump within 10
            randAng = randInt(-180,180);    //Random angle to jump at
            newX = getX() + r * sin(randAng * 3.14159265 / 180);    //New X location based on adding x-component of those random radius and angle to old X
            newY = getY() + r * cos(randAng * 3.14159265 / 180);    //New Y location based on adding y-component of those random radius and angle to old X
            
            if (newX >= 0 && newX <64 && newY >=0 && newY <64)  //If new location within bounds
                flag = 1;   //Exit loop, otherwise find new location
        }
        
        moveTo(newX, newY); //Move to new location
        if (blockMovement(getX(),getY()))   //If blocked, move bakc
            moveTo(oldX, oldY);
        else
        {
            adjustParalysis(whatParal() + 2);   //Made meaning jump, so go to sleep
            adjustMoved(true);  //Moved, so cannot be moved again in one tick
            return; //Do nothing else
        }
    }
}

void BabyGrasshopper::doDiffStuff()
{
    if (whatHP() >= 1600)   //Dies and turns into adult
    {
        adjustState(dead);
        Actor* makeAdult = new AdultGrasshopper(getX(),getY(), getWorld());
        getWorld() -> add2Tracker(makeAdult, getX(), getY());
        return;
    }
}