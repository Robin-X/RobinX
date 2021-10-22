#ifndef CLASSMEETING_H
#define CLASSMEETING_H

#include "ClassTeam.h"
#include "ClassSlot.h"

class Meeting
{
public:
	// constructor
	Meeting(Team* t1, Team* t2, bool mode, int slot = -1);
	~Meeting() {}
	
	// Team modifiers
	Team* getFirstTeam() const{ return team1; }
	Team* getSecondTeam() const{ return team2; }
	void setFirstTeam(Team* t) { team1 = t; }
	void setSecondTeam(Team* t) { team2 = t; }
	void swapTeams() { std::swap(team1, team2); }
	
	// Slot modifiers
	Slot* getAssignedSlot() const{ return assignedSlot; }
	void setAssignedSlot(Slot* newSlot) { assignedSlot = newSlot; }

	// modus modifiers
	bool getNoHome() const{ return noHome; }
	void setNoHome(const bool newMod) { noHome = newMod; } 

private:
	Team* team1;			// First team
	Team* team2;			// Second team
	bool noHome;			// True if the home advantage is undetermined. False if t1 is the home team, and t2 is the away team.
	Slot* assignedSlot = NULL; 	// Date of play: initially no meeting is scheduled yet
};

// Comparator functions
bool compMeetingScheduledSlot(const Meeting* meeting1, const Meeting* meeting2);
bool compMeetingNoHome(const Meeting* meeting1, const Meeting* meeting2);

#endif /* CLASSMEETING_H */
