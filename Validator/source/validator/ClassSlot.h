#ifndef CLASSSLOT_H
#define CLASSSLOT_H
// Include std
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <list>
#include <algorithm>    // std::find

// Include other
#include "Globals.h"
#include "ClassSlotGroup.h"
#include "ClassException.h"

class Slot
{
public:
	// constructors
	Slot(int id, std::string name, IdList slotGroupIds);
	virtual ~Slot() {}

	// Getters and setters
	int getNrSlotsGroups() const{ return slotGroups.size(); }
	void addSlotGroup(SlotGroup* tg); 
	SlotGroupSetIt getFirstSlotGroup() { return slotGroups.begin(); }
	SlotGroupSetIt getLastSlotGroup() { return slotGroups.end(); }
	SlotGroupSet getSlotGroups() { return slotGroups; }

	void setName(const std::string nm) { name = nm; }
	std::string getName() const{ return name; }

	int getId() const{ return id; }

	AttrMap serialize();

	// operator overloading: sorting
	bool operator==(const Slot* other) { return this->getId() == other->getId(); }
	bool operator<(const Slot* other) { return this->getId() < other->getId(); }
	friend std::ostream& operator<<(std::ostream& os , const Slot& s) { return os << s.id; }
	friend std::ostream& operator<<(std::ostream& os , const Slot* s) { return os << s->id; }

protected:
	// Type here all protected member variables
	int id; 			// Unique identifier. Slots should be ordered from 0 to |S|	
	std::string name; 		// Non-unique alias
	SlotGroupSet slotGroups; 	// Link to all slot groups the slot belongs to
};

#endif /* CLASSSLOT_H */
