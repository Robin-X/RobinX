#include "ClassConstraint.h" // Convert enum to string
#include <random>
#include <algorithm>
#include <iterator>

#define IN Instance::get()

template<typename T>
std::string printSet(std::set<T*> set){
	// Requires a set of objects that have getId() function.
	// Compact print all member ids
	std::stringstream members;
	members << ""; // Avoid segmentation error when set is empty
	for (auto m : set) {
		if (!members.str().empty()) {
			members << ", ";	
		}
		members << m->getId();
	}

	return members.str();
}

std::string printIntPairSet(std::set<std::pair<int,int>> set){
	std::stringstream members;
	for(auto m : set){
		if (!members.str().empty()) {
			members << ", ";
		} else { members << "{"; }
		members << "(" << m.first << "," << m.second << ")";
	}	
	members << "}";
	return members.str();
}

BA1::BA1(CType c, int p) : Constraint(c, p, "BA1") {}
AttrMap BA1::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "BasicConstraints";
	attrs["Type"] = "BA1";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);
	return attrs;
}

ObjCost BA1::checkConstr(){
	/* All meetings should be scheduled.
	 * One violation per unscheduled meeting. */
	ObjCost c = std::make_pair(0,0);
	for(auto m : IN->getMeetings()){
		if (m->getAssignedSlot() == NULL) {
			(type == HARD) ? c.first += penalty : c.second += penalty;
			std::stringstream msg1;
			msg1 << "Unscheduled meeting between team " << m->getFirstTeam() << " (" << m->getFirstTeam()->getName() << ")" << " and " << m->getSecondTeam() << " (" << m->getSecondTeam()->getName() << ")" << ".";	
			if (penalty > 0) {
				std::cout << std::setw(10) << name << std::setw(10) << "" << std::setw(50) << msg1.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
			}
		}	
	}
	return c;
}

BA2::BA2(CType c, int p) : Constraint(c, p, "BA2") {}
AttrMap BA2::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "BasicConstraints";
	attrs["Type"] = "BA2";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);
	return attrs;
}
ObjCost BA2::checkConstr(){
	/* A team should play at most one game per slot.
	 * For each slot and team, a violation with a deviation equal to the number of games above one. */
	ObjCost c = std::make_pair(0,0);
	int deviation;
	for(auto t : IN->getTeams()){
		for (auto s : IN->getSlots()) {
			MeetingList meetings = IN->getMeetingsTeamSlot({t.second}, {s.second}, HA);	
			deviation = meetings.size() - 1;
			if (deviation > 0) {
				(type == HARD) ? c.first += penalty*deviation : c.second += penalty*deviation;
				std::stringstream msg1;
				msg1 << "Team " << t.second << " plays " << deviation + 1 << " games in slot " << s.second;
				std::cout << std::setw(10) << name << std::setw(10) << "" << std::setw(50) << msg1.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
			}	
		}
	}
	return c;
}

CA1::CA1 (CType c, int p, std::array<IdList, 2> teamIds, int min, int max, HomeMode m, std::array<IdList, 2> slotIds) : Constraint(c,p, "CA1"), min(min), max(max), mode(m){
	for (auto id : teamIds[0]){ teams.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds[1]){ teamGroups.insert(Instance::get()->getTeamGroup(id)); }	
	for (auto id : slotIds[0]){ slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]){ slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
CA1::CA1 (CType c, int p) : Constraint(c,p, "CA1"){
	// Choose a random team
	Team* t = IN->getRandTeam();
	teams = {t};

	if (c == HARD) {
		// Determine how many time slots to draw, |S|\sim  Pr(1, 2, 3, 4 | 40, 20, 20, 20)
		slots = IN->getRandSlots({1,2,3,4}, {2,1,1,1});
		const int nrSlotsChosen = slots.size();

		//const int valuesSlot[4] = {1,2,3,4};
		//std::discrete_distribution<int> randNrSlot {2,1,1,1};
		//const int nrSlotsChosen = valuesSlot[randNrSlot(randEngine)];

		//// Choose the time slots themselves
		//std::vector<int> slotIds(IN->getNrSlots());
		//std::iota (slotIds.begin(), slotIds.end(), 0); 
		//std::shuffle(slotIds.begin(), slotIds.end(), randEngine);
		//assert(slotIds.size() >= nrSlotsChosen);
		//for(int id=0; id < nrSlotsChosen; ++id){
		//	slots.insert(IN->getSlot(slotIds.at(id)));
		//}

		// Count the total number of home games
		int nh = IN->getMeetingsTeamSlot({t}, slots, H).size();
		int na = IN->getMeetingsTeamSlot({t}, slots, A).size();
		if (nh < na) {
			mode = H;
			min = 0;
			max = std::max(nh, (int) (nrSlotsChosen > 1));
		} else {
			mode = A;
			min = 0;
			max = std::max(na, (int) (nrSlotsChosen > 1));
		}
	} else {
		// Determine how many time slots to draw, |S|\sim  Pr(1, 2, 3, 4 | 40, 20, 20, 20)
		slots = IN->getRandSlots({1,2,3,4,5,6}, {30,14,14,14,14,14});
		const int nrSlotsChosen = slots.size();

		//// Determine how many time slots to draw, |S|\sim  Pr(1, 2, 3, 4, 5, 6 | 30, 14, 14, 14, 14, 14)
		//const int valuesSlot[6] = {1,2,3,4,5,6};
		//std::discrete_distribution<int> randNrSlot {30,14,14,14,14,14};
		//const int nrSlotsChosen = valuesSlot[randNrSlot(randEngine)];

		//// Choose the time slots themselves
		//std::vector<int> slotIds(IN->getNrSlots());
		//std::iota (slotIds.begin(), slotIds.end(), 0); 
		//std::shuffle(slotIds.begin(), slotIds.end(), randEngine);
		//assert(slotIds.size() >= nrSlotsChosen);
		//for(int id=0; id < nrSlotsChosen; ++id){
		//	slots.insert(IN->getSlot(slotIds.at(id)));
		//}

		// Randomly decide the home mode
		std::uniform_int_distribution<int> randBool(0, 1);
		mode = (randBool(randEngine)) ? H : A;
		min = 0;
		max = std::max((int)(nrSlotsChosen/2)-1, (int)(nrSlotsChosen > 1));
	}
}
AttrMap CA1::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "CapacityConstraints";
	attrs["Type"] = "CA1";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["min"] = std::to_string(min);
	attrs["max"] = std::to_string(max);
	attrs["mode"] = HomeModeToStr[mode];
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	return attrs;
}
ObjCost CA1::checkConstr(){
	/* Each team in team group T 1 plays at least k min and at most k max { home
	 * games, away games, games } in time group S.
	 * Each team in T 1 triggers a deviation equal to the number of home games
	 * in S less than k or more than k max.
	 */
	ObjCost c = std::make_pair(0,0);
	TeamSet allTeams = IN->collectTeams(teams, teamGroups);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);
	for (auto t : allTeams) {
		int nrMeetings = IN->getMeetingsTeamSlot({t}, allSlots, mode).size();
		int p = std::max(0, nrMeetings-max) + std::max(0, min-nrMeetings);
		if (p != 0) {
			(type == HARD) ? c.first += p*penalty : c.second += p*penalty;
			std::stringstream msg1;
			msg1 << "Team " << std::setw(3) << t->getId() << " (" << t->getName() << ") has " << std::setw(3) << nrMeetings
			       	<< " " << std::setw(2) << HomeModeToStr[mode] << "-games in S. Allowed range is [" << min << "," << max <<"].";
			std::cout << std::setw(10) << name << std::setw(10) << "" << std::setw(50) << msg1.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
			std::cout << std::setw(20) << "" <<  "Slots: " << printSet(allSlots) << std::endl;
		}
	}
	return c;
}

