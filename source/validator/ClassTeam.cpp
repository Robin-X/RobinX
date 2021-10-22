#include "ClassTeam.h"

Team::Team(int id, std::string name, int leagueId, IdList teamGroupIds) :  id(id), name(name){
	// Get pointer to league and save
	league = Instance::get()->getLeague(leagueId);
	
	// Add team groups
	for (auto id : teamGroupIds) {
		addTeamGroup(Instance::get()->getTeamGroup(id));
	}
}

void Team::addTeamGroup(TeamGroup* tg) { 
	// Check whether team group already in team group set
	try{
		std::pair<TeamGroupSetIt,bool> ret;
		ret = teamGroups.insert(tg);
		if (ret.second == false) {
			std::stringstream msg;
			msg << "Failled to add team group " << tg->getId() << " to team group member set of team " << id << ". Id already present.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
	}
}

AttrMap Team::serialize() {
	AttrMap attrs = {
		{"ClassType", "Teams"},
		{"Type", "team"},
		{"id", std::to_string(id)},
		{"league", std::to_string(league->getId())},
		{"name", name},
		{"teamGroups", idToString(teamGroups)}
	};	
	return attrs;	
}
