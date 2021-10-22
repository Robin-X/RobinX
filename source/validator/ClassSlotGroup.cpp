#include "ClassSlotGroup.h"

void SlotGroup::addMember(Slot* s) { 
	// Check whether slot is already in member set
	try{
		std::pair<SlotSetIt,bool> ret;
		ret = members.insert(s);
		if (ret.second == false) {
			std::stringstream msg;
			msg << "Failled to add slot " << s->getId() << " to slot member set of slot group " << id << ". Id already present.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
	}	
}

AttrMap SlotGroup::serialize(){
	AttrMap attrs = {
		{"ClassType", "SlotGroups"},
		{"Type", "slotGroup"},
		{"id", std::to_string(id)},
		{"name", name}
	};	
	return attrs;
}