CA2::CA2 (CType c, int p, std::array<IdList, 2> teamIds1, int min, int max, HomeMode m1, GlobMode m2, std::array<IdList, 2> teamIds2, std::array<IdList, 2> slotIds) : Constraint(c,p, "CA2"), min(min), max(max), mode1(m1), mode2(m2){
	for (auto id : teamIds1[0]){ teams1.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds1[1]){ teamGroups1.insert(Instance::get()->getTeamGroup(id)); }	
	for (auto id : teamIds2[0]){ teams2.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds2[1]){ teamGroups2.insert(Instance::get()->getTeamGroup(id)); }	
	for (auto id : slotIds[0]){ slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]){ slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
CA2::CA2 (CType c, int p) : Constraint(c,p, "CA2"){
	/* Pick a random team t
	 * Pick four random time slots, and denote these time slots with S
	 * Denote with O the opponents of t in S
	 * Pick three teams from T\O (these teams act as a strength group)
	 * Team t should play at most one game against teams in this strength group during time slots in S
	 */

	// Pick a random team
	Team *t = IN->getRandTeam();
	teams1.insert(t);

	if(c==HARD){
		// Randomly choose a mode h, a, or ha
		std::uniform_int_distribution<int> randMode(0, 2);
		const int dummy = randMode(randEngine);

		if (dummy == 0 || dummy == 1) {
			// Choose one time slot
			slots = IN->getRandSlots({1}, {1});	
			const int nrSlotsChosen = slots.size();

			mode1 = (dummy == 0) ? H : A;		

			// Derive opponents against whom we play home
			MeetingList meetings = IN->getMeetingsTeamSlot({t}, slots, mode1);
			std::set<int> opponentIds = {};
			for(auto m : meetings) { 
				opponentIds.insert( (m->getFirstTeam()->getId() == t->getId()) ? m->getSecondTeam()->getId() : m->getFirstTeam()->getId()); 
			}

			// Pick |S| teams from T\O
			std::vector<int> teamIds;
			for(int i=0; i < IN->getNrTeams(); i++) { 
				if(!opponentIds.count(i) && i !=t->getId()){
					teamIds.push_back(i); 
				}
			}
			std::shuffle(teamIds.begin(), teamIds.end(), randEngine);
			for (int i = 0; i < std::min(slots.size(), teamIds.size()); ++i) {
				teams2.insert(IN->getTeam(teamIds.at(i)));
			}	

			min = 0;
			max = 0;
			mode2 = GLOBAL;
		} else {
			assert(dummy == 2);
			mode1 = HA;		

			// Determine how many time slots to draw, |S|\sim  Pr(1, 2, 3, 4 | 40, 20, 20, 20)
			slots = IN->getRandSlots({4,5,6}, {1,1,1});	
			const int nrSlotsChosen = slots.size();

			// Derive opponents against whom we play
			MeetingList meetings = IN->getMeetingsTeamSlot({t}, slots, mode1);
			std::set<int> opponentIds = {};
			for(auto m : meetings) { 
				opponentIds.insert( (m->getFirstTeam()->getId() == t->getId()) ? m->getSecondTeam()->getId() : m->getFirstTeam()->getId()); 
			}

			// Pick floor(|S|/2)-1 teams from O
			std::vector<int> opponentIdsVec(opponentIds.begin(), opponentIds.end());
			std::shuffle(opponentIdsVec.begin(), opponentIdsVec.end(), randEngine);
			for (int i = 0; i < std::min((int) (nrSlotsChosen/2)-1, (int) opponentIdsVec.size()); ++i) {
				teams2.insert(IN->getTeam(opponentIdsVec.at(i)));
			}	


			// Pick ceil(|S|/2)+1 teams from T\O
			std::vector<int> teamIds;
			for(int i=0; i < IN->getNrTeams(); i++) { 
				if(!opponentIds.count(i) && i !=t->getId()){
					teamIds.push_back(i); 
				}
			}
			std::shuffle(teamIds.begin(), teamIds.end(), randEngine);
			for (int i = 0; i < std::min((int) ((nrSlotsChosen+1)/2)+1, (int) teamIds.size()); ++i) {
				teams2.insert(IN->getTeam(teamIds.at(i)));
			}	

			assert(teams2.size() == nrSlotsChosen);
			min = 0;
			// Set equal to the number of games against teams2: can be that you play multiple times against the same team --> at least floor(|S|/2)-1
			max = IN->getMeetingsTeamTeamSlot({t}, teams2, slots, mode1).size();
			mode2 = GLOBAL;
		}
	} else {
		assert(c==SOFT);

		//// Determine how many time slots to draw, |S|\sim  Pr(6,7,8 | 1,1,1)
		slots = IN->getRandSlots({6,7,8}, {1,1,1});
		const int nrSlotsChosen = slots.size();

		// Choose |S| teams
		std::vector<int> teamIds;
		for (int i = 0; i < IN->getNrTeams(); ++i) {
			if(i==t->getId()){
				continue;
			}	
			teamIds.push_back(i);
		}

		std::shuffle(teamIds.begin(), teamIds.end(), randEngine);
		for (int id = 0; id < std::min(nrSlotsChosen, (int) teamIds.size()); ++id) {
			teams2.insert(IN->getTeam(teamIds.at(id)));		
		}

		// Randomly choose a mode h, a, or ha
		std::uniform_int_distribution<int> randMode(0, 2);
		const int dummy = randMode(randEngine);
		if (dummy == 0 || dummy == 1) {
			mode1 = (dummy == 0) ? H : A;		
			mode2 = GLOBAL;
			min=0;
			max=(int)(nrSlotsChosen/2)-1;
		} else {
			assert(dummy == 2);
			mode1 = HA;
			mode2 = GLOBAL;
			min=0;
			max=(int)(nrSlotsChosen/2)-2;
		}
	}
}
AttrMap CA2::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "CapacityConstraints";
	attrs["Type"] = "CA2";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams1"] = idToString(teams1);
	attrs["teamGroups1"] = idToString(teamGroups1);
	attrs["teams2"] = idToString(teams2);
	attrs["teamGroups2"] = idToString(teamGroups2);
	attrs["min"] = std::to_string(min);
	attrs["max"] = std::to_string(max);
	attrs["mode1"] = HomeModeToStr[mode1];
	attrs["mode2"] = GlobModeToStr[mode2];
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	return attrs;
}

ObjCost CA2::checkConstr(){
	/* Each team in team group T 1 plays at least k min and at most k max {home games, away games,
	 * games} against {teams, each other team} in team group T 2 in time group S .
	 * Each team in T 1 triggers a deviation equal to the number of home games against teams in
	 * T 2 in S less than k min or more than k max .
	 */
	ObjCost c = std::make_pair(0,0);
	TeamSet allTeams1 = IN->collectTeams(teams1, teamGroups1);
	TeamSet allTeams2 = IN->collectTeams(teams2, teamGroups2);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);
	assert(mode2 == EVERY || mode2 == GLOBAL);
	if (mode2 == GLOBAL) {
		for (auto t : allTeams1) {
			int nrMeetings = IN->getMeetingsTeamTeamSlot({t}, allTeams2, allSlots, mode1).size();
			int p = std::max(0, nrMeetings-max) + std::max(0, min-nrMeetings);
			if (p != 0) {
				(type == HARD) ? c.first += p*penalty : c.second += p*penalty;
				std::stringstream msg1;
				msg1 << "Team " << std::setw(3) << t->getId() << " (" << t->getName() << ") has " << std::setw(3) << nrMeetings
						<< " " << std::setw(2) << HomeModeToStr[mode1] << "-games against teams in T2. Allowed range is [" << min << "," << max <<"].";
				std::cout << std::setw(10) << name << std::setw(10) << "" << std::setw(50) << msg1.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
				std::cout << std::setw(20) << "" <<  "T2: " << printSet(allTeams2) << std::endl;
				std::cout << std::setw(20) << "" <<  "Slots: " << printSet(allSlots) << std::endl;
			}
		}
	} else { // Every
		for (auto t1 : allTeams1) {
			for(auto t2 : allTeams2) {
				if (t1 == t2) { continue; }
				int nrMeetings = IN->getMeetingsTeamTeamSlot({t1}, {t2}, allSlots, mode1).size();
				int p = std::max(0, nrMeetings-max) + std::max(0, min-nrMeetings);
				if (p != 0) {
					(type == HARD) ? c.first += p*penalty : c.second += p*penalty;
					std::stringstream msg1;
					msg1 << "Team " << std::setw(3) << t1->getId() << " has " << std::setw(3) << nrMeetings
						<< " " << std::setw(2) << HomeModeToStr[mode1] << "-games against team " << t2 << ". Allowed range is [" << min << "," << max <<"].";

					std::cout << std::setw(10) << name << std::setw(10) << "" << std::setw(50) << msg1.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
					std::cout << std::setw(20) << "" <<  "Slots: " << printSet(allSlots) << std::endl;
				}
			}
		}
	}
	return c;
}

