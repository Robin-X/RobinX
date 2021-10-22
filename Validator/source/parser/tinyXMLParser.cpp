#include "tinyXMLParser.h"

void TinyParser::readFile(const std::string fileName){
	tinyxml2::XMLError eResult = doc->LoadFile(fileName.c_str());
	XMLCheckResult(eResult);
	return;
}

void TinyParser::readString(const std::string content){
	doc->Parse(content.c_str());
	return;
}

void TinyParser::saveXml(const std::string docName){
	doc->SaveFile(docName.c_str());
	return;
}

int TinyParser::getIntAttr(const tinyxml2::XMLElement* el, std::string attrName){
	int dummy = -1;
	el->QueryIntAttribute(attrName.c_str(), &dummy);	
	return dummy;
}

std::string TinyParser::getStringAttr(const tinyxml2::XMLElement* el, std::string attrName){
	std::string str = "";
	if( el->Attribute(attrName.c_str())) {
		str = el->Attribute(attrName.c_str());
	}
	return str;
}

IdList TinyParser::detokenizeIntString(std::string str){
	int idx = 0;
	std::list<int> numbers;
	std::stringstream ss(str);

	while ( ss >> idx)
	{ // str='1;2;5;..'
		// idx now represent the first undiscovered number
		numbers.push_back(idx);
		// ';' is delimiter	
		if (ss.peek() == ';'){ ss.ignore(); }
		idx = numbers.size() - 1;
	}
	return numbers;
}

MeetingIdList TinyParser::detokenizeMeetings(std::string str){
	int idx = 0;
	MeetingIdList meetingIds;
	int teamId1, teamId2;
	std::stringstream ss(str);

	while ( ss >> idx)
	{ // str='1;2;5;..'
		// idx now represent the first undiscovered number
		teamId1 = idx;
		// ',' is delimiter	between two teams of same game
		if (ss.peek() == ','){ ss.ignore(); }
		ss >> idx;
		teamId2 = idx;
		meetingIds.push_back({teamId1, teamId2});

		// ';' is delimiter	between games
		if (ss.peek() == ';'){ ss.ignore(); }
	}
	return meetingIds;
}

SlotPairList TinyParser::detokenizeSlotPairs(std::string str){
	int idx = 0;
	SlotPairList slotPairs;
	int slotId1, slotId2;
	std::stringstream ss(str);

	while ( ss >> idx)
	{ // str='1;2;5;..'
		// idx now represent the first undiscovered number
		slotId1 = idx;
		// ',' is delimiter	between two teams of same game
		if (ss.peek() == ','){ ss.ignore(); }
		ss >> idx;
		slotId2 = idx;
		slotPairs.push_back({slotId1, slotId2});

		// ';' is delimiter	between games
		if (ss.peek() == ';'){ ss.ignore(); }
	}
	return slotPairs;
}


std::array<IdList, 2> TinyParser::readTeamTags(const tinyxml2::XMLElement* c, int nr){
	std::array<IdList, 2> tags;
	const std::string s = (nr >=0) ? std::to_string(nr) : "";
	tags[0] = detokenizeIntString(getStringAttr(c, "teams"+s));
	tags[1] = detokenizeIntString(getStringAttr(c, "teamGroups"+s));
	return tags;
}
std::array<IdList, 2> TinyParser::readSlotTags(const tinyxml2::XMLElement* c, int nr){
	std::array<IdList, 2> tags;
	const std::string s = (nr >=0) ? std::to_string(nr) : "";
	tags[0] = detokenizeIntString(getStringAttr(c, "slots"+s));
	tags[1] = detokenizeIntString(getStringAttr(c, "slotGroups"+s));
	return tags;
}

void TinyParser::addChildNode(tinyxml2::XMLElement* parentNode, std::string childStr, std::string content){
	tinyxml2::XMLElement* el = doc->NewElement(childStr.c_str());
	el->SetText(content.c_str());
	parentNode->InsertEndChild(el);
}

void TinyParser::deserializeInstance(){
	// Translate XML document into data structures
	try{
		readMetaData();
		readObjectiveFunction();
		readLeagues();
		readSlotGroups();
		readSlots();
		readTeamGroups();
		readTeams();
		readAdditionalGames();
		Interface::get()->generateMeetings();
		readData();
		readConstr(); 		// Should be last due to dependencies on teams etc. within constraints!	
	} catch (const std::exception& e) {                                               	
		std::stringstream msg;
		msg << "Deserialization of instance resulted in unexpected exception: \n" <<  e.what( ) << "\n";                                                    	
		throw_line_robinx(XmlReadingException, msg.str());
	} catch (const XmlValidationException& e) {
		std::stringstream msg;
		msg << "Deserialization of instance resulted in xml validation exception: \n" << e.what() << std::endl;
		throw_line_robinx(XmlReadingException, msg.str());
	} catch (...){
		std::stringstream msg;
		msg << "Unknown exception during parsing." << std::endl;
		throw_line_robinx(XmlReadingException, msg.str());
	}	
}

