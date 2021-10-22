#include "ClassLeagueGroup.h"

void LeagueGroup::addMember(League* s) { 
	// Check whether slot is already in member set
	try{
		std::pair<LeagueSetIt,bool> ret;
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

AttrMap LeagueGroup::serialize(){
	AttrMap attrs = {
		{"ClassType", "LeagueGroups"},
		{"Type", "LeagueGroup"},
		{"id", std::to_string(id)},
		{"name", name}
	};	
	return attrs;
}