CA3::CA3 (CType c, int p, std::array<IdList, 2> teamIds1, int min, int max, HomeMode mode1, std::array<IdList, 2> teamIds2, int intp, CMode mode2): Constraint(c,p, "CA3"), min(min), max(max), mode1(mode1), intp(intp), mode2(mode2){
	for (auto id : teamIds1[0]){ teams1.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds1[1]){ teamGroups1.insert(Instance::get()->getTeamGroup(id)); }	
	for (auto id : teamIds2[0]){ teams2.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds2[1]){ teamGroups2.insert(Instance::get()->getTeamGroup(id)); }	
}
CA3::CA3 (CType c, int p) : Constraint(c,p, "CA3"){
	/* Take a random team t, and a random set of three other teams
	 * Denote with kmin (kmax) the smallest (largest) number of games team t plays against T in any sequence of 3 time slots
	 * Team t should play at least kmin and at most kmax times against teams in T in any sequence of 3 time slots
	 */

	if (c == HARD) {
		bool homeAlreadyPresent = false;
		bool awayAlreadyPresent = false;
		for(auto con : Instance::get()->getCnstrs()){
			if (con->getType() != HARD) {
				continue;	
			}
			if (CA3* conCA3 = dynamic_cast<CA3*>(con)) {
				std::cout << "CA3" << std::endl;	
				if (conCA3->mode1 == H) {
					homeAlreadyPresent = true;	
				} else {
					assert(conCA3->mode1 == A);
					awayAlreadyPresent = true;	
				}
			}
		}

		// Randomly choose a mode H or A
		for(auto t : IN->getTeams()) teams1.insert(t.second); 
		for(auto t : IN->getTeams()) teams2.insert(t.second);
		min = 0;
		max = 2;
		intp = 3;
		mode2 = SLOTS;
		
		if (!homeAlreadyPresent && !awayAlreadyPresent) {
			std::uniform_int_distribution<int> randBool(0, 1);
			if (randBool(randEngine)) {
				homeAlreadyPresent = true;
				mode1 = H;	
			} else {
				mode1 = A;
				awayAlreadyPresent = true;
			}
		} else {
			if (!homeAlreadyPresent) {
				mode1 = H;
				homeAlreadyPresent = true;
			} else {
				assert(!awayAlreadyPresent);
				mode1 = A;
				awayAlreadyPresent = true;
			}
		}
	} else {
		// Pick a random team
		Team *t = IN->getRandTeam();
		teams1.insert(t);

		// Determine how many teams in teams2 to draw, |T_2| ∼ Pr(5, 6, 7 | 1 1 1)
		std::vector<int> teamIds;
		for (int i = 0; i < IN->getNrTeams(); ++i) {
			if(i != t->getId()){
				teamIds.push_back(i);
			}	
		}

		// Draw the teams themselves
		teams2 = IN->getRandTeams({5,6,7}, {1,1,1}, teamIds);

		// Randomly select a mode h, a, or ha	
		std::uniform_int_distribution<int> randMode(0, 2);
		const int dummy = randMode(randEngine);
		switch (dummy) {
			case 0:
				mode1 = H;
				break;
			case 1:
				mode1 = A;
				break;
			case 2:
				mode1 = HA;
				break;
		}

		mode2 = SLOTS;
		min = 0;
		max = 2;
		intp = 4;
	}
}
AttrMap CA3::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "CapacityConstraints";
	attrs["Type"] = "CA3";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams1"] = idToString(teams1);
	attrs["teamGroups1"] = idToString(teamGroups1);
	attrs["teams2"] = idToString(teams2);
	attrs["teamGroups2"] = idToString(teamGroups2);
	attrs["min"] = std::to_string(min);
	attrs["max"] = std::to_string(max);
	attrs["mode1"] = HomeModeToStr[mode1];
	attrs["intp"] = std::to_string(intp);
	attrs["mode2"] = CModeToStr[mode2];
	return attrs;
}
ObjCost CA3::checkConstr(){
	/**
	 * Each team in team group T 1 plays at least k min and at most k max { home
	 * games, away games, games } against teams in team group T 2 in each sequence
	 * of k {time slots, games}.
	 * Each team in T 1 triggers a deviation equal to the sum of the number of home
	 * games against teams in T 2 less than k min or more than k max for each sequence
	 * of k {time slots, games}.
	 **/

	ObjCost c = std::make_pair(0,0);
	TeamSet allTeams1 = IN->collectTeams(teams1, teamGroups1);
	TeamSet allTeams2 = IN->collectTeams(teams2, teamGroups2);
	if (mode2 == SLOTS) { // Sequence of slots
		SlotMap slots = IN->getSlots();
		assert(IN->getSlots().size() >= intp);
		for (auto t : allTeams1) {
			for (SlotMapIt it1 = slots.begin(); it1 != std::prev(slots.end(), intp - 1); ++it1) {
				SlotSet period = {it1->second};
				SlotMapIt begin2 = std::next(it1);
				SlotMapIt end2 = std::next(it1, intp);
				for (SlotMapIt it2 = begin2; it2 != end2; ++it2) {
					period.insert(it2->second);
				}
				int nrMeetings = IN->getMeetingsTeamTeamSlot({t}, allTeams2, period, mode1).size();
				int p = std::max(0, nrMeetings-max) + std::max(0, min-nrMeetings);
				if (p != 0) {
					(type == HARD) ? c.first += p*penalty : c.second += p*penalty;
					std::stringstream msg1;
					msg1 << "Team " << std::setw(3) << t->getId() << " has " << std::setw(3) << nrMeetings
							<< " " << std::setw(2) << HomeModeToStr[mode1] << "-games in " << std::setw(2) << intp << " slots. Allowed range is [" << min << "," << max <<"].";
					std::cout << std::setw(10) << name << std::setw(10) << "" << std::setw(50) << msg1.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
					std::cout << std::setw(20) << "" <<  "Slots: " << printSet(period) << std::endl;
				}
			}
		}
	} else { // Sequence of games
		for (auto t : allTeams1) {
			MeetingList allMeetings = IN->getMeetingsTeam({t}, HA);
			allMeetings.sort(compMeetingScheduledSlot);
			// Team neet to play at least kmin games, or constraint is redundant
			assert(allMeetings.size() >= intp);
			MeetingListIt begin = allMeetings.begin();
			MeetingListIt end = std::prev(allMeetings.end(), intp - 1);
			for (MeetingListIt it1 = begin; it1 != end; ++it1) {
				int cntr = 0;
				// Consider next k meetings
				MeetingListIt end2 = std::next(it1, intp);
				Team *homeTeam, *awayTeam;
				for (MeetingListIt it2 = it1; it2 != end2; ++it2) {
					// Count all games against allTeams2
					homeTeam = (*it2)->getFirstTeam();
					awayTeam = (*it2)->getSecondTeam();
					// We also need to check that the other team is not the away team: necessary for case that allTeams2 contains team t
					if((mode1==H || mode1==HA) && awayTeam!=t && allTeams2.count(awayTeam)){
						cntr++;
					}
					if((mode1==A || mode1==HA) && homeTeam!=t && allTeams2.count(homeTeam)){
						cntr++;
					}
				}
				int dev = std::max(0, cntr-max) + std::max(0, min-cntr);
				if (dev > 0) {
					(type == HARD) ? c.first += dev*penalty : c.second += dev*penalty;
					std::stringstream msg;
					msg << "Team " << std::setw(3) << t->getId() << " has " << std::setw(3) << cntr
						<< " consecutive " << std::setw(2) << HomeModeToStr[mode1] << "-games. Series starts in slot " << (*it1)->getAssignedSlot() << ". Allowed range is [" << min << "," << max <<"].";
					std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
				}
			}
		}
	}
	return c;
}

CA4::CA4 (CType c, int p, std::array<IdList, 2> teamIds1, int min, int max, HomeMode mode1, std::array<IdList, 2> teamIds2, GlobMode mode2, std::array<IdList, 2> slotIds): Constraint(c,p, "CA4"), min(min), max(max), mode1(mode1), mode2(mode2){
	for (auto id : teamIds1[0]){ teams1.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds1[1]){ teamGroups1.insert(Instance::get()->getTeamGroup(id)); }	
	for (auto id : teamIds2[0]){ teams2.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds2[1]){ teamGroups2.insert(Instance::get()->getTeamGroup(id)); }	
	for (auto id : slotIds[0]){ slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]){ slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
CA4::CA4 (CType c, int p, std::vector<std::pair<int, int>> teamPairs) : Constraint(c,p, "CA4"){
	/* Take two random teams
	 * Teams have complementary patterns in one randomly chosen time slot in which teams currently have complementary patterns
	 */

	// Randomly determine whether we choose simultaneous games between top teams (70% prob) or complementary patterns (30% prob)
	std::uniform_real_distribution<double> randFloat(0.0,1.0);
	if(randFloat(randEngine) < 0.7 || teamPairs.empty()){
		if (c == HARD) {
			// Uniformly choose a set of |T | ∼ Pr(4, 5, 6 | 1, 1, 1) teams
			std::vector<int> teamIds(IN->getNrTeams());
			std::iota(teamIds.begin(), teamIds.end(), 0);
			teams1 = IN->getRandTeams({4,5,6}, {1,1,1}, teamIds);
			teams2 = TeamSet(teams1);

			// For each time slot, determine the total number of mutual games between teams in T
			std::vector<std::pair<int, int>> mutualGamesSlot;
			for (int s = 0; s < IN->getNrSlots(); ++s) {
				const int nrMeetings = IN->getMeetingsTeamTeamSlot(teams1, teams1, {IN->getSlot(s)}, H).size();
				mutualGamesSlot.push_back(std::make_pair(nrMeetings, s));
			}	
			std::sort(mutualGamesSlot.begin(), mutualGamesSlot.end());

			// The first |T| time slots are know the slots with the fewest number of mutual games
			int totNrMutual = 0;
			for(int s=0; s < teams1.size(); ++s){
				totNrMutual += mutualGamesSlot.at(s).first;
				slots.insert(IN->getSlot(mutualGamesSlot.at(s).second));
			}

			min = 0;
			max = std::max(2, totNrMutual);
			mode1 = H;
			mode2 = GLOBAL;
		} else {
			// Uniformly choose a set of |T | ∼ Pr(4, 5, 6 | 1, 1, 1) teams
			std::vector<int> teamIds(IN->getNrTeams());
			std::iota(teamIds.begin(), teamIds.end(), 0);
			teams1 = IN->getRandTeams({4,5,6}, {1,1,1}, teamIds);	
			teams2 = TeamSet(teams1);

			// Uniformly choose |T| time slots
			slots = IN->getRandSlots({teams1.size()}, {1});

			min = 0;
			max = (int) teams1.size()/2 -1;
			mode1 = H;
			mode2 = GLOBAL;
		}
	} else {
		for(auto t : IN->getTeams()) teams2.insert(t.second); 

		min = 0;
		max = 1;
		mode1 = H;
		mode2 = EVERY;

		std::shuffle(teamPairs.begin(), teamPairs.end(), randEngine);
		teams1.insert(IN->getTeam(teamPairs.back().first));
		teams1.insert(IN->getTeam(teamPairs.back().second));
		teamPairs.pop_back();

		if(c==HARD){
			std::vector<int> complSlots;
			for(int s=0; s < IN->getNrSlots(); ++s){
				if (IN->getMeetingsTeamSlot(teams1, {IN->getSlot(s)}, H).size() <= 1) {
					// Complementary pattern in this slot
					complSlots.push_back(s);
				}
			}
			std::shuffle(complSlots.begin(), complSlots.end(), randEngine);
			slots.insert(IN->getSlot(*complSlots.begin()));
		} else {
			// Complementary slot during a randomly chosen period
			slots = IN->getRandSlots({1},{1});
		}
	}
}
AttrMap CA4::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "CapacityConstraints";
	attrs["Type"] = "CA4";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams1"] = idToString(teams1);
	attrs["teamGroups1"] = idToString(teamGroups1);
	attrs["teams2"] = idToString(teams2);
	attrs["teamGroups2"] = idToString(teamGroups2);
	attrs["min"] = std::to_string(min);
	attrs["max"] = std::to_string(max);
	attrs["mode1"] = HomeModeToStr[mode1];
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	attrs["mode2"] = GlobModeToStr[mode2];
	return attrs;
}
ObjCost CA4::checkConstr(){
	/**
	 * Teams in team group T 1 play at least k min and at most k max {home games, away games,
	 * games} against teams in team group T 2 in {time group, each time slot of time group} S .
	 * Time group S triggers a deviation equal to the number of games with a home team in T 1
	 * and a team in T 2 less than k min or more than k max .
	 **/

	ObjCost c = std::make_pair(0,0);

	TeamSet allTeams1 = IN->collectTeams(teams1, teamGroups1);
	TeamSet allTeams2 = IN->collectTeams(teams2, teamGroups2);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);
	if (mode2 == GLOBAL) {
		int nrMeetings = IN->getMeetingsTeamTeamSlot(allTeams1, allTeams2, allSlots, mode1).size();
		int deviation = std::max(nrMeetings - max, min - nrMeetings);
		if (deviation > 0) {
			(type == HARD) ? c.first += penalty*deviation : c.second += penalty*deviation;
			std::stringstream msg1;
			msg1 << "There are " << nrMeetings << " teams playing " << HomeModeToStr[mode1] << " in given slots. Allowed range is [" << min << "," << max <<"].";
			std::cout << std::setw(10) << name << std::setw(10) << "" << std::setw(50) << msg1.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
			std::cout << std::setw(20) << "" <<  "Teams1: " << printSet(allTeams1) << std::endl;
			std::cout << std::setw(20) << "" <<  "Teams2: " << printSet(allTeams2) << std::endl;
			std::cout << std::setw(20) << "" <<  "Slots: " << printSet(allSlots) << std::endl;
		}
	} else { // EVERY
		for (auto s : allSlots) {
			int nrMeetings = IN->getMeetingsTeamTeamSlot(allTeams1, allTeams2, {s}, mode1).size();
			int deviation = std::max(nrMeetings - max, min - nrMeetings);
			if (deviation > 0) {
				(type == HARD) ? c.first += penalty*deviation : c.second += penalty*deviation;
				std::stringstream msg1;
				msg1 << "There are " << nrMeetings << " teams playing " << HomeModeToStr[mode1] << " in slot " << s->getId() << ". Allowed range is [" << min << "," << max <<"].";
				std::cout << std::setw(10) << name << std::setw(10) << "" << std::setw(50) << msg1.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
				std::cout << std::setw(20) << "" <<  "Teams1: " << printSet(allTeams1) << std::endl;
				std::cout << std::setw(20) << "" <<  "Teams2: " << printSet(allTeams2) << std::endl;
			}
		}
	}	
	return c;
}