void TinyParser::deserializeSolution(const bool readIn){
	// Assumes that solution is stored in doc element
	// Translate this solution into data structures
	try{
		if (readIn) {
			// Read instance file
			tinyxml2::XMLElement* el = doc->RootElement()->FirstChildElement("MetaData")->FirstChildElement("InstanceName");	
			Interface::get()->readInstanceXml(el->GetText());
		}

		// Read meta data
		readMetaDataSol();

		// Read games
		readGames();
	} catch (...){
		std::stringstream msg;
		msg << "Unknown exception during parsing." << std::endl;
		throw_line_robinx(XmlReadingException, msg.str());
	}
	return;
}

void TinyParser::readGames(){
	// Get root node and add all matches
	tinyxml2::XMLElement* root = doc->RootElement();	

	tinyxml2::XMLElement* gamesEl = nullptr;
	gamesEl = doc->RootElement()->FirstChildElement("Games");
	if (gamesEl != nullptr) {
		for (const tinyxml2::XMLElement* p = gamesEl->FirstChildElement("ScheduledMatch"); p; p = p->NextSiblingElement("ScheduledMatch")) {
			Interface::get()->scheduleMeeting(getIntAttr(p,"home"), getIntAttr(p,"away"), getIntAttr(p, "slot"));
		}
	}
}

void TinyParser::readMetaDataSol(){
	Interface *f = Interface::get();

	tinyxml2::XMLElement* metaData = nullptr;
	metaData = doc->RootElement()->FirstChildElement("MetaData");

	if (metaData != nullptr) {
		tinyxml2::XMLElement* dummyEl;
		std::string dummy="";

		// Instance Name
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("InstanceName");
		if(dummyEl != nullptr){
			f->addInstanceName(dummyEl->GetText());
		}

		// Solution Name
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("SolutionName");
		if(dummyEl != nullptr){
			f->addSolutionName(dummyEl->GetText());
		}

		// Contributor OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("Contributor");
		if(dummyEl != nullptr && dummyEl->GetText() != nullptr){
			f->addContributor(dummyEl->GetText());
		}

		// Date OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("Date");
		if(dummyEl != nullptr){
			f->addDate(getIntAttr(dummyEl, "day"), getIntAttr(dummyEl, "month"), getIntAttr(dummyEl, "year"));
		}

		// Solution Method OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("SolutionMethod");
		if(dummyEl != nullptr && dummyEl->GetText() != nullptr){
			f->addSolutionMethod(SolutionMethodMap.at(dummyEl->GetText())); 
		}

		// Objective Value OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("ObjectiveValue");
		if (dummyEl != nullptr) { 
			f->addObjectiveValue(getIntAttr(dummyEl, "infeasibility"), getIntAttr(dummyEl, "objective")); 
		}

		// Remarks OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("Remarks");
		if(dummyEl != nullptr && dummyEl->GetText() != nullptr){
			f->addRemarks(dummyEl->GetText()); 
		}
	}
}

void TinyParser::readMetaData() { 
	Interface *f = Interface::get();
	tinyxml2::XMLElement* metaData = nullptr;
        metaData = doc->RootElement()->FirstChildElement("MetaData");	

	if (metaData != nullptr) {
		tinyxml2::XMLElement* dummyEl;
		std::string dummy="";

		// Instance Name
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("InstanceName");
		if(dummyEl != nullptr && dummyEl->GetText() != nullptr){
			f->addInstanceName(dummyEl->GetText());
		}

		// data type OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("DaTaType");
		if(dummyEl != nullptr && dummyEl->GetText() != nullptr){
			f->addDataType(DaTaTypeMap.at(dummyEl->GetText())); 
		}
		
		// contributor OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("Contributor");
		if(dummyEl != nullptr && dummyEl->GetText() != nullptr){
			f->addContributor(dummyEl->GetText()); 
		}

		// Date OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("Date");
		if (dummyEl != nullptr) { 
			f->addDate(getIntAttr(dummyEl, "day"), getIntAttr(dummyEl, "month"), getIntAttr(dummyEl, "year")); 
		}

		// country OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("Country");
		if (dummyEl != nullptr && dummyEl->GetText() != nullptr) { 
			f->addCountry(dummyEl->GetText()); 
		}

		// description OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("Description");
		if(dummyEl != nullptr && dummyEl->GetText() != nullptr){
			f->addDescription(dummyEl->GetText()); 
		}

		// remarks OPTIONAL
		dummyEl = nullptr;
		dummyEl = metaData->FirstChildElement("Remarks");
		if(dummyEl != nullptr && dummyEl->GetText() != nullptr){
			f->addRemarks(dummyEl->GetText()); 
		}
	}
}

void TinyParser::readData() {
	readDistances();
	readCOEWeights();
	readCosts();
}

