// Include std
#ifndef CLASSLEAGUE_H
#define CLASSLEAGUE_H
#include <string>
#include <map>
#include <set>
#include <list>
#include <algorithm>    // std::find

#include "Globals.h"
#include "ClassTeam.h"
#include "ClassInstance.h"
#include "Globals.h"

class League
{
public:
	// constructors
	// Team members are added when new teams are added
	League(int id, std::string name, int nrRound, GameMode mode, Compactness comp) : id(id), name(name), nrRound(nrRound), mode(mode), comp(comp) {};
	League(int id, std::string name, int nrRound, GameMode mode, Compactness comp, IdList leagueGroupIds);
	virtual ~League() {}
	
	// Getters and setters
	int getId() const{ return id; }
	void setId(const int newId) { id = newId; }

	std::string getName() const{ return name; }
	void setName(const std::string newName) { name = newName; }

	int getNrMembers() const{ return members.size(); }
	void addMember(Team* t);
	TeamSet getMembers() { return members; }
	TeamSetIt getFirstMember() { return members.begin(); }
	TeamSetIt getLastMember() { return members.end(); }

	int getNrLeagueGroups() const{ return leagueGroups.size(); }
	void addLeagueGroup(LeagueGroup* lg); 
	LeagueGroupSetIt getFirstLeagueGroup() { return leagueGroups.begin(); }
	LeagueGroupSetIt getLastLeagueGroup() { return leagueGroups.end(); }
	LeagueGroupSet getLeagueGroups() { return leagueGroups; }

	int getNrRound() const{ return nrRound; }
	void setNrRound(const int nr) { nrRound = nr; }	

	GameMode getMode() const{ return mode; }
	void setMode(const GameMode newMode) { mode = newMode; }
	
	Compactness getComp() const{ return comp; }
	void setComp( const Compactness newComp) { comp = newComp; }
	
	AttrMap serialize();
	
	// operator overloading: sorting
	bool operator==(const League* other) { return this->getId() == other->getId(); }
	bool operator<(const League* other) { return this->getId() < other->getId(); }
	friend std::ostream& operator<<(std::ostream& os , const League& l) { return os << l.id; }
	friend std::ostream& operator<<(std::ostream& os , const League* l) { return os << l->id; }

protected:
	// Type here all protected member variables
	int id; 		// Id of the league
	std::string name; 	// Non-unique alias
	TeamSet members;	// Pointer to all id's of associated teams
	int nrRound; 		// Number of round robins played in the league
	GameMode mode; 		// Game mode of the league
	Compactness comp; 	// Compactness of the league
	LeagueGroupSet leagueGroups; // All league groups of which the league is a member of
};

#endif /* CLASSLEAGUE_H */