CA5::CA5 (CType c, int p, std::array<IdList, 2> teamIds1, int min, int max, std::array<IdList, 2> teamIds2, std::array<IdList, 2> slotIds) : Constraint(c,p, "CA5"), min(min), max(max){
	for (auto id : teamIds1[0]){ teams1.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds1[1]){ teamGroups1.insert(Instance::get()->getTeamGroup(id)); }	
	for (auto id : teamIds2[0]){ teams2.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds2[1]){ teamGroups2.insert(Instance::get()->getTeamGroup(id)); }	
	for (auto id : slotIds[0]){ slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]){ slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
CA5::CA5 (CType c, int p) : Constraint(c,p, "CA5"){
	std::cout << "CA5 random generator not yet initialized!" << std::endl;
	assert ( 2 < 1);
}
AttrMap CA5::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "CapacityConstraints";
	attrs["Type"] = "CA5";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams1"] = idToString(teams1);
	attrs["teamGroups1"] = idToString(teamGroups1);
	attrs["min"] = std::to_string(min);
	attrs["max"] = std::to_string(max);
	attrs["teams2"] = idToString(teams2);
	attrs["teamGroups2"] = idToString(teamGroups2);
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	return attrs;
}
ObjCost CA5::checkConstr(){
	/**
	 * Each team in team group T 1 plays at least k min and at most k max away games
	 * against a team in team group T 2 when it consecutively plays away during time
	 * group S .
	 * Each team in T 1 triggers a deviation equal to the sum of the number of away
	 * games against teams in T 2 less than k min or more than k max for each sequence
	 * of consecutive away games in S.
	 **/

	TeamSet allTeams1 = IN->collectTeams(teams1, teamGroups1);
	TeamSet allTeams2 = IN->collectTeams(teams2, teamGroups2);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);
	
	{ // Assumption: time group s contains a series of consecutive time slots
		int prevS=-1;
		for(auto s : allSlots){
			if (prevS != -1) {
				assert(s->getId() == prevS++);
			}
		}
	}

	ObjCost c = std::make_pair(0,0);
	for (auto t : allTeams1) {
		MeetingList allMeetings = IN->getMeetingsTeamSlot({t}, {allSlots}, HA);
		allMeetings.sort(compMeetingScheduledSlot);
		MeetingListIt begin = allMeetings.begin();
		// CONSECUTIVELY AWAY
		MeetingListIt end = std::prev(allMeetings.end(), 1);
		for (MeetingListIt it1 = begin; it1 != end; ++it1) {
			if ((*it1)->getSecondTeam() != t) { // Home game
				continue; // Check next meeting
			} else { // Away game
				MeetingListIt end2 = allMeetings.end();
				int cntr = allTeams2.count((*it1)->getFirstTeam()); // Status first away game
				int nrConsec=1;
				for (MeetingListIt it2 = std::next(it1); it2 != end2; ++it2) {
					if ((*it2)->getSecondTeam() != t) { // Home game
						break; // Break sequence
					} else { // Series of away games
						nrConsec++;
						if (allTeams2.count((*it2)->getFirstTeam())) { // Away game against team in T2
							cntr++;	
						}
					}
				}
				int dev = std::max(0, cntr-max) + std::max(0, min-cntr);
				if (nrConsec > 1 && dev > 0) { // Must be consecutive series
					std::stringstream msg;
					msg << "Team " << std::setw(3) << t->getId() << " plays " << std::setw(3) << cntr
					<< " consecutive away-games against teams in T2. Series starts in slot " << (*it1)->getAssignedSlot() << ". Allowed range is [" << min << "," << max <<"].";
					std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
				}
			}
		}
	}
	return c;
}