void TinyParser::readCOEWeights() {
	// Get root data element
	tinyxml2::XMLElement* data = nullptr;
        data = doc->RootElement()->FirstChildElement("Data");	

	if(data != nullptr){
		// Load all weights into memory
		for (const tinyxml2::XMLElement* p = data->FirstChildElement("COEWeights")->FirstChildElement("COEWeight"); p; p = p->NextSiblingElement("COEWeight")) {
			Interface::get()->addCOEWeight(getIntAttr(p,"team1"), getIntAttr(p,"team2"), getIntAttr(p,"weight"));
		}
	}
}

void TinyParser::readDistances() {
	// Get root data element
	tinyxml2::XMLElement* data = nullptr;
        data = doc->RootElement()->FirstChildElement("Data");	

	if(data != nullptr){
		// Load all distances into memory
		for (const tinyxml2::XMLElement* p = data->FirstChildElement("Distances")->FirstChildElement("distance"); p; p = p->NextSiblingElement("distance")) {
			Interface::get()->addDistance(getIntAttr(p,"team1"), getIntAttr(p,"team2"), getIntAttr(p,"dist"));
		}
	}
}

void TinyParser::readCosts() {
	// Get root data element
	tinyxml2::XMLElement* data = nullptr;
        data = doc->RootElement()->FirstChildElement("Data");	

	if(data != nullptr){
		// Load all costs into memory
		for (const tinyxml2::XMLElement* p = data->FirstChildElement("Costs")->FirstChildElement("cost"); p; p = p->NextSiblingElement("cost")) {
			Interface::get()->addCost(getIntAttr(p,"team1"), getIntAttr(p,"team2"), getIntAttr(p,"slot"), getIntAttr(p,"cost"));
		}
	}
}

void TinyParser::readObjectiveFunction() {
	// Get objective element
	tinyxml2::XMLElement* obj = doc->RootElement()->FirstChildElement("ObjectiveFunction");	
	Interface::get()->addObjective(ObjModeMap.at(obj->FirstChildElement("Objective")->GetText()));
}

void TinyParser::readLeagues() {
	// Get leagues element
	tinyxml2::XMLElement* leagues = doc->RootElement()->FirstChildElement("Resources")->FirstChildElement("Leagues");	

	// Get structures element
	tinyxml2::XMLElement* formats = doc->RootElement()->FirstChildElement("Structure");	

	// Store format of each league
	std::map<int, int> nrLeagues;
	std::map<int, Compactness> compactness; 
	std::map<int, GameMode> gameMode;	

	for (const tinyxml2::XMLElement* p = formats->FirstChildElement("Format"); p; p = p->NextSiblingElement("Format")) {
		// For which leagues is the format applicable?
		IdList ids = detokenizeIntString(getStringAttr(p, "leagueIds"));

		// Compactness of the leagues: optional
		Compactness c = NONECOM;
		const tinyxml2::XMLElement* dummyEl = p->FirstChildElement("compactness");
		if(dummyEl != nullptr){
			c = CompactnessMap.at(dummyEl->GetText());
		}


		// nrRounds
		int n = -1;
	   	p->FirstChildElement("numberRoundRobin")->QueryIntText(&n);

		// gameMode
		GameMode g;
		dummyEl = nullptr;	
		dummyEl = p->FirstChildElement("gameMode");
		if(dummyEl != nullptr && dummyEl->GetText() != nullptr){
			g = GameModeMap.at(dummyEl->GetText());
		} else { g = NONEGAME; }


		// Add info for all applicable leagues
		for (auto id : ids) {
			try {
				if (nrLeagues.count(id)) {
					std::stringstream msg;
					msg << "Multiple formats defined for league " << id << std::endl;
					throw_line_robinx(XmlReadingException, msg.str());
				}	
			}catch(XmlReadingException e) {
				std::cerr << e.what() << std::endl;	
			}
			nrLeagues[id] = n;
			compactness[id] = c;
			gameMode[id] = g;
		}
	}


	// Load all leagues into memory
	for (const tinyxml2::XMLElement* p = leagues->FirstChildElement("league"); p; p = p->NextSiblingElement("league")) {
		// Read all attributes
		int id = getIntAttr(p, "id");
		std::string name = getStringAttr(p, "name");

		// Add leagues
		try {
			if (!nrLeagues.count(id)) {
				std::stringstream msg;	
				msg << "No format specidied for league " << id << std::endl;
				throw_line_robinx(XmlReadingException, msg.str());
			}
			Interface::get()->addLeague(new class League(id, name, nrLeagues[id], gameMode[id], compactness[id], {}));
		}catch(XmlReadingException e) {
			std::cerr << e.what() << std::endl;	
		}
	}
}
void TinyParser::readSlots() {
	tinyxml2::XMLElement* slots = doc->RootElement()->FirstChildElement("Resources")->FirstChildElement("Slots");	
	// Load all slots into memory
	for (const tinyxml2::XMLElement* p = slots->FirstChildElement("slot"); p; p = p->NextSiblingElement("slot")) {
		// Read all attributes and add slot
		IdList	slotGroupIds = detokenizeIntString(getStringAttr(p, "slotGroup"));
		Interface::get()->addSlot(new Slot(getIntAttr(p,"id"), getStringAttr(p, "name"), slotGroupIds));	
	}
}
void TinyParser::readSlotGroups() {
	tinyxml2::XMLElement* slotGroups = nullptr;
	slotGroups = doc->RootElement()->FirstChildElement("Resources")->FirstChildElement("SlotGroups");	
	if(slotGroups != nullptr){
		// Load all slot groups into memory
		for (const tinyxml2::XMLElement* p = slotGroups->FirstChildElement("slotGroup"); p; p = p->NextSiblingElement("slotGroup")) {
			// Add slot
			Interface::get()->addSlotGroup(new SlotGroup(getIntAttr(p,"id"), getStringAttr(p, "name")));	
		}
	}
}
void TinyParser::readTeams() {
	tinyxml2::XMLElement* teams = doc->RootElement()->FirstChildElement("Resources")->FirstChildElement("Teams");	
	// Load all slot groups into memory
	for (const tinyxml2::XMLElement* p = teams->FirstChildElement("team"); p; p = p->NextSiblingElement("team")) {
		// Add team to team map
		IdList teamGroupIds = detokenizeIntString(getStringAttr(p, "teamGroups"));
		Interface::get()->addTeam(new Team(getIntAttr(p, "id"), getStringAttr(p, "name"), getIntAttr(p, "league"), teamGroupIds));
	}
}
void TinyParser::readAdditionalGames() {
	tinyxml2::XMLElement* games = nullptr;
	games = doc->RootElement()->FirstChildElement("Structure")->FirstChildElement("AdditionalGames");	
	if (games != nullptr) {
		// Load all slot groups into memory
		for (const tinyxml2::XMLElement* p = games->FirstChildElement("game"); p; p = p->NextSiblingElement("game")) {
			// Add game to meetings map
			Interface::get()->addMeeting(getIntAttr(p, "team1"), getIntAttr(p, "team2"), getIntAttr(p, "noHome"));
		}
	}
}


