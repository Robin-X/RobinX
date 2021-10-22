#include "ClassInstance.h"

// Allocating and initializing Instance static data member.
// Lazy initialization: the pointer is being allocated, not the object itself.
Instance *Instance::iInstance = 0;

Instance::~Instance(){
	// Free all memory
	
	// Teams
	for (TeamMapIt it = teams.begin(); it != teams.end();) {
		it->second->free();
		it = teams.erase(it);		
	}	
	teams.empty();

	// Team Groups
	for (TeamGroupMapIt it = teamGroups.begin(); it != teamGroups.end();) {
		delete it->second;
		it = teamGroups.erase(it);		
	}	

	// Slots
	for (SlotMapIt it = slots.begin(); it != slots.end();) {
		delete it->second;
		it = slots.erase(it);		
	}	

	// Slot Groups
	for (SlotGroupMapIt it = slotGroups.begin(); it != slotGroups.end();) {
		delete it->second;
		it = slotGroups.erase(it);		
	}	

	// League Groups
	for(LeagueGroupMapIt it = leagueGroups.begin(); it != leagueGroups.end();){
		delete it->second;
		it = leagueGroups.erase(it);
	}

	// Leagues
	for (LeagueMapIt it = leagues.begin(); it != leagues.end();) {
		delete it->second;
		it = leagues.erase(it);		
	}	

	// Meetings
	for (MeetingListIt it = meetings.begin(); it != meetings.end(); ++it) {
		delete *it;
	}

	// Constraints
	for (ConstraintListIt it = constraints.begin(); it != constraints.end(); ++it) {
		delete *it;	
	}	
}

