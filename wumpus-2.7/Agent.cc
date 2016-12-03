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
    agentOrientation = RIGHT;
    currentLocation = Location(START_X, START_Y);
    previousLocation = Location(-1, -1);
    for (int i=0; i < 100; i++) {
        for (int j=0; j < 100; j++) {
            safeLocations[i][j] = 0;
        }
    }
    safeLocations[START_X][START_Y] = SAFE;

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
    Action prevAction = actionList[numActions - 1];
    if (prevAction == GOFORWARD) {
        if (percept.Bump) {
            return;
        }

        previousLocation = currentLocation;
        backtrack = false;
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

    if (prevAction == TURNRIGHT) {
        switch(agentOrientation) {
            case RIGHT:
                agentOrientation = DOWN;
                break;
            case UP:
                agentOrientation = RIGHT;
                break;
            case LEFT:
                agentOrientation = UP;
                break;
            case DOWN:
                agentOrientation = LEFT;
                break;
        }
        return;
    }
}


void Agent::updateResultsOfPreviousAction(Percept &percept){
    if (numActions == 0) {
        return;
    }
    Action prevAction = actionList[numActions-1];
    if (prevAction == GRAB) {
        hasGold = true;
        return;
        }
    if (prevAction == SHOOT) {
        //TODO: Update results so that if the Wumpus isn't killed, then all the squares which the agent is facing is are marked safe, and the current one is marked safe too
        hasArrow = false;
        if (percept.Scream) {
            wumpusKilled = true;
        } else {
            wumpusKilled = false;
            //Mark the forward location as safe
            Location nextLocation = getNextLocation(currentLocation, agentOrientation);
            safeLocations[nextLocation.X][nextLocation.Y] = SAFE;
            safeLocations[currentLocation.X][currentLocation.Y] = SAFE;
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
            if (percept.Stench && !wumpusKilled) {
                safeLocations[x+1][y] = STENCH;
            }
        }
        //UP
        if (isValidLocation(x, y+1) && safeLocations[x][y+1] == UNKNOWN) {
            safeLocations[x][y+1] = UNSAFE;
            if (percept.Stench && !wumpusKilled) {
                safeLocations[x][y+1] = STENCH;
            }
        }
        //LEFT
        if (isValidLocation(x-1, y) && safeLocations[x-1][y] == UNKNOWN) {
            safeLocations[x-1][y] = UNSAFE;
            if (percept.Stench && !wumpusKilled) {
                safeLocations[x-1][y] = STENCH;
            }
        }
        //DOWN
        if (isValidLocation(x, y-1) && safeLocations[x][y-1] == UNKNOWN) {
            safeLocations[x][y-1] == UNSAFE;
            if (percept.Stench && !wumpusKilled) {
                safeLocations[x][y-1] == STENCH;
            }
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
        int randomIndex = rand() % 6;
        switch(randomIndex) {
            case 0:
                randomAction = TURNLEFT;
                break;
            case 1:
                randomAction = TURNRIGHT;
                break;
            default:
                randomAction = GOFORWARD;
        }
        if (actionList[numActions-1] == TURNLEFT || actionList[numActions-1] == TURNRIGHT) {
            randomAction == GOFORWARD;
        }
        
        if (randomAction == GOFORWARD) {
            Location nextLocation = getNextLocation(currentLocation, agentOrientation);
            if (isValidLocation(nextLocation) && safeLocations[nextLocation.X][nextLocation.Y] >= 0) {
                action = randomAction;
                validMove = true;
            }
        } else {
            action = randomAction;
            validMove = true;
        }
        if (actionList[numActions-1] == GOFORWARD && actionList[numActions-1] == GOFORWARD && randomAction == GOFORWARD) {
            validMove = false;
        }
    } 
    
    //4-5. Stench and Breeze
    if (percept.Breeze) {
        backtrack = true;
    }

    if (percept.Stench && hasArrow && !wumpusKilled) {
        action = SHOOT;
    }

    if (backtrack) {
        if (getNextLocation(currentLocation, agentOrientation) == previousLocation) {
            action == GOFORWARD;
        } else {
            action == TURNRIGHT;
        }
    }


    if (percept.Glitter && !hasGold) {
        action = GRAB;
        hasGold = true;
    }
    
    //Edge cases:
    //If agent is at start position, and has the gold, the CLIMB out
    if (currentLocation.X == START_X && currentLocation.Y == START_Y) {
        if (percept.Stench){
            if (hasArrow && !wumpusKilled) {
                action = SHOOT;
            }
        }
        if (hasGold) {
            action = CLIMB;
        }
        if (percept.Breeze) {
            //If agent detects a breeze in the start position, then GAME OVER       
            action = CLIMB;
        }
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
    if (!hasGold) {
        cout <<"CANNOT SAFELY GET GOLD IN THIS CASE\n";
    }
}