void TinyParser::readTeamGroups() {
	// Get list of all team group nodes
	tinyxml2::XMLElement* teamGroups = nullptr;
	teamGroups = doc->RootElement()->FirstChildElement("Resources")->FirstChildElement("TeamGroups");	

	if(teamGroups != nullptr){
		// Load all team groups into memory
		for (const tinyxml2::XMLElement* p = teamGroups->FirstChildElement("teamGroup"); p; p = p->NextSiblingElement("teamGroup")) {
			// Add teamGroup to teamGroup list
			Interface::get()->addTeamGroup(new TeamGroup(getIntAttr(p, "id"), getStringAttr(p, "name")));	
		}
	}
}
void TinyParser::readConstr() {
	readBaseConstr();
	readCapacityConstr();
	readBreakConstr();
	readGameConstr();
	readFairnessConstr();
	readSeparationConstr();
}

void TinyParser::readBaseConstr(){
	readBA1();
}

void TinyParser::readBA1(){
	// Get list of all base constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("BasicConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("BA1"); c; c = c->NextSiblingElement("BA1")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		Interface::get()->addConstraint(new BA1(t, getIntAttr(c, "penalty")));
	}
}

void TinyParser::readCapacityConstr(){
	readCA1();
	readCA2();
	readCA3();
	readCA4();
	readCA5();
}
void TinyParser::readCA1(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("CapacityConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("CA1"); c; c = c->NextSiblingElement("CA1")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		HomeMode mode = HomeModeMap.at(getStringAttr(c, "mode"));
		Interface::get()->addConstraint(new CA1(t, getIntAttr(c, "penalty"), readTeamTags(c) , getIntAttr(c, "min"), getIntAttr(c, "max"), mode, readSlotTags(c)));
	}
}

