// Agent.h

#ifndef AGENT_H
#define AGENT_H

#include "Location.h"
#include "Orientation.h"
#include "Action.h"
#include "Percept.h"

class Agent
{
private:
    Action actionList[1000];
    int numActions;
    int safeLocations[100][100];
    bool backtrack;
    bool hasArrow;
    bool wumpusKilled;
    bool hasGold;
    bool foundGold;
    Location previousLocation;
    Location currentLocation;
    Orientation agentOrientation;

    void updatePosition(Percept &percept);
    void updateResultsOfPreviousAction(Percept &percept);
    void updateSafety(Percept &percept);

public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);
};

#endif // AGENT_H
