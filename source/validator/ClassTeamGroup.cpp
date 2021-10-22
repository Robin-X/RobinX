#include "ClassTeamGroup.h"

void TeamGroup::addMember(Team* t) { 
	// Check whether team already in member set
	try{
		std::pair<TeamSetIt,bool> ret;
		ret = members.insert(t);
		if (ret.second == false) {
			std::stringstream msg;
			msg << "Failled to add team " << t->getId() << " to team member set of team group " << id << ". Id already present.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
	}	
}

AttrMap TeamGroup::serialize(){
	AttrMap attrs = {
		{"ClassType", "TeamGroups"},
		{"Type", "teamGroup"},
		{"id", std::to_string(id)},
		{"name", name}
	};	
	return attrs;
}