void TinyParser::readCA2(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("CapacityConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("CA2"); c; c = c->NextSiblingElement("CA2")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		HomeMode mode1 = HomeModeMap.at(getStringAttr(c, "mode1"));
		GlobMode mode2 = GlobModeMap.at(getStringAttr(c, "mode2"));
		readTeamTags(c,1);
		readTeamTags(c,2);
		readSlotTags(c);
		Interface::get()->addConstraint(new CA2(t, getIntAttr(c, "penalty"), readTeamTags(c,1) , getIntAttr(c, "min"), getIntAttr(c, "max"), mode1, mode2, readTeamTags(c,2), readSlotTags(c)));
	}
}
void TinyParser::readCA3(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("CapacityConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("CA3"); c; c = c->NextSiblingElement("CA3")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		HomeMode mode1 = HomeModeMap.at(getStringAttr(c, "mode1"));
		CMode mode2 = CModeMap.at(getStringAttr(c, "mode2"));
		Interface::get()->addConstraint(new CA3(t, getIntAttr(c, "penalty"), readTeamTags(c,1), getIntAttr(c, "min"), getIntAttr(c, "max"), mode1, readTeamTags(c,2), getIntAttr(c, "intp"), mode2));
	}
}
void TinyParser::readCA4(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("CapacityConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("CA4"); c; c = c->NextSiblingElement("CA4")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		HomeMode mode1 = HomeModeMap.at(getStringAttr(c, "mode1"));
		GlobMode mode2 = GlobModeMap.at(getStringAttr(c, "mode2"));
		Interface::get()->addConstraint(new CA4(t, getIntAttr(c, "penalty"), readTeamTags(c,1), getIntAttr(c, "min"), getIntAttr(c, "max"), mode1, readTeamTags(c,2), mode2, readSlotTags(c)));
	}
}
void TinyParser::readCA5(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("CapacityConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("CA5"); c; c = c->NextSiblingElement("CA5")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		Interface::get()->addConstraint(new CA5(t, getIntAttr(c, "penalty"), readTeamTags(c,1), getIntAttr(c, "min"), getIntAttr(c, "max"), readTeamTags(c,2), readSlotTags(c)));
	}
}
void TinyParser::readGameConstr(){
	readGA1();
	readGA2();
}
void TinyParser::readGA1(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("GameConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("GA1"); c; c = c->NextSiblingElement("GA1")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		MeetingIdList meetingIds = detokenizeMeetings(getStringAttr(c, "meetings").c_str());
		Interface::get()->addConstraint(new class GA1(t, getIntAttr(c, "penalty"), readSlotTags(c), getIntAttr(c, "min"), getIntAttr(c, "max"), meetingIds));
	}
}
void TinyParser::readGA2(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("GameConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("GA2"); c; c = c->NextSiblingElement("GA2")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		HomeMode mode1 = HomeModeMap.at(getStringAttr(c, "mode1"));
		CompareMode mode2 = CompareModeMap.at(getStringAttr(c, "mode2").c_str());
		HomeMode mode3 = HomeModeMap.at(getStringAttr(c, "mode3"));
		Interface::get()->addConstraint(new class GA2(t, getIntAttr(c, "penalty"), readTeamTags(c,1) , mode1, readTeamTags(c,2), readSlotTags(c,1), readTeamTags(c,3), mode2, mode3, readTeamTags(c,4), readSlotTags(c,2)));
	}
}


void TinyParser::readBreakConstr(){
	readBR1();
	readBR2();
	readBR3();
	readBR4();
}
void TinyParser::readBR1(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("BreakConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("BR1"); c; c = c->NextSiblingElement("BR1")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		CompareMode cMode = CompareModeMap.at(getStringAttr(c, "mode1"));
		HomeMode hMode = HomeModeMap.at(getStringAttr(c, "mode2"));
		Interface::get()->addConstraint(new class BR1(t, getIntAttr(c, "penalty"), readTeamTags(c), getIntAttr(c, "intp"), cMode, hMode, readSlotTags(c)));
	}
}
void TinyParser::readBR2(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("BreakConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("BR2"); c; c = c->NextSiblingElement("BR2")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		CompareMode cMode = CompareModeMap.at(getStringAttr(c, "mode2"));
		Interface::get()->addConstraint(new class BR2(t, getIntAttr(c, "penalty"), readTeamTags(c), cMode, getIntAttr(c, "intp"), readSlotTags(c)));
	}
}
void TinyParser::readBR3(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("BreakConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("BR3"); c; c = c->NextSiblingElement("BR3")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		BreakMode bMode = BreakModeMap.at(getStringAttr(c, "mode1"));
		HomeMode hMode = HomeModeMap.at(getStringAttr(c, "mode2"));
		Interface::get()->addConstraint(new class BR3(t, getIntAttr(c, "penalty"), readTeamTags(c), bMode, hMode, getIntAttr(c, "intp")));
	}
}
void TinyParser::readBR4(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("BreakConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("BR4"); c; c = c->NextSiblingElement("BR4")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		CompareMode cMode = CompareModeMap.at(getStringAttr(c, "mode"));
		Interface::get()->addConstraint(new class BR4(t, getIntAttr(c, "penalty"), cMode, readTeamTags(c), getIntAttr(c, "min"), readSlotTags(c)));
	}
}

void TinyParser::readFairnessConstr(){
	readFA1();
	readFA2();
	readFA3();
	readFA4();
	readFA5();
	readFA6();
}
void TinyParser::readFA1(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("FairnessConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("FA1"); c; c = c->NextSiblingElement("FA1")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		Interface::get()->addConstraint(new class FA1(t, getIntAttr(c, "penalty"), readTeamTags(c), getIntAttr(c, "intp"), readSlotTags(c)));
	}
}
void TinyParser::readFA2(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("FairnessConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("FA2"); c; c = c->NextSiblingElement("FA2")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		HomeMode mode = HomeModeMap.at(getStringAttr(c, "mode"));
		Interface::get()->addConstraint(new class FA2(t, getIntAttr(c, "penalty"), readTeamTags(c), mode, getIntAttr(c, "intp"), readSlotTags(c)));
	}
}
void TinyParser::readFA3(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("FairnessConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("FA3"); c; c = c->NextSiblingElement("FA3")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		Interface::get()->addConstraint(new class FA3(t, getIntAttr(c, "penalty"), readTeamTags(c)));
	}
}
void TinyParser::readFA4(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("FairnessConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("FA4"); c; c = c->NextSiblingElement("FA4")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		Interface::get()->addConstraint(new class FA4(t, getIntAttr(c, "penalty"), readTeamTags(c), getIntAttr(c, "intp")));
	}
}
void TinyParser::readFA5(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("FairnessConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("FA5"); c; c = c->NextSiblingElement("FA5")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		Interface::get()->addConstraint(new class FA5(t, getIntAttr(c, "penalty"), readTeamTags(c), readSlotTags(c), getIntAttr(c, "intp")));
	}
}
void TinyParser::readFA6(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("FairnessConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("FA6"); c; c = c->NextSiblingElement("FA6")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		Interface::get()->addConstraint(new class FA6(t, getIntAttr(c, "penalty"), readSlotTags(c), getIntAttr(c, "intp")));
	}
}

