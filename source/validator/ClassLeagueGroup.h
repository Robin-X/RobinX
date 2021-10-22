#ifndef CLASSLEAGUEGROUP_H
#define CLASSLEAGUEGROUP_H
// Include std
#include <string>
#include <map>
#include <set>
#include <list>
#include <algorithm>    // std::find
// Include other
#include "Globals.h"
#include "ClassLeague.h"
#include "ClassException.h"

class LeagueGroup
{
public:
	// constructors
	// Members will be added when adding leagues
	LeagueGroup(int id, std::string name) : id(id), name(name) {}
	virtual ~LeagueGroup() {}
	
	// Getters and setters
	int getId() const{ return id; }
	void setId( const int newId) { id = newId; }

	std::string getName() const{ return name; }
	void setName( const std::string newName) { name = newName; }

	int getNrMembers() const{ return members.size(); }
	void addMember(League* s); 
	LeagueSetIt getFirstMember() { return members.begin(); }
	LeagueSetIt getLastMember() { return members.end(); }
	bool isMember(League* s) const{ return members.count(s); }
	LeagueSet getMembers() const { return members; }
	
	AttrMap serialize();

	// operator overloading: sorting
	bool operator==(const LeagueGroup* other) { return this->getId() == other->getId(); }
	bool operator<(const LeagueGroup* other) { return this->getId() < other->getId(); }
	friend std::ostream& operator<<(std::ostream& os , const LeagueGroup& lg) { return os << lg.id; }
	friend std::ostream& operator<<(std::ostream& os , const LeagueGroup* lg) { return os << lg->id; }

protected:
	// Type here all protected member variables
	int id;				// Unique id
	std::string name; 		// Non-unique alias
	LeagueSet members;		// All leagues belonging to the group
};

#endif /* CLASSSLEAGUEGROUP_H */