GA1::GA1(CType c, int p, std::array<IdList, 2> slotIds, int min, int max, MeetingIdList meetingIds) : Constraint(c,p, "GA1"), min(min), max(max){
	for (auto id : slotIds[0]){ slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]){ slotGroups.insert(Instance::get()->getSlotGroup(id)); }
	//for (auto idPair : meetingIds){ meetings.push_back(Instance::get()->getMeetingsTeamTeam({Instance::get()->getTeam(idPair.first)}, {Instance::get()->getTeam(idPair.second)}, H)); }
	for (auto idPair : meetingIds){ meetings.insert({idPair.first, idPair.second}); }
}
GA1::GA1 (CType c, int p) : Constraint(c,p, "GA1"){
	/* Take a random game (i,j) in a random slot s
	 * Fixed game assignment: (i,j) has to take place in s
	 */

	// Choose |G| \sim Pr(1, 2, 3, 4 | 5, 1, 1, 1)
	const int values[4] = {1,2,3,4};
	std::discrete_distribution<int> randMeetings {5,1,1,1};
	const int nrMeetingsChosen = values[randMeetings(randEngine)];

	// Choose the games themselves
	std::vector<Meeting*> meetingsCopyAll;
	for(auto m : IN->getMeetings()){
		meetingsCopyAll.push_back(m);
	}
	std::shuffle(meetingsCopyAll.begin(), meetingsCopyAll.end(), randEngine);
	for (int i = 0; i < nrMeetingsChosen; ++i) {
		meetings.insert({meetingsCopyAll.at(i)->getFirstTeam()->getId(), meetingsCopyAll.at(i)->getSecondTeam()->getId()});	
	}

	if (c==HARD) {
		// Retrieve all time slots during which none of the games in G are scheduled
		SlotSet slotsUsed;
		for(auto m : meetings){
			slotsUsed.insert((*IN->getMeetingsTeamTeam({IN->getTeam(m.first)}, {IN->getTeam(m.second)}, H).begin())->getAssignedSlot());
		}

		std::uniform_real_distribution<double> randFloat(0.0,1.0);

		if (randFloat(randEngine) < 0.6) {
			// Forbidden slots (60%): choose |G| time slots from P \ S during which at most
			// floor(|G|/2) games in G can be scheduled
			min = 0;
			max = (int) (nrMeetingsChosen/2);

			std::vector<int> slotsFreeId;
			for(auto s : IN->getSlots()){
				if (!slotsUsed.count(s.second)) {
					slotsFreeId.push_back(s.second->getId());	
				}
			}
			std::shuffle(slotsFreeId.begin(), slotsFreeId.end(), randEngine);
			for (int m = 0; m < std::min(meetings.size(), slotsFreeId.size()); ++m) {
				slots.insert(IN->getSlot(slotsFreeId.at(m)));	
			}
		} else {
			// Fixed slots (40%): at least b|G 0 |/2c games from G 0 must be scheduled during S .
			min = (int) ((nrMeetingsChosen+1)/2);
			max = meetings.size();
			slots = slotsUsed;
		}

	} else {
		slots = IN->getRandSlots({1,2,3,4}, {5,1,1,1});
		std::uniform_real_distribution<double> randFloat(0.0,1.0);
		if (randFloat(randEngine) < 0.6) {
			// Forbidden slots (60%): at most floor(|G|/2) games can be scheduled during S.
			min = 0;
			max = (int) (nrMeetingsChosen/2);
		} else {
			// Fixed slots (40%): at least floor(|G|/2) games from G must be scheduled during S.
			min = (int) ((nrMeetingsChosen+1)/2);
			max = meetings.size();
		}
	}

}
AttrMap GA1::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "GameConstraints";
	attrs["Type"] = "GA1";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	attrs["min"] = std::to_string(min);
	attrs["max"] = std::to_string(max);
	// Serialize games string
	std::string str = "";
	for(auto meeting : meetings){
		str += std::to_string(meeting.first) + "," + std::to_string(meeting.second)  + ";";
	}
	attrs["meetings"] = str;
	return attrs;
}
ObjCost GA1::checkConstr(){
	/**
	 * Time group S hosts at least k min and at most k max games in G = { ( i 1 , j 1 ) ,
	 * ( i 2 , j 2 ) , ... } .
	 * Time group S triggers a deviation equal to the number of games in G less
	 * than k min or more than k max .
	 **/
	ObjCost c = std::make_pair(0,0);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);
	MeetingList scheduledMeetings = IN->getMeetingsSlot(allSlots);
	
	int cntr = 0;
	for(auto m : scheduledMeetings){
		if (meetings.count(std::make_pair(m->getFirstTeam()->getId(), m->getSecondTeam()->getId()))) { cntr++; }
	}
	int dev = std::max(cntr - max, min - cntr);
	if (dev > 0) {
		std::stringstream msg;
		msg << "Total of " << cntr << " scheduled games from " << printIntPairSet(meetings) <<". Allowed range [" << min << "," << max << "].";
		(type == HARD) ? c.first += penalty*dev : c.second += penalty*dev;
		std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
		std::cout << std::setw(10) << "Time slots: " << printSet(allSlots) << std::endl;
	}

	return c;
}

GA2::GA2(CType c, int p, std::array<IdList, 2> teamIds1, HomeMode mode1, std::array<IdList, 2> teamIds2, std::array<IdList, 2> slotIds1, std::array<IdList, 2> teamIds3, CompareMode mode2, HomeMode mode3, std::array<IdList, 2> teamIds4, std::array<IdList, 2> slotIds2) : Constraint(c,p, "GA2"), hMode1(mode1), cMode(mode2), hMode2(mode3) {
	for (auto id : teamIds1[0]) { teams1.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds1[1]) { teamGroups1.insert(Instance::get()->getTeamGroup(id)); }

	for (auto id : teamIds2[0]) { teams2.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds2[1]) { teamGroups2.insert(Instance::get()->getTeamGroup(id)); }

	for (auto id : teamIds3[0]) { teams3.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds3[1]) { teamGroups3.insert(Instance::get()->getTeamGroup(id)); }

	for (auto id : teamIds4[0]) { teams4.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds4[1]) { teamGroups4.insert(Instance::get()->getTeamGroup(id)); }

	for (auto id : slotIds1[0]) { slots1.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds1[1]) { slotGroups1.insert(Instance::get()->getSlotGroup(id)); }

	for (auto id : slotIds2[0]) { slots2.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds2[1]) { slotGroups2.insert(Instance::get()->getSlotGroup(id)); }
}
GA2::GA2 (CType c, int p) : Constraint(c,p, "GA2"){
	std::cout << "GA2 random generator not yet initialized!" << std::endl;
	assert ( 2 < 1);
}
AttrMap GA2::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "GameConstraints";
	attrs["Type"] = "GA2";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams1"] = idToString(teams1);
	attrs["teamGroups1"] = idToString(teamGroups1);
	attrs["mode1"] = HomeModeToStr[hMode1];
	attrs["teams2"] = idToString(teams1);
	attrs["teamGroups2"] = idToString(teamGroups2);
	attrs["slots1"] = idToString(slots1);
	attrs["slotGroups1"] = idToString(slotGroups1);
	attrs["teams3"] = idToString(teams3);
	attrs["teamGroups3"] = idToString(teamGroups3);
	attrs["mode2"] = CompareModeToStr[cMode];
	attrs["mode3"] = HomeModeToStr[hMode2];
	attrs["teams4"] = idToString(teams4);
	attrs["teamGroups4"] = idToString(teamGroups4);
	attrs["slots2"] = idToString(slots2);
	attrs["slotGroups2"] = idToString(slotGroups2);
	return attrs;
}
ObjCost GA2::checkConstr(){
	/**
	 * If a team from team group T1 plays a { home game, game } against a team in team group T2 in time group S 1 , then
	 * a team from team group T3 { plays, does not play } a { home game, game } against a team in team group T3 in time
	 * group S 2 .
	 * Time group S 2 triggers a deviation of 1 if i plays a home game against j in
	 * S 1 but k does not play a home game against l in S 2 .
	 **/
	assert(cMode == EQ || cMode == NEQ);
	ObjCost c = std::make_pair(0,0);
	
	SlotSet allSlots1 = IN->collectSlots(slots1, slotGroups1);
	SlotSet allSlots2 = IN->collectSlots(slots2, slotGroups2);

	TeamSet allTeams1 = IN->collectTeams(teams1, teamGroups1);
	TeamSet allTeams2 = IN->collectTeams(teams2, teamGroups2);
	TeamSet allTeams3 = IN->collectTeams(teams3, teamGroups3);
	TeamSet allTeams4 = IN->collectTeams(teams4, teamGroups4);

	assert(cMode == EQ || cMode == NEQ);

	if (IN->getMeetingsTeamTeamSlot(allTeams1, allTeams2, allSlots1, hMode1).size() > 0) { // i plays against j in S1
		int cntr = IN->getMeetingsTeamTeamSlot(allTeams3, allTeams4, slots2, hMode2).size();
		if (cMode == EQ && cntr == 0) {
			std::stringstream msg;
			msg << "A team from T1 plays a game against a team from T2 in slot " << printSet(allSlots1) << " but no team from T3 plays against a team from T4 in slot " << printSet(slots2);
			(type == HARD) ? c.first += penalty : c.second += penalty;
			std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
		} else if(cMode == NEQ && cntr > 0){
			std::stringstream msg;
			msg << "A team from T1 plays a game against a team from T2 in slot " << printSet(allSlots1) << " a team from T3 plays at least one game against a team from T4 in slot " << printSet(slots2);
			(type == HARD) ? c.first += penalty : c.second += penalty;
			std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
			std::cout << "T1: " << printSet(allTeams1) << std::endl;
			std::cout << "T2: " << printSet(allTeams2) << std::endl;
			std::cout << "T3: " << printSet(allTeams3) << std::endl;
			std::cout << "T4: " << printSet(allTeams4) << std::endl;
		}
	}
	return c;
}