void TinyParser::readSeparationConstr(){
	readSE1();
	readSE2();
}
void TinyParser::readSE1(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("SeparationConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("SE1"); c; c = c->NextSiblingElement("SE1")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		Interface::get()->addConstraint(new class SE1(t, getIntAttr(c, "penalty"), readTeamTags(c), getIntAttr(c, "min")));
	}
}
void TinyParser::readSE2(){
	// Get list of all capacity constraints
	tinyxml2::XMLElement* constraints = doc->RootElement()->FirstChildElement("Constraints")->FirstChildElement("SeparationConstraints");	

	// Load all constraints into memory
	for (const tinyxml2::XMLElement* c = constraints->FirstChildElement("SE2"); c; c = c->NextSiblingElement("SE2")) {
		CType t = CTypeMap.at(getStringAttr(c, "type"));
		SlotPairList slotPairs = detokenizeSlotPairs(getStringAttr(c, "slotPairs").c_str());
		Interface::get()->addConstraint(new class SE2(t, getIntAttr(c, "penalty"), readTeamTags(c), slotPairs));
	}
}

void TinyParser::serializeInstance(std::string fileName){
	// Translate data structures into XML

	// Create a new declaration for the document
	doc->InsertFirstChild(doc->NewDeclaration());
	
	// Create root node and append it to document
	tinyxml2::XMLElement* root = doc->NewElement("Instance");
	doc->InsertEndChild(root);

	// Add meta data tags
	tinyxml2::XMLElement* meta = doc->NewElement("MetaData");
	addChildNode(meta, "InstanceName", Instance::get()->getInstanceName());
	addChildNode(meta, "DataType", DataTypeToStr[Instance::get()->getDataType()]);
	addChildNode(meta, "Contributor", Instance::get()->getContributor());
	int day = std::get<0>(Instance::get()->getDate());
	int month = std::get<1>(Instance::get()->getDate());
	int year = std::get<2>(Instance::get()->getDate());
	if (year != -1) { 
		tinyxml2::XMLElement* date = doc->NewElement("Date");
		date->SetAttribute("year", year);
		if (month != -1) { date->SetAttribute("month", month); }
		if (day != -1) { date->SetAttribute("day", day); }
		meta->InsertEndChild(date);
	}
	addChildNode(meta, "Country", Instance::get()->getCountry());
	addChildNode(meta, "Description", Instance::get()->getDescription());
	addChildNode(meta, "Remarks", Instance::get()->getRemarks());
	root->InsertEndChild(meta);

	// Add structure tags
	tinyxml2::XMLElement* struc = doc->NewElement("Structure");

	// Add a format tag for each league
	for (auto i:Instance::get()->getLeagues()) {
		tinyxml2::XMLElement* form = doc->NewElement("Format");
		form->SetAttribute("leagueIds", std::to_string(i.second->getId()).c_str());
		addChildNode(form, "numberRoundRobin", std::to_string(i.second->getNrRound()));
		addChildNode(form, "compactness", CompactnessToStr[i.second->getComp()]);
		addChildNode(form, "gameMode", GameModeToStr[i.second->getMode()]);
		struc->InsertEndChild(form);
	}
	root->InsertEndChild(struc);

	// Check for additional games
	serializeGames();

	// Add objective function tag
	tinyxml2::XMLElement* obj = doc->NewElement("ObjectiveFunction");
	addChildNode(obj, "Objective", ObjModeToStr[Instance::get()->getObjective()]);
	root->InsertEndChild(obj);

	// Add data tags
	tinyxml2::XMLElement* data = doc->NewElement("Data");

	// Pairwise distances
	tinyxml2::XMLElement* distances = doc->NewElement("Distances");
	for (auto i:Instance::get()->serializeDistances()) {
		distances->InsertEndChild(serialize(i));	
	}
	data->InsertEndChild(distances);

	// COE Weights
	tinyxml2::XMLElement* weights = doc->NewElement("COEWeights");
	for (auto i:Instance::get()->serializeCOEWeights()) {
		weights->InsertEndChild(serialize(i));	
	}
	data->InsertEndChild(weights);
	

	// Costs
	tinyxml2::XMLElement* costs = doc->NewElement("Costs");
	for (auto i:Instance::get()->serializeCosts()) {
		costs->InsertEndChild(serialize(i));	
	}
	data->InsertEndChild(costs);
	
	root->InsertEndChild(data);



	// Add resources tag
	tinyxml2::XMLElement* res = doc->NewElement("Resources");

	tinyxml2::XMLElement* leagueGroups = doc->NewElement("LeagueGroups");
	for (auto i:Instance::get()->getLeagueGroups()) {
		leagueGroups->InsertEndChild(serialize(i.second->serialize()));	
	}
	res->InsertEndChild(leagueGroups);

	tinyxml2::XMLElement* leagues = doc->NewElement("Leagues");
	for (auto i:Instance::get()->getLeagues()) {
		leagues->InsertEndChild(serialize(i.second->serialize()));	
	}	
	res->InsertEndChild(leagues);

	tinyxml2::XMLElement* teamGroups = doc->NewElement("TeamGroups");
	for (auto i:Instance::get()->getTeamGroups()) {
		teamGroups->InsertEndChild(serialize(i.second->serialize()));	
	}	
	res->InsertEndChild(teamGroups);

	tinyxml2::XMLElement* teams = doc->NewElement("Teams");
	for (auto i:Instance::get()->getTeams()) {
		teams->InsertEndChild(serialize(i.second->serialize()));	
	}	
	res->InsertEndChild(teams);

	tinyxml2::XMLElement* slotGroups = doc->NewElement("SlotGroups");
	for (auto i:Instance::get()->getSlotGroups()) {
		slotGroups->InsertEndChild(serialize(i.second->serialize()));	
	}		
	res->InsertEndChild(slotGroups);

	tinyxml2::XMLElement* slots = doc->NewElement("Slots");
	for (auto i:Instance::get()->getSlots()) {
		slots->InsertEndChild(serialize(i.second->serialize()));	
	}
	res->InsertEndChild(slots);

	root->InsertEndChild(res);



	// Add constraints tag
	tinyxml2::XMLElement* cons = doc->NewElement("Constraints");
	cons->InsertEndChild(doc->NewElement("BasicConstraints"));
	cons->InsertEndChild(doc->NewElement("CapacityConstraints"));
	cons->InsertEndChild(doc->NewElement("GameConstraints"));
	cons->InsertEndChild(doc->NewElement("BreakConstraints"));
	cons->InsertEndChild(doc->NewElement("FairnessConstraints"));
	cons->InsertEndChild(doc->NewElement("SeparationConstraints"));

	for (auto c : Instance::get()->getCnstrs()) {
		// Query the class constraint
		AttrMap attrs = c->serialize();
		tinyxml2::XMLElement* el = cons->FirstChildElement(attrs.at("ClassType").c_str());
		el->InsertEndChild(serialize(attrs));
	}
	root->InsertEndChild(cons);

	// Save file
	tinyxml2::XMLError err = doc->SaveFile(fileName.c_str());
	XMLCheckResult(err);	
	return;
}

