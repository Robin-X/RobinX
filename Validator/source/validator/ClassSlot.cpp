#include "ClassSlot.h"

Slot::Slot(int id, std::string name, IdList slotGroupIds) : id(id), name(name) {
	// Add slot groups
	for (auto id : slotGroupIds) {
		addSlotGroup(Instance::get()->getSlotGroup(id));
	}	
}

void Slot::addSlotGroup(SlotGroup* sg) { 
	// Check whether slot groups is already in slot group set
	try{
		std::pair<SlotGroupSetIt,bool> ret;
		ret = slotGroups.insert(sg);
		if (ret.second == false) {
			std::stringstream msg;
			msg << "Failled to add slot group " << sg->getId() << " to slot group set of slot " << id << ". Id already present.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
	}	
}
AttrMap Slot::serialize(){
	AttrMap attrs = {
		{"ClassType", "Slots"},
		{"Type", "slot"},
		{"id", std::to_string(id)},
		{"slotGroup", idToString(slotGroups)},
		{"name", name}
	};		
	return attrs;
}