BR1::BR1(CType c, int p, std::array<IdList, 2> teamIds, int intp, CompareMode mod1, HomeMode mod2, std::array<IdList, 2> slotIds) : Constraint(c,p, "BR1"), intp(intp), cMode(mod1), hMode(mod2) {
	for (auto id : teamIds[0]) { teams.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds[1]) { teamGroups.insert(Instance::get()->getTeamGroup(id)); }
	for (auto id : slotIds[0]) { slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]) { slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
BR1::BR1 (CType c, int p) : Constraint(c,p, "BR1"){

	// Choose a random team
	Team* t = IN->getRandTeam();
	teams = {t};


	if (c==HARD) {
		std::vector<int> slotIdsBreak;
		std::vector<int> slotIdsNoBreak;

		// Get all the games of the team
		MeetingList meetings = IN->getMeetingsTeam({t}, HA);
		meetings.sort(compMeetingScheduledSlot);

		// Loop over all breaks
		int br = 0;
		HomeMode m1;
		HomeMode m2 = HA; // Make sure the first game does not result in a break

		for (auto m : meetings) {
			m1 = m2; 	// Game mode of previous period
			m2 = (m->getFirstTeam() == t) ? H : A;
			if (m1 == m2) { 
				// Break with correct home mode occuring in one of the specified slots
				slotIdsBreak.push_back(m->getAssignedSlot()->getId());
			} else {
				// Break with correct home mode occuring in one of the specified slots
				slotIdsNoBreak.push_back(m->getAssignedSlot()->getId());
			}
		}

		std::shuffle(slotIdsBreak.begin(), slotIdsBreak.end(), randEngine);
		std::shuffle(slotIdsNoBreak.begin(), slotIdsNoBreak.end(), randEngine);

		// Choose how many time slots we take
		const int valuesSlot[4] = {1,3,6};
		std::discrete_distribution<int> randNrSlot {1,1,1};
		const int nrSlotsChosen = valuesSlot[randNrSlot(randEngine)];
		intp = std::min((int) nrSlotsChosen/3, (int) slotIdsNoBreak.size());
		for (int i = 0; i < nrSlotsChosen; ++i) {
			if (i < intp) {
				slots.insert(IN->getSlot(slotIdsBreak.at(i)));
			} else {
				slots.insert(IN->getSlot(slotIdsNoBreak.at(i-intp)));
			}
		} 

		cMode = LEQ;
		hMode = HA;


	} else {
		// Choose |S| \sim Pr(1, 3, 6 | 1, 1, 1)
		slots = IN->getRandSlots({1,3,6}, {1,1,1});
		intp = (int) slots.size()/3;
		hMode = HA;
		cMode = LEQ;
	}

}
AttrMap BR1::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "BreakConstraints";
	attrs["Type"] = "BR1";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["intp"] = std::to_string(intp);
	attrs["mode1"] = CompareModeToStr[cMode];
	attrs["mode2"] = HomeModeToStr[hMode];
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	return attrs;
}
ObjCost BR1::checkConstr(){
	/**
	 * Each team in team group T has at least k min and at most k max { regular,
	 * generalized } { home breaks, away breaks, breaks } in time group S .
	 * Each team in T triggers a deviation equal to the number of regular home
	 * breaks in S less than k min P or more than k max.
	 **/
	ObjCost c = std::make_pair(0,0);
	TeamSet allTeams = IN->collectTeams(teams, teamGroups);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);
	for (auto t : allTeams) {
		// Get all the games of the team
		MeetingList meetings = IN->getMeetingsTeam({t}, HA);
		meetings.sort(compMeetingScheduledSlot);

		// Loop over all breaks
		int br = 0;
		HomeMode m1;
		HomeMode m2 = HA; // Make sure the first game does not result in a break

		for (auto m : meetings) {
			m1 = m2; 	// Game mode of previous period
			m2 = (m->getFirstTeam() == t) ? H : A;
			if (m1 == m2 && (m2 == hMode || hMode==HA) && allSlots.count(m->getAssignedSlot())) { 
				// Break with correct home mode occuring in one of the specified slots
				br++; 
			}
		}

		int dev = 0;
		std::stringstream msg1;
		switch (cMode) {
			case EQ:
				if (br != intp) {
					dev = std::abs(br - intp);
					msg1 << "Team " << t->getId() << " has " << br << " " << HomeModeToStr[hMode] << " breaks in given period. Must be equal to " << intp <<".";
				}	
				break;
			case LEQ:
				if (br > intp) {
					dev = br - intp;
					msg1 << "Team " << t->getId() << " has " << br << " " << HomeModeToStr[hMode] << " breaks in given period. Must be smaller than " << intp <<".";
				}
				break;
			default:
				std::cout << "ERROR in BR1 evaluaton. Mode should be EQ or LEQ."  << std::endl;
				abort();
				
		}
		if (dev > 0) {
			(type == HARD) ? c.first += penalty*dev : c.second += penalty*dev;
			std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg1.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
		}
	}	

	return c;
}

BR2::BR2(CType c, int p, std::array<IdList, 2> teamIds, CompareMode mod2, int intp, std::array<IdList, 2> slotIds) : Constraint(c,p, "BR2"), cMode(mod2), intp(intp) {
	for (auto id : teamIds[0]) { teams.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds[1]) { teamGroups.insert(Instance::get()->getTeamGroup(id)); }
	for (auto id : slotIds[0]) { slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]) { slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
BR2::BR2 (CType c, int p) : Constraint(c,p, "BR2"){
	if (c==HARD) {
		std::cout << "Hard" << std::endl;
		// the total number of breaks in the competition is at most the total number of
		// breaks in the partial timetable
		
		// Count the total number of breaks
		int totNrBreaks = 0;
		for (int t = 0; t < IN->getNrTeams(); ++t) {
			// Loop over all breaks
			HomeMode m1;
			HomeMode m2 = HA; // Make sure the first game does not result in a break

			// Get all the games of the team
			MeetingList meetings = IN->getMeetingsTeam({IN->getTeam(t)}, HA);
			meetings.sort(compMeetingScheduledSlot);

			for (auto m : meetings) {
				m1 = m2; 	// Game mode of previous period
				m2 = (m->getFirstTeam()->getId() == t) ? H : A;
				if (m1 == m2) { 
					// Break with correct home mode occuring in one of the specified slots
					totNrBreaks++; 
				}
			}	

		}

		//bMode = REGULAR;
		for(auto t : IN->getTeams()) teams.insert(t.second);
		cMode = LEQ;
		intp = totNrBreaks;
		for(auto s : IN->getSlots()) slots.insert(s.second);
	} else {
		std::cout << "Soft" << std::endl;
		// the total number of breaks in the competition is at most |U|-2
		//bMode = REGULAR;
		for(auto t : IN->getTeams()) teams.insert(t.second);
		cMode = LEQ;
		intp = IN->getNrTeams()-2;
		for(auto s : IN->getSlots()) slots.insert(s.second);
	}
}
AttrMap BR2::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "BreakConstraints";
	attrs["Type"] = "BR2";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	//attrs["mode1"] = BreakModeToStr[bMode];
	attrs["homeMode"] = HomeModeToStr[hMode];
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["mode2"] = CompareModeToStr[cMode];
	attrs["intp"] = std::to_string(intp);
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	return attrs;
}
ObjCost BR2::checkConstr(){
	/**
	 * The sum over all { regular, generalized } breaks of teams in team group T is
	 * { exactly, no more than } k in time group S.
	 * Team group T triggers a deviation equal to the difference in the sum of regular
	 * breaks in S and k .
	 **/
	ObjCost c = std::make_pair(0,0);
	TeamSet allTeams = IN->collectTeams(teams, teamGroups);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);
	int nrBreaks = 0;
	for (auto t : allTeams) {
		// Get all the games of the team
		MeetingList meetings = IN->getMeetingsTeam({t}, HA);
		meetings.sort(compMeetingScheduledSlot);
		// Sum over all breaks
		HomeMode m1;
		HomeMode m2 = HA; // Make sure the first game does not result in a break
		for (auto m : meetings) {
			m1 = m2; // Game mode of previous period
			m2 = (m->getFirstTeam() == t) ? H : A;
			// Need to loop over all meetings: slot set might be non-continuous
			if (m1 == m2 && allSlots.count(m->getAssignedSlot())) { nrBreaks++; }	
		}
	}
	int dev = 0;
	std::stringstream msg1;
	switch (cMode) {
		case EQ:
			if (nrBreaks != intp) {
				dev = std::abs(nrBreaks - intp);
				msg1 << "Total number of breaks is " << nrBreaks << ". Must be equal to " << intp <<".";			
			}	
			break;
		case LEQ:
			if (nrBreaks > intp) {
				dev = nrBreaks - intp;
				msg1 << "Total number of breaks in is " << nrBreaks << ". Must be smaller than " << intp <<".";			
			}
			break;
		default:
			std::cout << "ERROR in BR4 evaluaton. Mode should be EQ or LEQ."  << std::endl;
			abort();
			
	}
	if (dev > 0) {	
		(type == HARD) ? c.first += penalty*dev : c.second += penalty*dev;
		std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg1.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
		std::set<int> leagueSet {};
	}
	return c;
}

BR3::BR3(CType c, int p, std::array<IdList, 2> teamIds, BreakMode mod1, HomeMode mod2, int intp) : Constraint(c,p, "BR3"), bMode(mod1), hMode(mod2), intp(intp) {
	for (auto id : teamIds[0]) { teams.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds[1]) { teamGroups.insert(Instance::get()->getTeamGroup(id)); }
}
BR3::BR3 (CType c, int p) : Constraint(c,p, "BR3"){
	std::cout << "BR3 random generator not yet initialized!" << std::endl;
	assert ( 2 < 1);
}
AttrMap BR3::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "BreakConstraints";
	attrs["Type"] = "BR3";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["mode1"] = BreakModeToStr[bMode];
	attrs["mode2"] = HomeModeToStr[hMode];
	attrs["intp"] = std::to_string(intp);
	return attrs;
}
ObjCost BR3::checkConstr(){
	ObjCost c = std::make_pair(0,0);
	return c;
}

BR4::BR4(CType c, int p, CompareMode m, std::array<IdList, 2> teamIds, int min, std::array<IdList, 2> slotIds) : Constraint(c,p, "BR4"), cMode(m), min(min) {
	for (auto id : teamIds[0]) { teams.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds[1]) { teamGroups.insert(Instance::get()->getTeamGroup(id)); }
	for (auto id : slotIds[0]) { slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]) { slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
BR4::BR4 (CType c, int p) : Constraint(c,p, "BR4"){
	std::cout << "BR4 random generator not yet initialized!" << std::endl;
	assert ( 2 < 1);
}
AttrMap BR4::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "BreakConstraints";
	attrs["Type"] = "BR4";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["mode"] = CompareModeToStr[cMode];
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["min"] = std::to_string(min);
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	return attrs;
}
ObjCost BR4::checkConstr(){
	ObjCost c = std::make_pair(0,0);
	return c;
}