void TinyParser::serializeSolution(std::string fileName){
	// Translate data structures into XML
	
	// Create a new declaration for the document
	doc->InsertFirstChild(doc->NewDeclaration());
	
	// Create root node and append it to document
	tinyxml2::XMLElement* root = doc->NewElement("Solution");
	doc->InsertEndChild(root);

	// Add meta data tags
	tinyxml2::XMLElement* meta = doc->NewElement("MetaData");
	addChildNode(meta, "SolutionName", Instance::get()->getSolutionName());
	addChildNode(meta, "InstanceName", Instance::get()->getInstanceName());
	addChildNode(meta, "Contributor", Instance::get()->getContributor());
	int day = std::get<0>(Instance::get()->getDate());
	int month = std::get<1>(Instance::get()->getDate());
	int year = std::get<2>(Instance::get()->getDate());
	if (year != -1) { 
		tinyxml2::XMLElement* date = doc->NewElement("Date");
		date->SetAttribute("year", year);
		if (month != -1) { date->SetAttribute("month", month); }
		if (day != -1) { date->SetAttribute("day", day); }
		meta->InsertEndChild(date);
	}
	addChildNode(meta, "SolutionMethod",  SolutionMethodToStr[Instance::get()->getSolutionMethod()]);

	// Add objective value
	tinyxml2::XMLElement* obj = doc->NewElement("ObjectiveValue");
	obj->SetAttribute("infeasibility", Instance::get()->getObjectiveValue().first);
	obj->SetAttribute("objective", Instance::get()->getObjectiveValue().second);
	meta->InsertEndChild(obj);

	addChildNode(meta, "Remarks", Instance::get()->getRemarks());

	// Append meta data alement to document tree
	root->InsertEndChild(meta);

	// Save all scheduled games
	tinyxml2::XMLElement* games = doc->NewElement("Games");
	for (auto m:Instance::get()->getMeetings()) {
		if (m->getAssignedSlot() != NULL) {
			tinyxml2::XMLElement* game = doc->NewElement("ScheduledMatch");
			game->SetAttribute("home", m->getFirstTeam()->getId());
			game->SetAttribute("away", m->getSecondTeam()->getId());
			game->SetAttribute("slot", m->getAssignedSlot()->getId());
			games->InsertEndChild(game);
		}
	}
	root->InsertEndChild(games);

	// Save file
	tinyxml2::XMLError err = doc->SaveFile(fileName.c_str());
	XMLCheckResult(err);	
	return;
}
void TinyParser::serializeBound(std::string fileName){
	// Translate data structures into XML
	
	// Create a new declaration for the document
	doc->InsertFirstChild(doc->NewDeclaration());
	
	// Create root node and append it to document
	tinyxml2::XMLElement* root = doc->NewElement("Bound");
	doc->InsertEndChild(root);

	// Add meta data tags
	tinyxml2::XMLElement* meta = doc->NewElement("MetaData");
	addChildNode(meta, "InstanceName", Instance::get()->getInstanceName());
	addChildNode(meta, "Contributor", Instance::get()->getContributor());
	int day = std::get<0>(Instance::get()->getDate());
	int month = std::get<1>(Instance::get()->getDate());
	int year = std::get<2>(Instance::get()->getDate());
	if (year != -1) { 
		tinyxml2::XMLElement* date = doc->NewElement("Date");
		date->SetAttribute("year", year);
		if (month != -1) { date->SetAttribute("month", month); }
		if (day != -1) { date->SetAttribute("day", day); }
		meta->InsertEndChild(date);
	}

	// Add objective value
	tinyxml2::XMLElement* bound = doc->NewElement("LowerBound");
	addChildNode(bound, "Infeasibility", std::to_string(Instance::get()->getLowerBound().first));
	addChildNode(bound, "Objective", std::to_string(Instance::get()->getLowerBound().second));
	meta->InsertEndChild(bound);

	addChildNode(meta, "Remarks", Instance::get()->getRemarks());

	// Append meta data alement to document tree
	root->InsertEndChild(meta);

	// Save file
	tinyxml2::XMLError err = doc->SaveFile(fileName.c_str());
	XMLCheckResult(err);	
	return;
}

