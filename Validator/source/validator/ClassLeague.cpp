#include "ClassLeague.h"

League::League(int id, std::string name, int nrRound, GameMode mode, Compactness comp, IdList leagueGroupIds) : id(id), name(name), nrRound(nrRound), mode(mode), comp(comp) {
	// Add league groups
	for (auto id : leagueGroupIds) {
		addLeagueGroup(Instance::get()->getLeagueGroup(id));
	}	
}

void League::addLeagueGroup(LeagueGroup* lg) { 
	// Check whether league group already in league group set
	try{
		std::pair<LeagueGroupSetIt,bool> ret;
		ret = leagueGroups.insert(lg);
		if (ret.second == false) {
			std::stringstream msg;
			msg << "Failled to add league group " << lg<< " to league group member set of team " << this << ". Id already present.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
	}
}
void League::addMember(Team* t) { 
	// Check whether team is already in member set
	try{
		std::pair<TeamSetIt,bool> ret;
		ret = members.insert(t);
		if (ret.second == false) {
			std::stringstream msg;
			msg << "Failled to add team " << t->getId() << " to team member set of league " << id << ". Id already present.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
	}	
}

AttrMap League::serialize() {
	AttrMap attrs = {
		{"ClassType", "Leagues"},
		{"Type", "league"},
		{"id", std::to_string(id)},
		{"name", name},
		{"leagueGroups", idToString(leagueGroups)}
	};	
	return attrs;
}