void Instance::addLeague(League* l){
	// Check whether league already in league map
	try{
		if (leagues.count(l->getId())) {
			std::stringstream msg;
			msg << "Failled to add league " << l->getId() << " to league map. Id already present.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
		else{
			leagues[l->getId()] = l;
		}
		// Add league to members of its league groups
		for (auto lg:l->getLeagueGroups()){
			lg->addMember(l);
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
		// We are not able to store the object: delete it
		delete l;
	}
}

void Instance::addLeagueGroup(LeagueGroup* lg){
	// Check whether slot group already in team map
	try{
		if (leagueGroups.count(lg->getId())) {
			std::stringstream msg;
			msg << "Failled to add league group " << lg->getId() << " to league group map. Id already present.";
			throw_line_robinx (InterfaceReadingException, msg.str());
		}
		else{
			leagueGroups[lg->getId()] = lg;
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
		// We are not able to store the object: delete it
		delete lg;
	}
}

void Instance::addSlot(Slot* s){
	try{
	// Check whether slot already in slot map
		if (slots.count(s->getId())) {
			std::stringstream msg;
			msg << "Failled to add slot " << s->getId() << " to slot map. Id already present.";
			throw_line_robinx (InterfaceReadingException, msg.str());
		}
		else{
			slots[s->getId()] = s;
		}
		// Add slot to members of its slot groups
		for (auto sg:s->getSlotGroups()){
			sg->addMember(s);
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
		// We are not able to store the object: delete it
		delete s;
	}
}

void Instance::addSlotGroup(SlotGroup* sg){
	// Check whether slot group already in team map
	try{
		if (slotGroups.count(sg->getId())) {
			std::stringstream msg;
			msg << "Failled to add slot group " << sg->getId() << " to slot group map. Id already present.";
			throw_line_robinx (InterfaceReadingException, msg.str());
		}
		else{
			slotGroups[sg->getId()] = sg;
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
		// We are not able to store the object: delete it
		delete sg;
	}
}

void Instance::addTeam(Team* t){
	try{
		// Check whether team already in team map
		if (teams.count(t->getId())) {
			std::stringstream msg;
			msg << "Failled to add team " << t->getId() << " to team map. Id already present.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
		else{
			teams[t->getId()] = t;
		}
		// Add team to members of its team groups
		for (auto tg:t->getTeamGroups()){
			tg->addMember(t);
		}
		// Add team to memnbers of league
		t->getLeague()->addMember(t);
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
		// We are not able to store the object: delete it
		t->free();
	}
}

void Instance::addTeamGroup(TeamGroup* tg){
	// Check whether team already in team map
	try{
		if (teamGroups.count(tg->getId())) {
			std::stringstream msg;
			msg << "Failled to add team group " << tg->getId() << " to team group map. Id already present.";
			throw_line_robinx (InterfaceReadingException, msg.str());
		}
		else{
			teamGroups[tg->getId()] = tg;
		}
	}
	catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
		// We are not able to store the object: delete it
		delete tg;
	}
}
AttrMapList Instance::serializeDistances(){
	AttrMapList list;
	for (auto d:distances) {
		list.push_back({
			{"ClassType", "Distances"},
			{"Type", "distance"},
			{"team1", std::to_string(d.first.first->getId())},
			{"team2", std::to_string(d.first.second->getId())},
			{"dist", std::to_string(d.second)}
			});		
	}
	return list;
}
AttrMapList Instance::serializeCOEWeights(){
	AttrMapList list;
	for (auto w:COEWeights) {
		list.push_back({
			{"ClassType", "COEWeights"},
			{"Type", "COEWeight"},
			{"team1", std::to_string(w.first.first->getId())},
			{"team2", std::to_string(w.first.second->getId())},
			{"weight", std::to_string(w.second)}
			});		
	}
	return list;
}
AttrMapList Instance::serializeCosts(){
	AttrMapList list;
	for (auto c:costs) {
		list.push_back({
			{"ClassType", "Costs"},
			{"Type", "cost"},
			{"team1", std::to_string(std::get<0>(c.first)->getId())},
			{"team2", std::to_string(std::get<1>(c.first)->getId())},
			{"slot", std::to_string(std::get<2>(c.first)->getId())},
			{"cost", std::to_string(c.second)}
			});		
	}
	return list;
}
// Querry games
MeetingList Instance::getMeetingsTeam(TeamSet teams, HomeMode mode){
	// Return all scheduled home (H), away (HA), all (HA) meetings of teams in the team set
	MeetingList foundMeetings {};
	
	for(auto meeting:meetings){
		if (meeting->getAssignedSlot() == NULL) { continue; }
		Team* t1 = meeting->getFirstTeam();
		Team* t2 = meeting->getSecondTeam();
		
		if (teams.count(t1) && (mode == H || mode == HA) && meeting->getAssignedSlot() != NULL) {
			foundMeetings.push_back(meeting);	
		}
	
		if (teams.count(t2) && (mode == A || mode == HA) && meeting->getAssignedSlot() != NULL) {
			foundMeetings.push_back(meeting);	
		}	
	}	
	return foundMeetings;
}

MeetingList Instance::getMeetingsSlot(SlotSet slots){
	// Return all meetings scheduled in slots of the slot set
	MeetingList foundMeetings {};

	for(auto meeting:meetings){
		if (meeting->getAssignedSlot() == NULL) { continue; }
		Slot* s =  meeting->getAssignedSlot();
		if (slots.count(s)) {
			foundMeetings.push_back(meeting);
		}
	}

	return foundMeetings;
}

MeetingList Instance::getMeetingsTeamSlot(TeamSet teams, SlotSet slots, HomeMode mode){
	// Return all meetings of teams in the team set, scheduled in the slot set
	MeetingList foundMeetings {};
	
	for (auto meeting:meetings){
		if (meeting->getAssignedSlot() == NULL) { continue; }
		Team* t1 = meeting->getFirstTeam();
		Team* t2 = meeting->getSecondTeam();
		Slot* s = meeting->getAssignedSlot();

		if (slots.count(s) && teams.count(t1) && (mode == H || mode == HA) && meeting->getAssignedSlot() != NULL) {
			foundMeetings.push_back(meeting);	
		}
	
		if (slots.count(s) && teams.count(t2) && (mode == A || mode == HA) && meeting->getAssignedSlot() != NULL) {
			foundMeetings.push_back(meeting);	
		}
	}

	return foundMeetings;
}

MeetingList Instance::getMeetingsTeamSlotGroup(TeamSet teams, SlotGroupSet slotGroups, HomeMode mode){
	// Return all meetings of teams in the team set, scheduled in slots belonging to the slotgroup set
	SlotSet slots;	
	for(auto slotGroup:slotGroups){
		// Add all members of slot group to slots
		for (auto slot:slotGroup->getMembers()) {
			slots.insert(slot);	
		}
	}
	return getMeetingsTeamSlot(teams, slots, mode);
}

MeetingList Instance::getMeetingsTeamTeamSlot(TeamSet teams1, TeamSet teams2, SlotSet slots, HomeMode mode) {
	// Return all meetings involving a (home) team from teams1 and an (away) team from teams2 during slots in slot set
	MeetingList foundMeetings {};
	
	for(auto meeting:meetings){
		if (meeting->getAssignedSlot() == NULL) { continue; }
		Team* t1 = meeting->getFirstTeam();
		Team* t2 = meeting->getSecondTeam();
		Slot* s = meeting->getAssignedSlot();

		if (slots.count(s) && (( mode != A && (teams1.count(t1) && teams2.count(t2))) || ( mode != H && (teams1.count(t2) && teams2.count(t1))))) {
			foundMeetings.push_back(meeting);
		}
	}
	return foundMeetings;
}
MeetingList Instance::getMeetingsTeamTeam(TeamSet teams1, TeamSet teams2, HomeMode mode) {
	// Return all meetings involving a (home) team from teams1 and an (away) team from teams2
	MeetingList foundMeetings {};
	
	for(auto meeting:meetings){
		if (meeting->getAssignedSlot() == NULL) { continue; }
		Team* t1 = meeting->getFirstTeam();
		Team* t2 = meeting->getSecondTeam();
		Slot* s = meeting->getAssignedSlot();

		if (( mode != A && (teams1.count(t1) && teams2.count(t2))) || ( mode != H && (teams1.count(t2) && teams2.count(t1)))) {
			foundMeetings.push_back(meeting);
		}
	}
	return foundMeetings;
}

void Instance::addMeeting(Team* t1, Team* t2, bool noHome, int slot){ 
	// noHome is true if the home advantage is undetermined. Otherwise it is assumed that t1 is the home 
	// team and t2 is the away team
	meetings.push_back(new Meeting(t1,t2,noHome, slot)); 
	return;
}
void Instance::generateMeeting(League* l){
	// Generate k round robin between all members of league
	// Round robin even: teams meet each other k/2 times at each other venue
	try{
		try{
			int k = l->getNrRound();
			for (TeamSetIt it1 = l->getFirstMember(); it1 != l->getLastMember(); ++it1) {
				for (TeamSetIt it2 = std::next(it1, 1); it2 != l->getLastMember(); ++it2) {
					for (int i = 0; i < k/2; ++i) {
						addMeeting(*it1, *it2, false, -1);	
						addMeeting(*it2, *it1, false, -1);	
					}
					if (k%2) {
						addMeeting(*it1, *it2, true, -1);
					}
				}	
			}
		}
		catch(std::out_of_range e) {
			std::stringstream msg;
			msg << "Could not create the meetings of league " << l->getId() << ": an out of range error occurred. Are you user that there exists a league with id " << l->getId() << "?\n";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}		
	} catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
	}
}
void Instance::scheduleMeeting(Team* h, Team* a, Slot* s){
	// Find unscheduled meeting between home team h, and away team a
	// By preference, select meeting with determined home advantage
	try{
		Meeting* m = NULL;
		
		for(auto meeting:meetings){
			if (meeting->getAssignedSlot() != NULL) { continue; }
			Team* t1 = meeting->getFirstTeam();
			Team* t2 = meeting->getSecondTeam();
			
			// Case 1: teams match + determined home advantage	
			if (t1 == h && t2 == a && !meeting->getNoHome()) { m = meeting; break; }
			// Case 2: teams match + undetermined home advantage
			if (t1 == h && t2 == a && meeting->getNoHome()) { m = meeting; }
			if (t1 == a && t2 == h && meeting->getNoHome()) { m = meeting; m->swapTeams();  }
		}
		if (m == NULL) { 
			std::stringstream msg;
			msg << "Meeting not scheduled: there is no unscheduled meeting between home team " << h->getId() << " (" << h->getName() << ")" << " and away team " << a->getId() << " (" << a->getName() << ")" <<  std::endl;
			throw_line_robinx(XmlReadingException, msg.str());
		}
		m->setAssignedSlot(s);
	} catch(XmlReadingException e){
		std::cerr << e.what() << std::endl;
	}
}

int Instance::carryOver(League* l){
	// Step 0: create a carry-over effects matrix on the heap
	int nrRows = l->getNrMembers();
	int** COM = new int*[nrRows];
	for (int i = 0; i < nrRows; ++i) {	
		COM[i] = new int[nrRows] {0}; 
	}

	// Step 1: calculate the carry-over effects matrix
	for (auto t : l->getMembers()) {
		MeetingList meetings = getMeetingsTeam({t}, HA);
		if (meetings.size() == 0) {
			continue;
		}
		meetings.sort(compMeetingScheduledSlot);
		MeetingListIt start = std::next(meetings.begin());
		MeetingListIt end = meetings.end();
		Meeting* m0 = *meetings.begin();
		int o1 = (m0->getFirstTeam() == t) ? m0->getSecondTeam()->getId() : m0->getFirstTeam()->getId();
		for(MeetingListIt it = start; it != end; ++it){
			// Determine the opponents of both matches
			int o2 = ((*it)->getFirstTeam() == t) ? (*it)->getSecondTeam()->getId() : (*it)->getFirstTeam()->getId();
			// 01 gives a carry-over to 02
			COM[o1][o2]++;
			o1 = o2;
		}
		// Close the circle: carry-over from last round to the first
		int o2 = (m0->getFirstTeam() == t) ? m0->getSecondTeam()->getId() : m0->getFirstTeam()->getId();
		COM[o1][o2]++;
	}

	// Step 2: calculate the carry-over effects value
	int value = 0;
	for (int i = 0; i < nrRows; ++i) {
		for (int j = 0; j < nrRows; ++j) {
			if (isSetCOEWeight(getTeam(i), getTeam(j))) {
				value += getCOEWeight(getTeam(i), getTeam(j))*COM[i][j]*COM[i][j];		
			} else {
				value += COM[i][j]*COM[i][j];		
			}
		}	
	}

	// Step 3: clean memory
	for (int i = 0; i < nrRows; ++i){
	    delete [] COM[i];
	}
	delete [] COM;
	
	return value;
}

int Instance::distance(Team* t, SlotSet slots){
	int dist = 0;
	// Get list with all matches of t
	MeetingList meetings = getMeetingsTeam({t}, HA);		
	if (meetings.size() == 0) { return 0; }

	// Sort meetings in increasing slot
	meetings.sort(compMeetingScheduledSlot);	
	
	// Assume teams begin in their home city: + dist home --> first match
	if (slots.count((*(meetings.begin()))->getAssignedSlot()) || slots.empty()) {
		dist += getDistance(t, (*(meetings.begin()))->getFirstTeam());
	}

	// Assume teams end in their home city: + dist last match --> home
	if (slots.count((*(std::prev(meetings.end())))->getAssignedSlot()) || slots.empty()) {
		dist += getDistance((*(std::prev(meetings.end())))->getFirstTeam(), t);
	}
	// Calculate distance between all other matches	
	for (MeetingListIt it2 = meetings.begin(); it2 != std::prev(meetings.end()); ++it2) {
		if (slots.count((*(std::next(it2)))->getAssignedSlot()) || slots.empty()) {
			dist += getDistance((*it2)->getFirstTeam(), (*(std::next(it2)))->getFirstTeam());
		}
	}
	return dist;
}

int Instance::breaks(Team* t){
	int br = 0;
	// Get all the games of the team
	MeetingList meetings = getMeetingsTeam({t}, HA);
	meetings.sort(compMeetingScheduledSlot);

	// Sum over all breaks
	HomeMode m1;
	HomeMode m2 = HA; // Make sure the first game does not result in a break
	for (auto m : meetings) {
		m1 = m2; 	// Game mode of previous period
		m2 = (m->getFirstTeam() == t) ? H : A;
		if (m1 == m2) { br++; }
	}
	return br;
}

int Instance::cost(Team* t){
	int cost = 0;
	// Get all the home games of the team
	MeetingList meetings = getMeetingsTeam({t}, H);
	meetings.sort(compMeetingScheduledSlot);

	// Sum over all costs
	for (auto m : meetings) {
		cost += getCost(m->getFirstTeam(), m->getSecondTeam(), m->getAssignedSlot());
	}
	return cost;
}

void Instance::checkConstr(bool silent){
	// Disable cout
	if (silent) {
		std::cout.setstate(std::ios_base::failbit);
	}
	// Check constraints
	ObjCost result = std::make_pair(0,0);
	std::pair<int, int> temp; // Dummy to add two pairs

	// Base constraints
	// Check whether it is mandatory to schedule all games
	bool found = false;
	for (auto c:constraints) {
		if (c->getName().compare("BA1")==0) {
			// Soft constraint!
			found = true;		
			break;
		}
	}
	if (!found) {
		BA1 base1 = BA1(HARD, 1);
		temp = base1.checkConstr();
		result.first += temp.first;
		result.second += temp.second;
	} 

	BA2 base2 = BA2(HARD, 2);
	temp = base2.checkConstr();
	result.first += temp.first;
	result.second += temp.second;

	// Symmetry
	try{
		for (auto l:getLeagues()) {
			const int nrRoundRobin = l.second->getNrRound();
			const int nrTeams = l.second->getNrMembers();
			const int nrSlotsPhase = nrTeams - 1 + nrTeams%2;

			if (l.second->getMode() != NONEGAME) {
				// Symmetric structures only defined for kRR, with k > 1		
				if (nrRoundRobin <= 1) {
					throw_line_robinx(XmlValidationException, "League " + std::to_string(l.second->getId()) + " organizes a symmetric " + std::to_string(nrRoundRobin) + "RR. Symmetry structures only defined for kRR and kBRR with k > 1.");
				}
			} else {
				// No symmetry, nothing to check
				continue;
			}
			for (int r = 0; r < nrRoundRobin-1; ++r) {
				for(auto t1 : l.second->getMembers()){
					for(auto t2 : l.second->getMembers()){
						if (t1 == t2) { continue; }
						switch (l.second->getMode()) {
							case M:
							{
								for(int s = 0; s < nrSlotsPhase; s++){
									MeetingList meetings1 = getMeetingsTeamTeamSlot({t1}, {t2}, {getSlot(s + r*nrSlotsPhase)}, H);
									MeetingList meetings2 = getMeetingsTeamTeamSlot({t1}, {t2}, {getSlot(s + (r+1)*nrSlotsPhase)}, A);
									if (meetings1.size() != meetings2.size()) {
										result.first += 1;
										std::stringstream msg;
										msg << "Game (" <<  t1->getId() << "," << t2->getId() << ") on slot " << s+r*nrSlotsPhase << " and game (" <<  t2->getId() << "," << t1->getId() << ") on slot " << s + (r+1)*nrSlotsPhase;
										std::cout << std::setw(10) << "Mirror violation." << std::setw(10) << "" << std::setw(50) << msg.str() << std::setw(10) << result.first << std::setw(10) << result.second << std::endl;
									}
								}
								break;
							}
							case E:
							{
								for(int s = 0; s < nrSlotsPhase; s++){
									MeetingList meetings1 = getMeetingsTeamTeamSlot({t1}, {t2}, {getSlot(s + r*nrSlotsPhase)}, H);
									MeetingList meetings2;
									if(s == nrSlotsPhase-1){
										// Opponents in last slot of interval correspond to opponets in first slot of next interval
										meetings2 = getMeetingsTeamTeamSlot({t1}, {t2}, {getSlot((r+1)*nrSlotsPhase)}, A);
									} else {
										meetings2 = getMeetingsTeamTeamSlot({t1}, {t2}, {getSlot(s+1+(r+1)*nrSlotsPhase)}, A);
									}
									if (meetings1.size() != meetings2.size()) {
										result.first += 1;
										std::stringstream msg;
										msg << "Game (" <<  t1->getId() << "," << t2->getId() << ") on slot " << s+r*nrSlotsPhase << " and game (" <<  t2->getId() << "," << t1->getId() << ") on slot " << s + (r+1)*nrSlotsPhase;
										std::cout << std::setw(10) << "English violation." << std::setw(10) << "" << std::setw(50) << msg.str() << std::setw(10) << result.first << std::setw(10) << result.second << std::endl;
									}
								}
								break;
							}
							case F:
							{
								for(int s = 0; s < nrSlotsPhase; s++){
									MeetingList meetings1 = getMeetingsTeamTeamSlot({t1}, {t2}, {getSlot(s + r*nrSlotsPhase)}, H);
									MeetingList meetings2;
									if(s == 0){
										// Opponents in the first slot of interval correspond to opponets in last slot of next interval
										meetings2 = getMeetingsTeamTeamSlot({t1}, {t2}, {getSlot((r+2)*nrSlotsPhase-1)}, A);
									} else {
										meetings2 = getMeetingsTeamTeamSlot({t1}, {t2}, {getSlot(s-1+(r+1)*nrSlotsPhase)}, A);
									}
									if (meetings1.size() != meetings2.size()) {
										result.first += 1;
										std::stringstream msg;
										msg << "Game (" <<  t1->getId() << "," << t2->getId() << ") on slot " << s+r*nrSlotsPhase << " and game (" <<  t2->getId() << "," << t1->getId() << ") on slot " << s + (r+1)*nrSlotsPhase;
										std::cout << std::setw(10) << "French violation." << std::setw(10) << "" << std::setw(50) << msg.str() << std::setw(10) << result.first << std::setw(10) << result.second << std::endl;
									}
								}
								break;
							}
							case I:
							{
								for(int s = 0; s < nrSlotsPhase; s++){
									MeetingList meetings1 = getMeetingsTeamTeamSlot({t1}, {t2}, {getSlot(s + r*nrSlotsPhase)}, H);
									MeetingList meetings2 = getMeetingsTeamTeamSlot({t1}, {t2}, {getSlot((r+2)*nrSlotsPhase-s-1)}, A);
									if (meetings1.size() != meetings2.size()) {
										result.first += 1;
										std::stringstream msg;
										msg << "Game (" <<  t1->getId() << "," << t2->getId() << ") on slot " << s+r*nrSlotsPhase << " and game (" <<  t2->getId() << "," << t1->getId() << ") on slot " << s + (r+1)*nrSlotsPhase;
										std::cout << std::setw(10) << "Inverted violation." << std::setw(10) << "" << std::setw(50) << msg.str() << std::setw(10) << result.first << std::setw(10) << result.second << std::endl;
									}
								}
								break;
							}
							case P:
							{
								if (t1 == t2) { continue; }
								SlotSet slots = {};
								for(int s = 0; s < nrSlotsPhase; s++){
									slots.insert(getSlot(s + r*nrSlotsPhase));
								}
								MeetingList meetings = getMeetingsTeamTeamSlot({t1}, {t2}, slots, HA);
								if (meetings.size() != 1) {
									result.first += 1;
										std::stringstream msg;
										msg << "Team " << t1->getId() << " and team " << t2->getId() << " meet " << meetings.size() << " times in phase " << r << " of the tournament. Should be 1.";
										std::cout << std::setw(10) << "Phased violation." << std::setw(10) << "" << std::setw(50) << msg.str() << std::setw(10) << result.first << std::setw(10) << result.second << std::endl;
								}
								break;
							}
						}
					}
				}
			}
		}
	}catch(XmlValidationException e) {
		std::cerr << e.what() << std::endl;	
	}

	for (auto c:constraints) {
		temp = c->checkConstr();
		result.first += temp.first;
		result.second += temp.second;
	}

	// Calculate objective value
	int cost = calculateObj();
	result.second += cost;

	try {
		if (objectiveValue.first != -1 && objectiveValue.first != result.first) {
			throw_line_robinx(XmlValidationException, "Infeasibility value of " + std::to_string(result.first) +  " does not correspond with value from XML file (" + std::to_string(objectiveValue.first) + ").");
		}
	}catch(XmlValidationException e) {
		std::cerr << e.what() << std::endl;	
	}

	try {
		if (objectiveValue.second != -1 && objectiveValue.second != result.second) {
			throw_line_robinx(XmlValidationException, "Objective value of " + std::to_string(result.second) + " does not correspond with value from XML file (" + std::to_string(objectiveValue.second) + ").");
		}		
	}catch(XmlValidationException e) {
		std::cerr << e.what() << std::endl;	
	}	
	try {
		if (lowerBound.first != -1 && lowerBound.first > result.first) {
			throw_line_robinx(XmlValidationException, "Infeasibility value of " + std::to_string(result.first) +  " is lower than infeasibility lowerbound from XML file (" + std::to_string(objectiveValue.first) + ").");
		}
	}catch(XmlValidationException e) {
		std::cerr << e.what() << std::endl;	
	}

	try {
		if (lowerBound.second != -1 && lowerBound.second > result.second) {
			throw_line_robinx(XmlValidationException, "Objective value of " + std::to_string(result.second) + " is lower than objective value lowerbound from XML file (" + std::to_string(objectiveValue.second) + ").");
		}		
	}catch(XmlValidationException e) {
		std::cerr << e.what() << std::endl;	
	}

	objectiveValue = result;

	// Enable cout again
	if(silent){
		std::cout.clear();
	}
	return;
}

int Instance::calculateObj(){
	int obj = 0; // Objective is added to feasibility cost
	switch (objectiveMode) {
		case BM:	
			for (auto t : teams) {
				obj += breaks(t.second);
			}
			break;
		case CO:	
			for (auto l : leagues) {
				obj += carryOver(l.second);
			}
			break;
		case CR:	
			for (auto t : teams) {
				obj += cost(t.second);
			}
			break;
		case TR:	
			for (auto t : teams) {
				obj += distance(t.second);
			}
			break;
		default:
			break;
	}

	return obj;
}

std::string Instance::classify(){
	std::stringstream type;
	for (auto l : getLeagues()) {
		type << l.second->getNrRound() << "RR, " << CompactnessToStr[l.second->getComp()] << ", " << (l.second->getMode() != NONEGAME ? GameModeToStr[l.second->getMode()] : "${\\emptyset}$") << "{\\textpipe}";
	}	
	std::set<std::string> cnstrs;
	if (objectiveMode == SC) { // Additionally mention hard or soft
		for (auto c : getCnstrs()) { 
			if (c->getType() == HARD) {
				cnstrs.insert(c->getName() + "$^\\text{H}$"); 
			} else {
				cnstrs.insert(c->getName() + "$^\\text{S}$"); 
			}
		}
	} else {
		for (auto c : getCnstrs()) { cnstrs.insert(c->getName()); }
	}
	for (auto c : cnstrs) { 
		if (c!=*cnstrs.begin()) { type <<", "; }
		type << c; 
	}
	type << "{\\textpipe}";
	if (objectiveMode == NONEOBJ) { type << "${\\emptyset}$"; }
	else { type << ObjModeToStr[objectiveMode]; }


	return type.str();
}

void Instance::printSolution(){
	// Every row represents a team, every column represents a time slot
	// Cell contains opponent of team: + sign means home game, - sign means away game	
	for(auto t:getTeams()){
		std::cout << "Team " << t.first << "\t";
		for(auto s:getSlots()){
			MeetingList meetings = getMeetingsTeamSlot({t.second}, {s.second}, HA);
			// At most one meeting per team per time slot
			assert(meetings.size() <= 1);
			if (meetings.size() == 0) {
				std::cout << "B\t";
			} else {
				Meeting* m = *(meetings.begin());
				std::cout << (m->getFirstTeam() == t.second ? "+" + std::to_string(m->getSecondTeam()->getId()) : "-" + std::to_string(m->getFirstTeam()->getId())) << "\t";
			}
		}
		std::cout << std::endl;
	}
}

TeamSet Instance::collectTeams(TeamSet teams, TeamGroupSet teamGroups){
	TeamSet set(teams);
	for (auto tg : teamGroups) { 
		for (auto t : tg->getMembers()) { set.insert(t); }
	}
	return set;	
}
SlotSet Instance::collectSlots(SlotSet slots, SlotGroupSet slotGroups){
	SlotSet set(slots);
	for (auto sg : slotGroups) { 
		for (auto s : sg->getMembers()) { set.insert(s); }
	}
	return set;
}


void Instance::clearSchedule(){
	MeetingList meetings = getMeetings();
	for(auto m : meetings){
		m->setAssignedSlot(NULL);
	}
	return;
}

Team* Instance::getRandTeam(){
	// Choose a random team id
	std::uniform_int_distribution<int> randTeam(0, getNrTeams() - 1);
	const int teamId = randTeam(randEngine);
	return getTeam(teamId);
}

Slot* Instance::getRandSlot(){
	// Choose a random slot id
	std::uniform_int_distribution<int> randSlot(0, getNrSlots() - 1);
	const int slotId = randSlot(randEngine);
	return getSlot(slotId);
}

// Get a number of random slots S, with S\sim Pr(values | weights)
SlotSet Instance::getRandSlots(const std::vector<int> values, const std::vector<int> weights){
	assert(values.size() == weights.size());
	// Randomly sample a number of slots |S| \sum Pr(values, weights)
	std::discrete_distribution<int> randNrSlot(weights.begin(), weights.end());
	const int nrSlotsChosen = values[randNrSlot(randEngine)];

	// Choose the time slots themselves
	std::vector<int> slotIds(getNrSlots());
	std::iota (slotIds.begin(), slotIds.end(), 0); 
	std::shuffle(slotIds.begin(), slotIds.end(), randEngine);
	assert(slotIds.size() >= nrSlotsChosen);
	SlotSet slots;
	for(int id=0; id < nrSlotsChosen; ++id){
		slots.insert(getSlot(slotIds.at(id)));
	}

	return slots;
}

// Get a number of random teams T, with T\sim Pr(values | weights) and T\subseteq teamIds
TeamSet Instance::getRandTeams(const std::vector<int> values, const std::vector<int> weights, std::vector<int> teamIds){

	// Randomly sample a number of teams |T| \sum Pr(values, weights)
	std::discrete_distribution<int> randNrTeam(weights.begin(), weights.end());
	const int nrTeamsChosen = std::min((int) teamIds.size(),values[randNrTeam(randEngine)]);

	// Choose the teams themselves
	std::shuffle(teamIds.begin(), teamIds.end(), randEngine);
	assert(teamIds.size() >= nrTeamsChosen);
	TeamSet teams;
	for(int id=0; id < nrTeamsChosen; ++id){
		teams.insert(getTeam(teamIds.at(id)));
	}

	return teams;
}
