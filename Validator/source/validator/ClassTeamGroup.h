#ifndef CLASSTEAMGROUP_H
#define CLASSTEAMGROUP_H
// Include std
#include <string>
#include <map>
#include <set>
#include <list>
#include <algorithm>    // std::find

// Include other
#include "Globals.h"
#include "ClassTeam.h"
#include "ClassLeague.h"

class TeamGroup
{
public:
	// constructors
	// Members will be added when adding teams
	TeamGroup(int id, std::string name) : id(id), name(name) {}
	virtual ~TeamGroup() {}

	// Getters and setters
	int getId() const{ return id; }
	void setId(const int newId) { id = newId; }

	std::string getName() const{ return name; }
	void setName(const std::string newName) { name = newName; }

	int getNrMembers() const{ return members.size(); }
	void addMember(Team* t); 
	TeamSetIt getFirstMember() { return members.begin(); }
	TeamSetIt getLastMember() { return members.end(); }
	bool isMember(Team* t) const{ return members.count(t); }
	TeamSet getMembers() const { return members; }
	AttrMap serialize();

	// operator overloading: sorting
	bool operator==(const TeamGroup* other) { return this->getId() == other->getId(); }
	bool operator<(const TeamGroup* other) { return this->getId() < other->getId(); }
	friend std::ostream& operator<<(std::ostream& os , const TeamGroup& tg) { return os << tg.id; }
	friend std::ostream& operator<<(std::ostream& os , const TeamGroup* tg) { return os << tg->id; }

protected:
	// Type here all protected member variables
	int id; 			// Unique identifier
	std::string name; 		// Non-unique alias
	TeamSet members; 		// Pointer to all member teams
};

#endif /* CLASSTEAMGROUP_H */