FA1::FA1(CType c, int p, std::array<IdList, 2> teamIds, int intp, std::array<IdList, 2> slotIds) : Constraint(c,p, "FA1"), intp(intp){
	for (auto id : teamIds[0]) { teams.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds[1]) { teamGroups.insert(Instance::get()->getTeamGroup(id)); }
	for (auto id : slotIds[0]) { slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]) { slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
FA1::FA1 (CType c, int p) : Constraint(c,p, "FA1"){
	std::cout << "FA1 random generator not yet initialized!" << std::endl;
	assert ( 2 < 1);
}
AttrMap FA1::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "FairnessConstraints";
	attrs["Type"] = "FA1";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["intp"] = std::to_string(intp);
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	return attrs;
}
ObjCost FA1::checkConstr(){
	/**
	 * Each team in team group T has a difference in played home and away games
	   that is smaller than k during each time slot in S.
	 * Each team in T triggers a deviation equal to the largest difference in played
	   home and away games more than k over all time slots in S .
	 **/
	ObjCost c = std::make_pair(0,0);
	TeamSet allTeams = IN->collectTeams(teams, teamGroups);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);
	for(auto t : allTeams){
		MeetingList meetings = IN->getMeetingsTeam({t}, HA);
		if (meetings.size() == 0) {
			continue;
		}
		meetings.sort(compMeetingScheduledSlot);

		// Strategy: set the difference, and look one meeting ahead
		// If next meeting has same home advanatge, then wait (difference will further increase)
		// If next meeting has different home advantage: check whether difference is unacceptable
		int difference = 1; // After the first round, the difference is always one
		int maxDifference = -1, maxSlot=-1;
		bool home = ((*meetings.begin())->getFirstTeam() == t); // Home advantage of first meeting
		bool nextHome;
		const MeetingListIt start = meetings.begin();
		const MeetingListIt end = std::prev(meetings.end());
		const MeetingListIt penultimate = std::prev(end);
		for(MeetingListIt it = start; it != end; ++it){
			nextHome = ((*std::next(it))->getFirstTeam() == t);
			if (home != nextHome) {
				difference = 0;
			} else { // Same home advantage
				difference++;
				if (difference > maxDifference && allSlots.count((*std::next(it))->getAssignedSlot())) { // New largest difference on one of the slots in slot set
					maxDifference = difference;
					maxSlot = (*std::next(it))->getAssignedSlot()->getId();
				}
			} 
			home = nextHome;
		}
		if (maxDifference > intp) {
			(type == HARD) ? c.first += penalty*maxDifference : c.second += penalty*maxDifference;
			std::stringstream msg;
			msg << "K-balancedness-index for team " << t->getId() << " is " << maxDifference << " in period "  << maxSlot << ".";
			std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
		}
	}
	return c;
}

FA2::FA2(CType c, int p, std::array<IdList, 2> teamIds, HomeMode mode, int intp, std::array<IdList, 2> slotIds) : Constraint(c,p, "FA2"), hMode(mode), intp(intp) {
	for (auto id : teamIds[0]) { teams.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds[1]) { teamGroups.insert(Instance::get()->getTeamGroup(id)); }
	for (auto id : slotIds[0]) { slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]) { slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
FA2::FA2 (CType c, int p) : Constraint(c,p, "FA2"){
	// The difference in home games played between any two teams in U is not larger than 2 at any point in time.
	for(auto t : IN->getTeams()) teams.insert(t.second); 
	hMode = H;
	intp = 2;
	for(auto s : IN->getSlots()) slots.insert(s.second);
}
AttrMap FA2::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "FairnessConstraints";
	attrs["Type"] = "FA2";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["mode"] = HomeModeToStr[hMode];
	attrs["intp"] = std::to_string(intp);
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);

	return attrs;
}
ObjCost FA2::checkConstr(){
	/**
	 * Each team pairing in team group T has a difference in played { home games,
	 * away games, games } that is smaller than k during each time slot in S .
	 * Each team pairing in T triggers a deviation equal to the largest difference in
	 * played home games more than k over all time slots in S .
	 **/

	ObjCost c = std::make_pair(0,0);
	TeamSet allTeams = IN->collectTeams(teams, teamGroups);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);

	// Querying games of teams is rather expensive. First IN->collect number of teams played per team, thereafter compare
	// Step 1: IN->collect games played per team per slot
	int nrRows = allTeams.size();
	int nrCols = IN->getNrSlots();

	// TODO TODO TODO
	// For now we assume that all teams and all slots are involved
	// Otherwise, change datastructure to map
	if (nrRows != IN->getNrTeams() || nrCols != IN->getNrSlots()) {
		std::abort();
	}

	int** gamesPlayed = new int*[nrRows];
	for (int i = 0; i < nrRows; ++i) {	
		gamesPlayed[i] = new int[nrCols]; 
	}
	for (auto t : allTeams) {
		int prevSlotId = -1;
		int nrPlayed = 0;
		int tId = t->getId();
		MeetingList meetings = IN->getMeetingsTeam({t}, hMode);	
		meetings.sort(compMeetingScheduledSlot);
		for (auto m : meetings) {
			int slotId = m->getAssignedSlot()->getId();
			for (int i = prevSlotId+1; i < slotId; ++i) {
				gamesPlayed[tId][i] = nrPlayed;
			}
			gamesPlayed[tId][slotId] = ++nrPlayed;
			prevSlotId = slotId;
		}
		// Fill in slots after last game
		for (int i = prevSlotId; i < nrCols; ++i) {
			gamesPlayed[tId][i] = nrPlayed;
		}	
	}

	// Step 2: calculate games played difference index per team
	
	// For any pair of teams: penalize the solution with the MAXIMAL difference in games played
	for (int j = 0; j < nrRows - 1; ++j) {
		for (int k = j + 1; k < nrRows; ++k) {
			int maxDifference = 0;
			int period = -1;
			for(auto s : allSlots){ // For all specified slots
				int i = s->getId();
				int difference = std::abs(gamesPlayed[j][i] - gamesPlayed[k][i]);
				if(difference > maxDifference){
					maxDifference = difference;
					period = i;
				}
			}
			if (maxDifference > intp) {
				(type == HARD) ? c.first += penalty*(maxDifference-intp) : c.second += penalty*(maxDifference-intp);
				std::stringstream msg;
				msg << HomeModeToStr[hMode] << "-ranking-balancedness-index between team " << j << " and team " << k << " has a value of " << maxDifference << " in slot " << period << ". Max value is " << intp << ".";
				std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
			}
		}
	}
	
	
	/* Obsolete: wrongly used code in ITC2020
	// We penalize the solution with the difference in games played during any period!
	for(auto s : allSlots){ // For all specified slots
		int i = s->getId();
		// For any pair of teams: check whether the difference is greater than max
		for (int j = 0; j < nrRows - 1; ++j) {
			for (int k = j + 1; k < nrRows; ++k) {
				int difference = std::abs(gamesPlayed[j][i] - gamesPlayed[k][i]);
				if (difference > intp) {
					(type == HARD) ? c.first += penalty*(difference-intp) : c.second += penalty*(difference-intp);
					std::stringstream msg;
					msg << HomeModeToStr[hMode] << "-ranking-balancedness-index between team " << j << " and team " << k << " in slot " << i << " has a value of " << difference << " in slot " << i << ". Max value is " << intp << ".";
					std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
				}
			}
		}
	}
	*/
	
	// Step 3: clean memory
	for (int i = 0; i < nrRows; ++i){
	    delete [] gamesPlayed[i];
	}
	delete [] gamesPlayed;
	
	return c;
}

FA3::FA3(CType c, int p, std::array<IdList, 2> teamIds) : Constraint(c,p, "FA3") {
	for (auto id : teamIds[0]) { teams.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds[1]) { teamGroups.insert(Instance::get()->getTeamGroup(id)); }
}
FA3::FA3 (CType c, int p) : Constraint(c,p, "FA3"){
	std::cout << "FA3 random generator not yet initialized!" << std::endl;
	assert ( 2 < 1);
}
AttrMap FA3::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "FairnessConstraints";
	attrs["Type"] = "FA3";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	return attrs;
}
ObjCost FA3::checkConstr(){
	/**
	 * Each pair of teams in team group T plays each other at home and in turn
	 * away.
	 * Each pair of teams in team group T triggers a deviation equal to the total
	 * number of consecutive mutual games with the same home-away assignment.
	 **/
	ObjCost c = std::make_pair(0,0);
	TeamSet allTeams = IN->collectTeams(teams, teamGroups);
	// Check for each pair whether the games alternate
	TeamSetIt start = allTeams.begin();
	TeamSetIt end1 = std::prev(allTeams.end());
	TeamSetIt end2 = allTeams.end();
	for (TeamSetIt it1 = start; it1 != end1; ++it1) {
		for (TeamSetIt it2 = std::next(it1); it2 != end2; ++it2) {
			MeetingList meetings = IN->getMeetingsTeamTeam({*it1}, {*it2}, HA);
			meetings.sort(compMeetingScheduledSlot);		
			Team* t1 = 0;
			int difference = 0;
			for (auto m : meetings) {
				Team* t2 = m->getSecondTeam();
				if (t1 == t2) { difference++; }	
			}
			if (difference > 0) {
				(type == HARD) ? c.first += penalty*difference : c.second += penalty*difference;
				std::stringstream msg;
				msg << "Team " << (*it1)->getId() << " and team " << (*it2)->getId() << " do not play alternately." << difference << " times violated.";
				std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
			}
		}	
	}	

	return c;
}

