#include "StudentWorld.h"
#include "Field.h"
#include "Compiler.h"

#include <list>
#include <string>
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

int StudentWorld::init()
{
    m_tick = 0; //Set ticks to 0
    
    if (!LoadField())   //loads field, if fails returns
        return GWSTATUS_LEVEL_ERROR;
    
    Compiler *compiler0 = nullptr, *compiler1 = nullptr, *compiler2 = nullptr, *compiler3 = nullptr;
    Anthill *ah0, *ah1, *ah2, *ah3; //SHOULD I MAKE THIS POINTER TO ACTOR
    
    vector<string> fileNames = getFilenamesOfAntPrograms(); //Gets all files of ant programs
    
    compiler0 = new Compiler; compiler1 = new Compiler; compiler2 = new Compiler; compiler3 = new Compiler;
    string error;
    
    if ( 0 < fileNames.size()  &&  ! compiler0->compile(fileNames[0], error) )  //Compiles each one and checks for error
    {
        setError(fileNames[0] + " " + error);
        return GWSTATUS_LEVEL_ERROR;
    }
    if ( 1 < fileNames.size() && ! compiler1->compile(fileNames[1], error) )
    {
        setError(fileNames[1] + " " + error);
        return GWSTATUS_LEVEL_ERROR;
    }
    if ( 2 < fileNames.size() && ! compiler2->compile(fileNames[2], error) )
    {
        setError(fileNames[2] + " " + error);
        return GWSTATUS_LEVEL_ERROR;
    }
    if ( 3 < fileNames.size() && ! compiler3->compile(fileNames[3], error) )
    {
        setError(fileNames[3] + " " + error);
        return GWSTATUS_LEVEL_ERROR;
    }
    
    names.push_back(compiler0->getColonyName());    //Gets colony name of each file
    antCount.push_back(0);  //Push's back current number of ants for each colon, which is 0
    names.push_back(compiler1->getColonyName());
    antCount.push_back(0);
    names.push_back(compiler2->getColonyName());
    antCount.push_back(0);
    names.push_back(compiler3->getColonyName());
    antCount.push_back(0);
    
    //Go through each point in field and allocate object for it
    for (int r = 0; r < 64; r++)
    {
        for (int c = 0; c < 64; c++)
        {
            Field::FieldItem item = m_f.getContentsOf(c, r);    //Gets field contacts at c,r
            Actor* temp = nullptr;
            switch (item)
            {
                case Field::FieldItem::anthill0:
                    if(compiler0->getColonyName() != "--------")    //If there is no file for this colony, then it will return "-------", so don't even allocate an anthill for it
                    {
                        ah0 = new Anthill(c, r, this, 0, compiler0);
                        trackActor[c][r].push_back(ah0);
                    }
                    break;
                case Field::FieldItem::anthill1:
                    if (compiler1->getColonyName() != "--------")
                    {
                        ah1 = new Anthill(c, r, this, 1, compiler1);
                        trackActor[c][r].push_back(ah1);
                    }
                    break;
                case Field::FieldItem::anthill2:
                    if (compiler2->getColonyName() != "--------")
                    {
                        ah2 = new Anthill(c, r, this, 2, compiler2);
                        trackActor[c][r].push_back(ah2);
                    }
                    break;
                case Field::FieldItem::anthill3:
                    if (compiler3->getColonyName() != "--------")
                    {
                        ah3 = new Anthill(c, r, this, 3, compiler3);
                        trackActor[c][r].push_back(ah3);
                    }
                    break;
                case Field::FieldItem::empty:
                    break;
                case Field::FieldItem::poison:
                    temp = new Poison(c,r,this);
                    trackActor[c][r].push_back(temp);
                    break;
                case Field::FieldItem::food:
                    temp = new Food(c,r,this);
                    trackActor[c][r].push_back(temp);
                    break;
                case Field::FieldItem::water:
                    temp = new WaterPool(c,r,this);
                    trackActor[c][r].push_back(temp);
                    break;
                case Field::FieldItem::rock:
                    temp = new Pebble(c,r,this);
                    trackActor[c][r].push_back(temp);
                    break;
                case Field::FieldItem::grasshopper:
                    temp = new BabyGrasshopper(c,r,this);
                    trackActor[c][r].push_back(temp);
                    break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld:: move()
{
    m_tick++;   //Update tick count
    
    for (int r = 0; r < 64; r++)    //Moves each character that can move positions first
    {
        for (int c = 0; c < 64; c++)
        {
            list<Actor*>::iterator it = trackActor[c][r].begin();
            while (it != trackActor[c][r].end())
            {
                if( ! ((*it)->movedThisTime()) && (*it)->canMove()) //Hasn't moved yet and it can move
                {
                    int oldX = (*it)->getX(), oldY = (*it)->getY();
                    if ( (*it) -> getState() != Actor::dead)    //Only if not dead do something
                        (*it)->doSomething();
                    if ( (*it)->getX() != oldX || (*it)->getY() != oldY)    //If you moved
                    {
                        updateStructure(*it);   //Update it in the 2d tracking array
                        it = trackActor[oldX][oldY].erase(it);  //Erase old spot
                    }
                    else
                        it++;
                }
                else
                    it++;
            }
        }
    }
    
    //Have characters that can't move do something, so that poison and pools can stun everyone there
    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            list<Actor*>::iterator it = trackActor[y][x].begin();
            if((*it) != nullptr && ! ((*it)->canMove()))    //If it ca't moved
            {
                if ( (*it) -> getState() != Actor::dead)    //If its not dead, do something
                    (*it)->doSomething();
                it++;
            }
        }
    }
    
    for(int a = 0; a<64; a++)   //After all moving actors moved, reset boolean for action to false
    {
        for(int b = 0; b<64; b++)
        {
            list<Actor*>::iterator it2 = trackActor[b][a].begin();
            while (it2 != trackActor[b][a].end())
            {
                (*it2)->adjustMoved(false);
                it2++;
            }
        }
    }
    
    removeDead();   //Take out all actors labeled dead
    
    setDisplayText();   //Update the ticker
    
    int winningColony = 0;
    
    if (m_tick == 2000) //End of game
    {
        if (isThereWinner(winningColony))   //Checks if there is a winner
        {
            setWinner(names[winningColony]);    //If so, then the winningColony holds colony number of winner, so find the name corresponding to colony numebr in names vector
            return GWSTATUS_PLAYER_WON;
        }
        else
            return GWSTATUS_NO_WINNER;
    }
    
    // the simulation is not yet over, continue!
    return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::LoadField()
{
    string fieldFile = getFieldFilename(), error;
    
    if (m_f.loadField(fieldFile, error) != Field::LoadResult::load_success)
    {
        setError(fieldFile + " " + error);
        return false; // something bad happened!
    }
    return true;
}

void StudentWorld::cleanUp()
{
    for(int r = 0; r < 64; r++) //Goes through each point in grid
    {
        for(int c = 0; c < 64; c++)
        {
            list<Actor*>::iterator it = trackActor[c][r].begin();
            while (it != trackActor[c][r].end())
            {
                (**it).~Actor();    //Calls destructor for every actor
                it++;
                trackActor[c][r].pop_front();   //Removes every actor
            }
        }
    }
}

bool StudentWorld::isThereWinner(int& winningColony)
{
    int winningAntNumber = antCount[0]; //Set winning ant to first one
    for(int i = 1; i < antCount.size(); i++)
    {
        if (antCount[i] >= winningAntNumber)    //If any subsequent ant counts greater than that
        {
            winningAntNumber = antCount[i]; //Set it as new winning ant
            winningColony = i;  //Set i as winning colony number
        }
    }
    
    for (int i = 0; i <antCount.size(); i++)  //Check tied scores and SEE WHO REACHED THE SCORE FASTER?
    {
        if ( i == winningColony)    //Don't compare if its already the winning ant
            continue;
        if( winningAntNumber == antCount[i] && m_curNum[i] < m_curNum[winningColony] )  //If they're tied, see who reached the score faster
            winningColony = i;  //Set new winning colony number
    }
    
    if( antCount[winningColony] < 6)    //In the end, see if the winning colony even made at least 6 ants
        return false;
    
    return true;
}

int StudentWorld::getWinningAnt()   //Winning ant for each round
{
    int winningAntNumber = antCount[0], winningColony = 0;
    for(int i = 1; i < antCount.size(); i++)    //Find maximum ant count
    {
        if (antCount[i] >= winningAntNumber)
        {
            winningAntNumber = antCount[i];
            winningColony = i;
        }
    }
    
    for (int i = 0; i <antCount.size(); i++)  //Check tied scores
    {
        if ( i == winningColony)
            continue;
        if( winningAntNumber == antCount[i])    //If tied, there is no winning colony
            winningColony = 4;
    }
    
    return winningColony;
}

void StudentWorld::setDisplayText()
{
    int winningAntNumber = getWinningAnt(); //Colony of winning ant
    
    ostringstream oss;
    oss << "Ticks: " << setw(5) << m_tick << " ";   //Ticks
    for( int i = 0; i < antCount.size(); i++)   //Goes through each ant
    {
        oss << names[i];    //Prints name
        if (i == winningAntNumber)  //If at the winning ant colony
            oss << "*"; //Put *
        oss.fill('0');  //Makes it a two digit number with 0 in front if a single digit
        oss << ": " << setw(2) << antCount[i] << " ants  ";
    }
    
    string s = oss.str();

    setGameStatText(s);
}

void StudentWorld::updateStructure(Actor* it)
{
    int newX = it->getX(), newY = it->getY();
    trackActor[newX][newY].push_back(it);
}

void StudentWorld::removeDead()     //Remove all the actors labeled dead
{
    for (int r = 0; r < 64; r++)
    {
        for (int c = 0; c < 64; c++)
        {
            list<Actor*>::iterator it = trackActor[c][r].begin();
            while (it != trackActor[c][r].end())
            {
                if( (*it) -> getState() == Actor::dead) //If labeled dead
                {
                    if ( (*it) -> canMove())    //and it is an insect
                    {
                        adjustAntCount( (*it) -> whatColony(), -1); //Remove count from colony
                        addFood(c, r, 100); //Add food to spot
                    }
                    (**it).~Actor();    //Delete all actors( insect or not)
                    it = trackActor[c][r].erase(it);
                }
                else
                    it++;
            }
        }
    }
}

void StudentWorld::addFood(int x, int y, int amount)
{
    Actor* check4Food = isFoodHere(x, y);
    if (check4Food != nullptr)  //If there is already food here
    {
        check4Food -> adjustHitpoint( check4Food-> whatHP() + amount);  //Adjust HP of that food
    }
    else
    {
        Actor* replaceFood = new Food(x,y,this, amount);    //Create new food
        add2Tracker(replaceFood, x, y); //Add to tracker
    }
}

void StudentWorld::addPheromone(int x, int y, int amount, int colony)
{
    Actor* check4Smell = isSmelly(x, y, colony);    //If there's pheromone of type colony here
    int image;
    switch (colony)
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
    
    if (check4Smell != nullptr ) //If there is already a pheromone of type colony here
    {
        check4Smell -> adjustHitpoint( check4Smell-> whatHP() + amount);    //add to it by amount
    }
    else
    {
        Actor* replaceSmell = new Pheromone(image, x,y,this, colony);   //otherwise make new pheromone of type colony
        add2Tracker(replaceSmell, x, y);
    }
}

bool StudentWorld::isPoison(int x,int y)
{
    list<Actor*>::iterator it = trackActor[x][y].begin();
    if( ! (trackActor[x][y].empty() ))
    {
        while (it != trackActor[x][y].end())
        {
            if ( (*it)->whoAmI() == IID_POISON )    //checks if poison is at linked list at (x,y)
                return true;
        }
    }
    return false;
}

Actor* StudentWorld::isFoodHere(int x, int y)
{
    list<Actor*>::iterator it = trackActor[x][y].begin();
    if( ! (trackActor[x][y].empty() ))
    {
        while (it != trackActor[x][y].end())
        {
            if ( (*it)->whoAmI() == IID_FOOD )  //checks if food is at linked list at  (x,y)
                return *it;
            it++;
        }
    }
    return nullptr;
}

Actor* StudentWorld:: isSmelly(int x, int y, int colony)
{
    list<Actor*>::iterator it = trackActor[x][y].begin();
    if( ! (trackActor[x][y].empty() ))
    {
        while (it != trackActor[x][y].end())
        {
            if ( (*it)->canSmell() && (*it) -> whatColony() == colony)  //checks if points to a pheromone and if that pheromone is of the same colony type
                return *it;
            it++;
        }
    }
    return nullptr;
}

void StudentWorld::adjustAntCount(int colony, int adj)
{
    switch(colony)
    {
        case 0: //For each colony that adjusts it...
            antCount[0] += adj; //Adjustment (either add positive or negative number)
            m_curNum[0] = m_tick;   //Tick that it reached the current ant count
            break;
        case 1:
            antCount[1] += adj;
            m_curNum[1] = m_tick;
            break;
        case 2:
            antCount[2] += adj;
            m_curNum[2] = m_tick;
            break;
        case 3:
            antCount[3] += adj;
            m_curNum[3] = m_tick;
            break;
        default:
            break;
    }
}

void StudentWorld::stunAll(int x, int y)
{
    list<Actor*> li = trackActor[x][y];    //Linked list at water's location
    list<Actor*>::iterator it = li.begin();
    while (it != li.end())  //Goes through list
    {
        if( (*it)->canStunPoison() && (*it)->getState() != Actor::stun)    //If an actor there is capable of being stunned and it hasn't left the spot yet, so still in a "stunned" state
        {
            (*it)->adjustParalysis( (*it)->whatParal() + 2);    //Stun it
            (*it)->adjustState(Actor::stun);
        }
        it++;
    }
}

void StudentWorld::poisonAll(int x, int y)
{
    list<Actor*> li = trackActor[x][y];    //Linked list at poisons location
    list<Actor*>::iterator it = li.begin();
    while (it != li.end())  //Go through lit
    {
        if((*it)->canStunPoison())  //If you can stun or poison it
        {
            (*it)->adjustHitpoint( (*it)->whatHP() - 150);  //Poison it
            if( (*it)->whatHP() <= 0)   //Check if dead after poisoning
                (*it)->adjustState(Actor::dead);
        }
        it++;
    }
}

bool StudentWorld::findEnemy(int x, int y, int colony)
{
    list<Actor*> li = trackActor[x][y];   //Linked list at (x,y)
    list<Actor*>::iterator it = li.begin();
    while (it != li.end())  //Goes thorugh list
    {
        if( (*it) -> canMove() && (*it)->whatColony() != colony)  //If its an insect & not of the same colony (ant, grasshopper), there is an enemy  there
            return true;
        it++;
    }
    return false;
}

Actor* StudentWorld::chooseRandomBite(Actor* bitingOne)
{
    list<Actor*> li = trackActor[bitingOne->getX()][bitingOne->getY()];   //Get linked list of actors sharing same spot as biting one
    if (li.size() == 1)   //Only that bug is on the square and no other actors there, so can't bite anything
        return nullptr;
    
    vector<Actor*> holdActor;
    list<Actor*>::iterator it = li.begin();
    while (it != li.end())
    {
        if( !(bitingOne->canStunPoison()) && (*it)->canMove() )  //If the biting one is a grasshopper and actor can moving, it's an enemy insect, so add to vector of enemies
            holdActor.push_back( (*it) );
        if( bitingOne->canStunPoison() && (*it)->canMove() && (*it)->whatColony() != bitingOne-> whatColony())  //If the biting one is an ant and actor can move and the actor is not from its colony, is an enemy
            holdActor.push_back( (*it) );
        
        it++;
    }
    
    Actor* bitten = nullptr;    //Holds pointer to the insect that will be bitten
    if( holdActor.size() > 1)
    {
        int flag = 0;
        while (flag == 0)
        {
            int pick = randInt(0, holdActor.size() - 1 );   //Randomly pick an insect to bite
            bitten = holdActor[pick];   //And assign it to bitten
            if (bitten != bitingOne)    //If the random insect isn't doing the biting, then exit loop
                flag = 1;
        }
    }
    return bitten;
}