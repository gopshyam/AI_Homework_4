// Agent.cc

#include <cstdlib> 
#include <ctime> 
#include <iostream>
#include "Agent.h"

#define START_X 0
#define START_Y 0

//states 0 and above are safe to visit
#define STENCH 2
#define SAFE 1
#define UNKNOWN 0
#define UNSAFE -1

using namespace std;

Agent::Agent ()
{
    hasArrow = true;
    wumpusKilled = false;
    hasGold = false;    
}

Agent::~Agent ()
{

}

void Agent::Initialize ()
{
    srand((unsigned)time(0));
    //Initialize positions, arrays
    numActions = 0;
    currentLocation = Location(START_X, START_Y);
    previousLocation = Location(-1, -1);
    for (int i=0; i < 100; i++) {
        for (int j=0; j < 100; j++) {
            safeLocations[i][j] = 0;
        }
    }

}

bool isValidLocation(int x, int y) {
    if (x < 0 || y < 0) {
        return false;
    }
    return true;
}

bool isValidLocation(Location location) {
    return isValidLocation(location.X, location.Y);
}

Location getNextLocation(Location location, Orientation agentOrientation){
    //Returns the location of a forward move
    switch(agentOrientation){
        case RIGHT:
            location.X++;
            break;
        case UP:
            location.Y++;
            break;
        case LEFT:
            location.X--;
            break;
        case DOWN:
            location.Y--;
            break;
    }
    return location;
}


void Agent::updatePosition(Percept &percept) {
    if (numActions == 0) {
        return;
    }
    Action prevAction = actionList[numActions];

    if (prevAction == GOFORWARD) {
        previousLocation = currentLocation;
        if (percept.Bump) {
            return;
        }
        switch(agentOrientation){
            case RIGHT:
                currentLocation.X++;
                break;
            case UP:
                currentLocation.Y++;
                break;
            case LEFT:
                currentLocation.X--;
                break;
            case DOWN:
                currentLocation.Y--;
                break;
        }
    return;
    }

    if (prevAction == TURNLEFT) {
        switch(agentOrientation) {
            case RIGHT:
                agentOrientation = UP;
                break;
            case UP:
                agentOrientation = LEFT;
                break;
            case LEFT:
                agentOrientation = DOWN;
                break;
            case DOWN:
                agentOrientation = RIGHT;
                break;
        }
        return;
    }
}


void Agent::updateResultsOfPreviousAction(Percept &percept){
    if (numActions == 0) {
        return;
    }
    Action prevAction = actionList[numActions];
    if (prevAction == GRAB) {
        hasGold = true;
    }
    if (prevAction == SHOOT) {
        hasArrow = false;
        if (percept.Scream) {
            wumpusKilled = true;
        } else {
            wumpusKilled = false;
        }
    }
}

void Agent::updateSafety(Percept &percept) {
    int x = currentLocation.X;
    int y = currentLocation.Y;
    if (percept.Stench || percept.Breeze) {
        //Mark all adjacent unexplored states as UNSAFE
        //RIGHT
        if (isValidLocation(x+1, y) && safeLocations[x+1][y] == UNKNOWN) {
            safeLocations[x+1][y] = UNSAFE;
        }
        //UP
        if (isValidLocation(x, y+1) && safeLocations[x][y+1] == UNKNOWN) {
            safeLocations[x][y+1] = UNSAFE;
        }
        //LEFT
        if (isValidLocation(x-1, y) && safeLocations[x-1][y] == UNKNOWN) {
            safeLocations[x-1][y] = UNSAFE;
        }
        //DOWN
        if (isValidLocation(x, y-1) && safeLocations[x][y-1] == UNKNOWN) {
            safeLocations[x][y-1] == UNSAFE;
        }

        if (percept.Stench) {
            safeLocations[x][y] == STENCH;
        }

        return;
    } else {
        //If no dangerous percepts, then all adjacent squares must be safe
        if (isValidLocation(x+1, y) && safeLocations[x+1][y] == UNKNOWN) {
            safeLocations[x+1][y] = SAFE;
        }
        //UP
        if (isValidLocation(x, y+1) && safeLocations[x][y+1] == UNKNOWN) {
            safeLocations[x][y+1] = SAFE;
        }
        //LEFT
        if (isValidLocation(x-1, y) && safeLocations[x-1][y] == UNKNOWN) {
            safeLocations[x-1][y] = SAFE;
        }
        //DOWN
        if (isValidLocation(x, y-1) && safeLocations[x][y-1] == UNKNOWN) {
            safeLocations[x][y-1] == SAFE;
        }
        return;
    }
}


Action Agent::Process (Percept& percept)
{
    int sdf;
    cin >> sdf;
    //Update the position of the agent based on the percept.
    updatePosition(percept);
    updateResultsOfPreviousAction(percept);
    updateSafety(percept);
	char c;
	Action action;
	bool validAction = true;


    //What are the cases we need to handle?
    //1. If the agent finds the gold, it must pick it up, and then backtrack
    //2. If agent is in a location, mark the location safe based on percepts, or mark other locations unsafe, also based on percepts
    //3. We need to keep track of stenches in the world. 
    //4. Need to check if it is possible to get stuck.
    //      How is that done? Check if the action pool is maxed out
    //      Also check if there is no safe path to move.



    //Pick a random action
    bool validMove = false;
    while(!validMove) {
        Action randomAction;
        int randomIndex = rand() % 3;
        cout << "ACTION " << randomIndex;
        switch(randomIndex) {
            case 0:
                randomAction = GOFORWARD;
                break;
            case 1:
                randomAction = TURNLEFT;
                break;
            case 2:
                randomAction = TURNRIGHT;
                break;
        }
        if (randomAction == GOFORWARD) {
            Location nextLocation = getNextLocation(currentLocation, agentOrientation);
            if (isValidLocation(nextLocation) && safeLocations[nextLocation.X][nextLocation.Y] >= 0) {
                action = randomAction;
                validMove = true;
            } else {
                cout << "FORWARD MOVE FAILED";
            }
        } else {
            action = randomAction;
            validMove = true;
        }
    } 
    

    //1. Check for gold
    if (percept.Glitter) {
        action = GRAB;
    }
    

	while (! validAction)
	{
		validAction = true;
		cout << "Action? ";
		cin >> c;
		if (c == 'f') {
			action = GOFORWARD;
		} else if (c == 'l') {
			action = TURNLEFT;
		} else if (c == 'r') {
			action = TURNRIGHT;
		} else if (c == 'g') {
			action = GRAB;
		} else if (c == 's') {
			action = SHOOT;
		} else if (c == 'c') {
			action = CLIMB;
		} else {
			cout << "Huh?" << endl;
			validAction = false;
		}
	}

    actionList[numActions++] = action;
	return action;
}

void Agent::GameOver (int score)
{

}