FA4::FA4(CType c, int p, std::array<IdList, 2> teamIds, int intp) : Constraint(c,p, "FA4"), intp(intp) {
	for (auto id : teamIds[0]) { teams.insert(Instance::get()->getTeam(id)); }
	for (auto id : teamIds[1]) { teamGroups.insert(Instance::get()->getTeamGroup(id)); }
}
FA4::FA4 (CType c, int p) : Constraint(c,p, "FA4"){
	std::cout << "FA4 random generator not yet initialized!" << std::endl;
	assert ( 2 < 1);
}
AttrMap FA4::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "FairnessConstraints";
	attrs["Type"] = "FA4";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["intp"] = std::to_string(intp);
	return attrs;
}
ObjCost FA4::checkConstr(){
	ObjCost c = std::make_pair(0,0);
	return c;
}

FA5::FA5(CType c, int p, std::array<IdList, 2> teamIds, std::array<IdList, 2> slotIds, int intp) : Constraint(c,p, "FA5"), intp(intp) {
	for (auto id : teamIds[0]){ teams.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds[1]){ teamGroups.insert(Instance::get()->getTeamGroup(id)); }	
	for (auto id : slotIds[0]){ slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]){ slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
AttrMap FA5::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "FairnessConstraints";
	attrs["Type"] = "FA5";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	attrs["intp"] = std::to_string(intp);
	return attrs;
}
FA5::FA5 (CType c, int p) : Constraint(c,p, "FA5"){
	std::cout << "FA5 random generator not yet initialized!" << std::endl;
	assert ( 2 < 1);
}
ObjCost FA5::checkConstr(){
	/**
	 * The total distance traveled by all teams in team group T during time group
	 * S is less than k .
	 * Team group T triggers a penalty equal to the total distance traveled more
	 * than k .
	 **/
	ObjCost c = std::make_pair(0,0);

	TeamSet allTeams = IN->collectTeams(teams, teamGroups);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);
	int dist = 0;
	for (auto t: allTeams) {
		dist += Instance::get()->distance(t, allSlots);
	}
	if (dist > intp) {
		std::stringstream msg;
		msg << "Total distance traveled by team group {" << printSet(allTeams) << "} during time group S is " << dist << ". Max allowed: " << intp;
		std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
	}

	return c;
}


FA6::FA6(CType c, int p, std::array<IdList, 2> slotIds, int intp) : Constraint(c,p, "FA6"), intp(intp) {
	for (auto id : slotIds[0]){ slots.insert(Instance::get()->getSlot(id)); }
	for (auto id : slotIds[1]){ slotGroups.insert(Instance::get()->getSlotGroup(id)); }
}
FA6::FA6 (CType c, int p) : Constraint(c,p, "FA6"){
	std::cout << "FA6 random generator not yet initialized!" << std::endl;
	assert ( 2 < 1);
}
AttrMap FA6::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "FairnessConstraints";
	attrs["Type"] = "FA6";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["slots"] = idToString(slots);
	attrs["slotGroups"] = idToString(slotGroups);
	attrs["intp"] = std::to_string(intp);
	return attrs;
}
ObjCost FA6::checkConstr(){
	/**
	 * The total cost associated with all games played during time group S is less
	 * than k.
	 * Time group S triggers a penalty equal to the total cost more than k .
	 **/
	ObjCost c = std::make_pair(0,0);
	SlotSet allSlots = IN->collectSlots(slots, slotGroups);
	int totalCost=0;
	for (auto m : IN->getMeetingsSlot(allSlots)) {
		int cost = IN->getCost(m->getFirstTeam(), m->getSecondTeam(), m->getAssignedSlot());
		totalCost += cost;
	}
	int difference = totalCost - intp;
	if (difference > 0) {
		(type == HARD) ? c.first += penalty*difference : c.second += penalty*difference;
		std::stringstream msg;
		msg << "Total cost of all games on given time slots is " << totalCost << ". Allowed: " << intp << ".";
		std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
		std::cout << "Slots: " << printSet(allSlots) << std::endl;
	}
	return c;
}

SE1::SE1(CType c, int p, std::array<IdList, 2> teamIds, int min) : Constraint(c,p, "SE1"), min(min) {
	for (auto id : teamIds[0]){ teams.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds[1]){ teamGroups.insert(Instance::get()->getTeamGroup(id)); }	
}
SE1::SE1 (CType c, int p) : Constraint(c,p, "SE1"){
	// Each pair of teams has at least 10 time slots between its mutual games
	for(auto t : IN->getTeams()) teams.insert(t.second); 
	min=10;
		
}
AttrMap SE1::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "SeparationConstraints";
	attrs["Type"] = "SE1";
	attrs["type"] = CTypeToStr[type];
	attrs["penalty"] = std::to_string(penalty);	
	attrs["min"] = std::to_string(min);
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["mode1"] = CModeToStr[mode1];
	return attrs;
}
ObjCost SE1::checkConstr(){
	/**
	 * 	Each team pairing in team group T has at least k time slots between two 
	   	consecutive mutual games.
	 *	Each team pairing in T triggers a deviation equal to the sum of the number
		of time slots less than k for all consecutive mutual games.
	**/
	ObjCost c = std::make_pair(0,0);
	TeamSet allTeams = IN->collectTeams(teams, teamGroups);
	TeamSetIt start = allTeams.begin();
	TeamSetIt end1 = std::prev(allTeams.end());
	TeamSetIt end2 = allTeams.end();
	for(TeamSetIt it1 = start; it1 != end1; ++it1){
		for (TeamSetIt it2 = std::next(it1); it2 != end2; ++it2) {
			MeetingList meetings = IN->getMeetingsTeamTeam({*it1}, {*it2}, HA);
			if (meetings.size() < 2) { continue; }
			meetings.sort(compMeetingScheduledSlot);
			int slotId1 = (*meetings.begin())->getAssignedSlot()->getId();
			MeetingListIt start = std::next(meetings.begin());
			MeetingListIt end = meetings.end();
			for (MeetingListIt it = start; it != end; ++it) {
				int slotId2 = (*it)->getAssignedSlot()->getId();
				int difference = slotId2 - slotId1 - 1; // BETWEEN two games
				difference = std::max(0, min - difference);
				if (difference > 0) {
					(type == HARD) ? c.first += penalty*difference : c.second += penalty*difference;
					std::stringstream msg;
					msg << "Team " << (*it1)->getId() << " and team " << (*it2)->getId() << " meet each other in slot " << slotId1 << " and " << slotId2 << ". Min distance is " << min;
					std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;
				}
				slotId1 = slotId2;
			}
		}
	}
	return c;
}

SE2::SE2(CType c, int p, std::array<IdList, 2> teamIds, SlotPairList slotPairings) : Constraint(c,p, "SE2"), slotPairings(slotPairings){
	for (auto id : teamIds[0]){ teams.insert(Instance::get()->getTeam(id)); }	
	for (auto id : teamIds[1]){ teamGroups.insert(Instance::get()->getTeamGroup(id)); }	
}
SE2::SE2 (CType c, int p) : Constraint(c,p, "SE2"){
	std::cout << "SE2 random generator not yet initialized!" << std::endl;
	assert ( 2 < 1);
}
AttrMap SE2::serialize(){
	AttrMap attrs;
	attrs["ClassType"] = "SeparationConstraints";
	attrs["Type"] = "SE2";
	attrs["type"] = CTypeToStr[type];
	attrs["teams"] = idToString(teams);
	attrs["teamGroups"] = idToString(teamGroups);
	attrs["penalty"] = std::to_string(penalty);	
	// Serialize slot pairings string
	std::string str = "";
	for(auto slotPair : slotPairings){
		str += std::to_string(slotPair.first) + "," + std::to_string(slotPair.second)  + ";";
	}
	attrs["slotPairs"] = str;
	return attrs;
}
ObjCost SE2::checkConstr(){
	/**
	 * If a team pairing in team group T meets in one time slot of a time slot pairing
	 * in Q = {{s 1 , s 2 }, {s 3 , s 4 }, . . . } , it also meets in the other time slot.
	 * Each time slot pairing in Q triggers a deviation equal to the number of team
	 * pairings that play in one P time slot but not in the other time slot.
	 **/

	ObjCost c = std::make_pair(0,0);

	TeamSet allTeams = IN->collectTeams(teams, teamGroups);
	TeamSetIt start = allTeams.begin();
	TeamSetIt end1 = std::prev(allTeams.end());
	TeamSetIt end2 = allTeams.end();

	for(auto pair : slotPairings){
		int slotId1 = pair.first;
		int slotId2 = pair.second;
		for(TeamSetIt it1 = start; it1 != end1; ++it1){
			for (TeamSetIt it2 = std::next(it1); it2 != end2; ++it2) { // For each team and slot pairing
				int nrMeetings1 = IN->getMeetingsTeamTeamSlot({*it1}, {*it2}, {IN->getSlot(slotId1)}, HA).size();
				int nrMeetings2 = IN->getMeetingsTeamTeamSlot({*it1}, {*it2}, {IN->getSlot(slotId2)}, HA).size();
				if (nrMeetings1 != nrMeetings2) {
					(type == HARD) ? c.first += penalty : c.second += penalty;
					std::stringstream msg;
					msg << "Team " << (*it1)->getId() << " and team " << (*it2)->getId() << " meet each other in slot " << (nrMeetings1 == 1 ? slotId1 : slotId2) << " but not in " << (nrMeetings1 == 1 ? slotId2 : slotId1) << ".";
					std::cout << std::setw(10) << name << std::setw(10) << " " << std::setw(50) << msg.str() << std::setw(10) << c.first << std::setw(10) << c.second << std::endl;	
				}
			}
		}
	}

	return c;
}
