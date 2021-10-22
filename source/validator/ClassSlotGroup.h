#ifndef CLASSSLOTGROUP_H
#define CLASSSLOTGROUP_H
// Include std
#include <string>
#include <map>
#include <set>
#include <list>
#include <algorithm>    // std::find
// Include other
#include "Globals.h"
#include "ClassTeam.h"
#include "ClassSlot.h"
#include "ClassException.h"

class SlotGroup
{
public:
	// constructors
	// Members will be added when adding slots
	SlotGroup(int id, std::string name) : id(id), name(name) {}
	virtual ~SlotGroup() {}
	
	// Getters and setters
	int getId() const{ return id; }
	void setId( const int newId) { id = newId; }

	std::string getName() const{ return name; }
	void setName( const std::string newName) { name = newName; }

	int getNrMembers() const{ return members.size(); }
	void addMember(Slot* s); 
	SlotSetIt getFirstMember() { return members.begin(); }
	SlotSetIt getLastMember() { return members.end(); }
	bool isMember(Slot* s) const{ return members.count(s); }
	SlotSet getMembers() const { return members; }
	
	AttrMap serialize();

	// operator overloading: sorting
	bool operator==(const SlotGroup* other) { return this->getId() == other->getId(); }
	bool operator<(const SlotGroup* other) { return this->getId() < other->getId(); }
	friend std::ostream& operator<<(std::ostream& os , const SlotGroup& sg) { return os << sg.id; }
	friend std::ostream& operator<<(std::ostream& os , const SlotGroup* sg) { return os << sg->id; }

protected:
	// Type here all protected member variables
	int id;				// Unique id
	std::string name; 		// Non-unique alias
	SlotSet members;		// All slots belonging to the group
};

#endif /* CLASSSLOTGROUP_H */
