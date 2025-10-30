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

void SlotGroup::removeMember(Slot* s) {
    try {
        // Try to find the slot in the member set
        SlotSetIt it = members.find(s);
        if (it == members.end()) {
            std::stringstream msg;
            msg << "Failed to remove slot " << s->getId()
                << " from slot member set of slot group " << id
                << ". Slot not found.";
            throw_line_robinx(InterfaceReadingException, msg.str());
        }
        else {
            // Remove the slot from the member set
            members.erase(it);
        }
    }
    catch (InterfaceReadingException& e) {
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