tinyxml2::XMLElement* TinyParser::serialize(AttrMap attrs){
	// Create element to be appended
	tinyxml2::XMLElement* el = doc->NewElement(attrs.at("Type").c_str());
	for (AttrMap::iterator it = attrs.begin();  it != attrs.end(); ++it){
		if (it->first == "ClassType" || it->first == "Type") { continue; }
		el->SetAttribute(it->first.c_str(), it->second.c_str());
	}	
	return el;
}
void TinyParser::serializeGames(){
	// A meeting should be added to the additinal games block if:
		// The two teams play in different leagues
		// The two teams meet each other more than prescribed in kRR of league, in
		// this case we add by preference games with noHome = false
	TeamMap teams = Instance::get()->getTeams();
	tinyxml2::XMLElement* struc = doc->RootElement()->FirstChildElement("Structure");
	tinyxml2::XMLElement* addGames = doc->NewElement("AdditionalGames");

	for (TeamMapIt t1 = teams.begin(); t1 != teams.end(); ++t1) {
		for (TeamMapIt t2 = std::next(t1); t2 != teams.end(); ++t2) {
				// Save file
			MeetingList meetings;
			for(auto m:Instance::get()->getMeetings()){
				if ((t1->second == m->getFirstTeam() && t2->second == m->getSecondTeam()) || 
				    (t2->second == m->getFirstTeam() && t1->second == m->getSecondTeam())) {
				    	meetings.push_back(m);
				}
			}	
			if (t1->second->getLeague() != t2->second->getLeague()) {
				for (auto m : meetings) {
					tinyxml2::XMLElement* game = doc->NewElement("game");
					game->SetAttribute("team1", m->getFirstTeam()->getId());
					game->SetAttribute("team2", m->getSecondTeam()->getId());
					game->SetAttribute("noHome", m->getNoHome());
					addGames->InsertEndChild(game);
				}
			} else if ((int) meetings.size() > t1->second->getLeague()->getNrRound()) {
				// By preference: add meetings with noHome = true	
				// Sort meeting list so that these meetings are in front
				meetings.sort(compMeetingNoHome);
				int toAdd = meetings.size() - t1->second->getLeague()->getNrRound();
				int added = 0;
				for (std::list<Meeting*>::reverse_iterator it = meetings.rbegin(); it != meetings.rend(); ++it) {
					if (added++ == toAdd) { break; }
					tinyxml2::XMLElement* game = doc->NewElement("game");
					game->SetAttribute("team1", (*it)->getFirstTeam()->getId());
					game->SetAttribute("team2", (*it)->getSecondTeam()->getId());
					game->SetAttribute("noHome", (*it)->getNoHome());
					addGames->InsertEndChild(game);
				}
			}
		}	
	}

	struc->InsertEndChild(addGames);
}
