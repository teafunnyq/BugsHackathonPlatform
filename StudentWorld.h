#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Field.h"  //Included to load field files
#include "Actor.h"

#include <list> //To create set to hold actors
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir), m_tick(0) {}
    
    ~StudentWorld()    //Destructor to free any remaining things
    {
        cleanUp();
    }

    virtual int init();
    
    virtual int move();

    virtual void cleanUp();
    
    Actor* whoThere(int x, int y){return trackActor[x][y].front();}  //What actor is at front of (x,y) linked list. Used for if pebble is blocking way since it'll be first in linked list if there
    
    void add2Tracker(Actor* add, int x,int y) {trackActor[x][y].push_back(add);}    //Adds to tracker array
    
    bool LoadField();   //Loads field
    void updateStructure(Actor* it);    //Updates the 2D array with new location
    void removeDead();  //Removes all the dead actors and add food in its place if necessary
    Actor* isFoodHere(int x, int y);    //Checks if any food is there
    void addFood(int x, int y,int amount);  //Adds amount of food to (x,y)
    void addPheromone(int x, int y, int amount, int colony);    //Adds amount of phermone from colony to (x,y)
    Actor* isSmelly(int x, int y, int colony);  //Is there a pheromone from colony already here?
    bool isPoison(int x,int y); //Is there poison on this spot
    void adjustAntCount(int colony, int adj);    //Adjust ant count of colony by adj
    void setDisplayText();  //Display text for ticks at top of screen
    int getWinningAnt();    //Determines winning ant per round for tick display
    bool isThereWinner(int& winningColony); //Is there an overall winner from the game
    
    void stunAll(int x, int y);   //Stuns all insects at (x,y)
    void poisonAll(int x, int y); //Poison all insects at (x,y)
    bool findEnemy(int x, int y, int colony);   //Returns true if there are enemies at the location
    
    Actor* chooseRandomBite(Actor* bitingOne);  //Choose random insect to bite
    
private:
    int m_tick;
    Field m_f;
    std::list<Actor*> trackActor[64][64];  //2D Array with linked list at y,x
    std::vector<std::string> names;
    std::vector<int> antCount;
    int m_curNum[4] = {0,0,0,0};
};

#endif // STUDENTWORLD_H_
