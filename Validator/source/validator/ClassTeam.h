#ifndef CLASSTEAM_H
#define CLASSTEAM_H

// Include std
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <list>
#include <algorithm>    // std::find


// Include other
#include "Globals.h"
#include "ClassLeague.h"
#include "ClassTeamGroup.h"
#include "ClassException.h"

class Team
{
public:
	// constructors, destructors, and factory method
	Team(int id, std::string name, int leagueId, IdList teamGroups);
	virtual ~Team() {}
	void free() { delete this; }


	// Getters and setters
	void setId(const int newId) { id = newId; }
	int getId() const { return id; }

	void setName(const std::string newName) { this->name = newName; }
	std::string getName() const{ return this->name; }

	void setLeagueId(League* l) { this->league = l; }
	League* getLeague() const{ return this->league; }

	int getNrTeamGroups() const{ return teamGroups.size(); }
	void addTeamGroup(TeamGroup* tg); 
	TeamGroupSetIt getFirstTeamGroup() { return teamGroups.begin(); }
	TeamGroupSetIt getLastTeamGroup() { return teamGroups.end(); }
	TeamGroupSet getTeamGroups() { return teamGroups; }

	AttrMap serialize();

	// operator overloading: sorting
	bool operator==(const Team* other) { return this->getId() == other->getId(); }
	bool operator<(const Team* other) { return this->getId() < other->getId(); }
	friend std::ostream& operator<<(std::ostream& os , const Team& t) { return os << t.id; }
	friend std::ostream& operator<<(std::ostream& os , const Team* t) { return os << t->id; }

protected:
	// Type here all protected member variables
	int id; 			// Unique identifier. Slots should be ordered from 0 to |T|
	std::string name; 		// Non-unique alias 
	League* league; 		// A team belongs to exactly one league
	TeamGroupSet teamGroups; 	// A team can belong to 0, 1, or more teamgroups
};

#endif /* CLASSTEAM_H */
