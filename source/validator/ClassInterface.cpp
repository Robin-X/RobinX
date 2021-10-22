#include "ClassInterface.h"


// The following headers are needed by the random schedule generation method
#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

// The following headers are needed for Vizing's algorithm
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edge_coloring.hpp>
#include <boost/graph/properties.hpp>

// TODO
// Cplex functionality removed for online github version
// Include CPOptimizer
//#include <ilcp/cp.h>

// Include Cplex
//#include <ilcplex/ilocplex.h>

/// CPLEX 2d variables
//typedef IloArray<IloBoolVarArray> IloBoolVarArray2;
//typedef IloArray<IloBoolVarArray2> IloBoolVarArray3;
//typedef IloArray<IloBoolVarArray3> IloBoolVarArray4;
//typedef IloArray<IloIntVarArray> IloIntVarArray2;
//typedef IloArray<IloIntVarArray2> IloIntVarArray3;



#define INS Instance::get()

// Allocating and initializing Interface static data member.
// Lazy initialization: the pointer is being allocated, not the object itself.
Interface *Interface::fInstance = 0;

// Constructor implementation
Interface::Interface(){
	// If necessary: create an instance object
	Instance::get();
}

Interface::~Interface(){
	// Delete instance object
	INS->reset();
}

void Interface::addDistance(int teamId1, int teamId2, int dist){
	try {
		Team *t1, *t2;
		try{
			t1 = INS->getTeam(teamId1);
			t2 = INS->getTeam(teamId2);
		}catch(std::out_of_range e){
			std::stringstream msg;
			msg << "Team " << teamId1 << " or " << teamId2 << " does not exist: " << e.what() << std::endl;
			std::cout << msg.str() << std::endl;
			throw_line_robinx(InterfaceReadingException, msg.str());
		}		
		// Is the distance already set?
		if (INS->isSetDistance(t1, t2)) {
			std::stringstream msg;
			msg << "Distance between team " << teamId1 << " and " << teamId2 << " is already set.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
		INS->setDistance(t1, t2, dist);	
	}catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
	}
}

void Interface::addCOEWeight(int teamId1, int teamId2, int weight){
	try {
		Team *t1, *t2;
		try{
			t1 = INS->getTeam(teamId1);
			t2 = INS->getTeam(teamId2);
		}catch(std::out_of_range e){
			std::stringstream msg;
			msg << "Team " << teamId1 << " or " << teamId2 << " does not exist: " << e.what() << std::endl;
			std::cout << msg.str() << std::endl;
			throw_line_robinx(InterfaceReadingException, msg.str());
		}		
		// Is the weight already set?
		if (INS->isSetCOEWeight(t1, t2)) {
			std::stringstream msg;
			msg << "COEWeight between team " << teamId1 << " and " << teamId2 << " is already set.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
		INS->setCOEWeight(t1, t2, weight);	
	}catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
	}
}

void Interface::addCost(int teamId1, int teamId2, int slotId, int cost){
	try {
		Team *t1, *t2;
		Slot* s;
		try{
			t1 = INS->getTeam(teamId1);
			t2 = INS->getTeam(teamId2);
			s = INS->getSlot(slotId);
		}catch(std::out_of_range e){
			std::stringstream msg;
			msg << "Team " << teamId1 << " or " << teamId2 << "does not exist: " << e.what() << std::endl;
			throw_line_robinx(InterfaceReadingException, msg.str());
		}		
		// Is the cost already set?
		if (INS->isSetCost(t1, t2, s)) {
			std::stringstream msg;
			msg << "Cost between home team " << teamId1 << ", away team" << teamId2 << " and slot " << slotId << " is already set.";
			throw_line_robinx(InterfaceReadingException, msg.str());
		}
		INS->setCost(t1, t2, s, cost);	
	}catch(std::out_of_range e){
		std::stringstream msg;
		msg << "Team " << teamId1 << " or " << teamId2 << "does not exist: " << e.what() << std::endl;
		throw_line_robinx(InterfaceReadingException, msg.str());
	}catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
	}
}
void Interface::addInstanceName(const std::string name, const std::string schema) { INS->setInstanceName(name, schema); }
void Interface::readInstanceXml(std::string xmlFile, std::string xmlSchema){
	// Initialize parser to read the sol file
	addInstanceName(xmlFile);

	// XML files can be large: put on heap
	TinyParser* p = new TinyParser();
	try {
		// Load xml file into memory
		p->readFile(xmlFile);

		// Try to parse the file
		p->deserializeInstance();	

	} catch(XmlReadingException e) {
		std::cout << e.what() << std::endl;
	}

	// Free the memory	
	delete p;
}

void Interface::readInstanceXmlString(std::string xmlContent, std::string xmlSchema){
	// Initialize parser to read the sol file
	std::string xmlFile = "fromString";
	addInstanceName(xmlFile);

	// XML files can be large: put on heap
	TinyParser* p = new TinyParser();
	try {
		// Load xml file into memory
		p->readString(xmlContent);

		// Try to parse the file
		p->deserializeInstance();	

	} catch(XmlReadingException e) {
		std::cout << e.what() << std::endl;
	}

	// Free the memory	
	delete p;
}

void Interface::addSolutionName(const std::string name, const std::string schema) { INS->setSolutionName(name, schema); }
void Interface::readSolutionXml(std::string xmlFile, std::string xmlSchemaIn, std::string xmlSchemaSol, bool readIn){
	// Initialize parser to read the sol file
	addSolutionName(xmlFile);

	// XML files can be large: put on heap
	TinyParser* p = new TinyParser();
	try {
		// Load xml file into memory
		p->readFile(xmlFile);

		// Try to parse the file
		p->deserializeSolution(readIn);	

	} catch(XmlReadingException e) {
		std::cout << e.what() << std::endl;
	}

	// Free the memory	
	delete p;
}
void Interface::readSolutionXmlString(std::string xmlContent, std::string xmlSchemaIn, std::string xmlSchemaSol, bool readIn){
	// Initialize parser to read the sol file
	std::string xmlFile = "fromString";
	addSolutionName(xmlFile);

	// XML files can be large: put on heap
	TinyParser* p = new TinyParser();
	try {
		// Load xml file into memory
		p->readString(xmlContent);

		// Try to parse the file
		p->deserializeSolution(readIn);	

	} catch(XmlReadingException e) {
		std::cout << e.what() << std::endl;
	}

	// Free the memory	
	delete p;
}
void Interface::writeInstanceXml(std::string xmlFile){
	// XML files can be large: put on heap
	TinyParser* p = new TinyParser();
	try {
		// Load xml file into memory
		p->serializeInstance(xmlFile);

	} catch(XmlReadingException e) {
		std::cout << e.what() << std::endl;
	}

	// Free the memory	
	delete p;
}
void Interface::writeSolutionXml(std::string xmlFile){
	// XML files can be large: put on heap
	TinyParser* p = new TinyParser();
	try {
		// Load xml file into memory
		p->serializeSolution(xmlFile);

	} catch(XmlReadingException e) {
		std::cout << e.what() << std::endl;
	}

	// Free the memory	
	delete p;
}
void Interface::writeBoundXml(std::string xmlFile){
	// XML files can be large: put on heap
	TinyParser* p = new TinyParser();
	try {
		// Load xml file into memory
		p->serializeBound(xmlFile);

	} catch(XmlReadingException e) {
		std::cout << e.what() << std::endl;
	}

	// Free the memory	
	delete p;
}
void Interface::addLeague(League* l) { INS->addLeague(l); }	
void Interface::addLeagueGroup(LeagueGroup* lg) { INS->addLeagueGroup(lg); }	
void Interface::addSlot(Slot* s) { INS->addSlot(s); }	
void Interface::addSlotGroup(SlotGroup* sg) { INS->addSlotGroup(sg); }	
void Interface::addTeam(Team* t) { INS->addTeam(t); }	
void Interface::addTeamGroup(TeamGroup* tg) { INS->addTeamGroup(tg); }	
void Interface::addObjective(const ObjMode o) { INS->setObjective(o); }
void Interface::addObjectiveValue(const int infeasibilityValue, const int feasibilityValue) { INS->setObjectiveValue(std::make_pair(infeasibilityValue, feasibilityValue)); }
void Interface::addConstraint(Constraint* c) { INS->addConstraint(c); }	
void Interface::addMeeting(int teamId1, int teamId2, bool noHome, int slot) { INS->addMeeting(INS->getTeam(teamId1), INS->getTeam(teamId2), noHome, slot); }
void Interface::generateMeetings() { 
	for (auto l:INS->getLeagues()) {
		INS->generateMeeting(l.second); 
	}
}
ObjCost Interface::checkConstr(bool silent) { 
	INS->checkConstr(silent); 
	return INS->getObjectiveValue();
}
std::string Interface::classify(){ return INS->classify(); }
void Interface::scheduleMeeting(int homeId, int awayId, int slotId){ 
	Team *t1, *t2;
	try{
		t1 = INS->getTeam(homeId);
		t2 = INS->getTeam(awayId);
	}catch(std::out_of_range e){
		std::stringstream msg;
		msg << "Error during scheduling game " << homeId << " vs. " << awayId << " on " << slotId << std::endl;
		msg << "Team " << homeId << " or " << awayId << " does not exist: " << e.what() << std::endl;
		std::cout << msg.str() << std::endl;
		throw_line_robinx(InterfaceReadingException, msg.str());
	}		

	Slot *s;
	try{
		s = INS->getSlot(slotId);
	}catch(std::out_of_range e){
		std::stringstream msg;
		msg << "Error during scheduling game " << homeId << " vs. " << awayId << " on " << slotId << std::endl;
		msg << "Slot " << slotId << " does not exist: " << e.what() << std::endl;
		std::cout << msg.str() << std::endl;
		throw_line_robinx(InterfaceReadingException, msg.str());
	}		

	INS->scheduleMeeting(t1, t2, s); 
}

void Interface::addDataType(const DataType d) { INS->setDataType(d); }
void Interface::addContributor(const std::string s) { INS->setContributor(s); }
void Interface::addDate(const int year, const int month, const int day) { INS->setDate(year, month, day); }
void Interface::addCountry(const std::string s) { INS->setCountry(s); }
void Interface::addDescription(const std::string s) { INS->setDescription(s); }
void Interface::addRemarks(const std::string s) { INS->setRemarks(s); }
void Interface::addLowerBound(const int lb_inf, const int lb_feas) { INS->setLowerBound({lb_inf, lb_feas}); }
void Interface::addSolutionMethod(const SolutionMethod s) { INS->setSolutionMethod(s); }

/**
 * Construct a canonical timetable.
 * See: 
 	* de Werra, D. Hansen, P. (Ed.) Scheduling in sports Studies on graphs and discrete programming, North-Holland, 1981, 381-395
	* Schaerf, A. Scheduling Sport Tournaments using Constraint Logic Programming Constraints, 1999, 4, 43-65
 * Each round i (with i = 1,..., nrTeams-1) of the pattern is composed by the following matches:
	* One match:
		[2n,i] if i is even,
	   [i,2n] if i is odd
	* floor(n/2) matches:
		[i + k,i - k] for k = 1, 3,..., 2*floor(n/2) - 1
	* floor((n-1)/2) matches:
		[i - k,i + k] for k = 2, 4,..., 2*floor((n - 1)/2)
 * Interpret numbers modulo 2n-1:
 * Modified canonical pattern: revert orientation last three matches of team 2n
 **/
std::pair<std::vector<std::vector<int>>, std::vector<std::vector<bool>>> Interface::canonicalPattern(const int nrTeams, const int nrRoundRobin, const bool modifiedPattern, const GameMode symmetry){
	// nrTeams = 2n must be even
	assert(nrTeams%2 == 0);
	const int n = nrTeams/2;
	const int nrSlotsPhase = nrTeams - 1 + nrTeams%2;

	std::vector<std::vector<int>> opponentSchedule(nrTeams);
	std::vector<std::vector<bool>> patternSet(nrTeams);
	for (int i = 0; i < nrTeams; ++i) {
		opponentSchedule.at(i) = std::vector<int> (nrRoundRobin*nrSlotsPhase, -1);
		patternSet.at(i) = std::vector<bool> (nrRoundRobin*nrSlotsPhase);	// 0 means home, 1 means away
	}
	
	const int bound1 = 2*((int) (n/2) )-1;
	const int bound2 = 2*((int) ((n-1)/2) );
	for (int r = 1; r <= nrTeams-1; ++r) { // All round of the first part
		bool dummy = (r >= nrTeams - 1 - 3) ? 1 : 0;

		if (r%2 == dummy) {
			opponentSchedule.at(2*n - 1).at(r-1) = r-1;
			patternSet.at(2*n-1).at(r-1) = 0;

			opponentSchedule.at(r-1).at(r-1) = 2*n-1;
			patternSet.at(r-1).at(r-1) = 1;
		} else {
			opponentSchedule.at(r-1).at(r-1) = 2*n-1;
			patternSet.at(r-1).at(r-1) = 0;

			opponentSchedule.at(2*n - 1).at(r-1) = r-1;
			patternSet.at(2*n-1).at(r-1) = 1;
		}

		for (int k = 1; k <= bound1; k+=2) {
			int t1 = (r+k < 2*n) ? r+k : r+k-(2*n-1);
			int t2 = (r-k > 0) ? r-k : r-k+(2*n-1);

			opponentSchedule.at(t1-1).at(r-1) = t2 - 1;
			patternSet.at(t1-1).at(r-1) = 0;

			opponentSchedule.at(t2-1).at(r-1) = t1 - 1;
			patternSet.at(t2-1).at(r-1) = 1;
		}
		for (int k = 2; k <= bound2; k+=2) {
			int t1 = (r-k > 0) ? r-k : r-k+(2*n-1);
			int t2 = (r+k < 2*n) ? r+k : r+k-(2*n-1);

			opponentSchedule.at(t1-1).at(r-1) = t2 - 1;
			patternSet.at(t1-1).at(r-1) = 0;

			opponentSchedule.at(t2-1).at(r-1) = t1 - 1;
			patternSet.at(t2-1).at(r-1) = 1;
		}
	}		

	// Generate the other parts of kRR
	for (int k = 1; k < nrRoundRobin; ++k) {
		switch (symmetry) {
			case NONEGAME:
			case P:
				// If the schedule need to be phased, we generate a mirrored schedule
			case M:
			{
				for (int i = 0; i < nrTeams; ++i) {
					for(int s = 0; s < nrSlotsPhase; ++s){ // Length of each phase
						patternSet.at(i).at(nrSlotsPhase*k+s) = !(patternSet.at(i).at(nrSlotsPhase*(k-1)+s));
						opponentSchedule.at(i).at(nrSlotsPhase*k+s) = opponentSchedule.at(i).at(nrSlotsPhase*(k-1)+s);
					}
				}	
	
				break;
			}
			case I:
			{
				for (int i = 0; i < nrTeams; ++i) {
					for(int s = 0; s < nrSlotsPhase; ++s){ // length of each phase
						patternSet.at(i).at(nrSlotsPhase*k+s) = !(patternSet.at(i).at(nrSlotsPhase*(k)-s-1));
						opponentSchedule.at(i).at(nrSlotsPhase*k+s) = opponentSchedule.at(i).at(nrSlotsPhase*(k)-s-1);
					}
				}
				break;
			}
			case E:
			{
				for (int i = 0; i < nrTeams; ++i) {
					for(int s = 0; s < nrSlotsPhase; ++s){ // length of each phase
						if (s == 0) { // First slot in interval r corresponds to last slot in interval r-1
							patternSet.at(i).at(nrSlotsPhase*k) = !(patternSet.at(i).at(nrSlotsPhase*k - 1));
							opponentSchedule.at(i).at(nrSlotsPhase*k) = opponentSchedule.at(i).at(nrSlotsPhase*k - 1);
						} else { // i-th slot of interval r corresponds to (i-1)-th slot in interval r+1
							patternSet.at(i).at(nrSlotsPhase*k+s) = !(patternSet.at(i).at(nrSlotsPhase*(k-1) + s - 1));
							opponentSchedule.at(i).at(nrSlotsPhase*k+s) = opponentSchedule.at(i).at(nrSlotsPhase*(k-1) + s - 1);
						}					
					}
				}
				break;
			}
			case F:
			{
				for (int i = 0; i < nrTeams; ++i) {
					for(int s = 0; s < nrSlotsPhase; ++s){ // length of each phase
						if (s == nrSlotsPhase-1) { // Last slot interval r is first slot interval r-1
							patternSet.at(i).at(nrSlotsPhase*(k+1)-1) = !(patternSet.at(i).at(nrSlotsPhase*(k-1)));
							opponentSchedule.at(i).at(nrSlotsPhase*(k+1)-1) = opponentSchedule.at(i).at(nrSlotsPhase*(k-1));
						} else { // i-th slot in interval r corresponds to (i+1)-th slot of interval r-1
							patternSet.at(i).at(nrSlotsPhase*k+s) = !(patternSet.at(i).at(nrSlotsPhase*(k-1)+s+1));
							opponentSchedule.at(i).at(nrSlotsPhase*k+s) = opponentSchedule.at(i).at(nrSlotsPhase*(k-1)+s+1);
						}
					}
				}
				break;
			}
			default:
				std::cout << "Game mode not yet implemented" << std::endl;
				assert(2 < 1);	
		}	
	}
	std::cout << "Constructed:" << std::endl;
	for(int i=0; i < opponentSchedule.size(); i++){
		for(int s=0; s < opponentSchedule.at(i).size(); s++){
			std::cout << ((patternSet.at(i).at(s) == 0) ? "+" : "-") <<  opponentSchedule.at(i).at(s) << "\t";
		}
		std::cout << std::endl;
	}
	return std::make_pair(opponentSchedule, patternSet);
}

/**
 * Use Vizing algorithm to generate an opponent schedule for a 1RR
 **/

std::vector<std::vector<int>> vizing1RR(const int nrTeams){
	using namespace boost;
	// nrTeams = 2n must be even
	assert(nrTeams%2 == 0);

	const int nrSlotsPhase = nrTeams - 1 + nrTeams%2;

	// Construct the 1RR for |T|-1 teams via edge coloring
	// See: Costa F. N.; Urrutia, S. & Ribeiro, C. C. An ILS heuristic for the traveling tournament problem with predefined venues Ann. Oper. Res., 2012, 194, 137-150
	const int nrNodes = nrTeams-1; // K(|T|-1)
	const int nrEdges = nrNodes*(nrNodes-1)/2; // Complete graph

	typedef adjacency_list<vecS, vecS, undirectedS, no_property, size_t, no_property> Graph;

	Graph G(nrNodes);
	std::vector<std::pair<int,int>> edges;
	for (int i = 0; i < nrNodes; ++i) {
		for (int j = i+1; j < nrNodes; ++j) {
			edges.push_back(std::make_pair(i,j));
		}	  
	}

	// Shuffle the order of the edges
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle (edges.begin(), edges.end(), std::default_random_engine(seed));


	// Add all edges
	for (auto e : edges) {
		add_edge(e.first, e.second, G);
	}


	size_t colors = edge_coloring(G, boost::get(edge_bundle, G));

	//std::cout << "Colored using " << colors << " colors" << std::endl;
	//for (size_t i = 0; i < edges.size(); i++) {
	//	std::cout << "  " << edges[i].first << " - " << edges[i].second << ": " << G[edge(edges[i].first, edges[i].second, G).first] << std::endl;
	//}

	// Save unused color for each team
	std::vector<std::set<int>>  unusedColors (nrNodes);
	for(int i=0; i < nrNodes; ++i){
		for(int j=0; j < nrNodes; ++j){
			unusedColors.at(i).insert(j);
		}
	}

	std::vector<std::vector<int>> opponentSchedule(nrTeams, std::vector<int> (nrSlotsPhase, -1));
	for (size_t i = 0; i < edges.size(); i++) {
		// Erase from unused color
		unusedColors.at(edges[i].first).erase(G[edge(edges[i].first, edges[i].second, G).first]);	
		unusedColors.at(edges[i].second).erase(G[edge(edges[i].first, edges[i].second, G).first]);	

		// Save opponent schedule
		opponentSchedule.at(edges[i].first).at(G[edge(edges[i].first, edges[i].second, G).first]) = edges[i].second;
		opponentSchedule.at(edges[i].second).at(G[edge(edges[i].first, edges[i].second, G).first]) = edges[i].first;
	}

	// Each team should now have exactly one unused color
	// We can use these colors to schedule all games against the last team
	for (int i = 0; i < nrNodes; ++i) {
		assert(unusedColors.at(i).size() == 1);

		// Save in opponent schedule
		opponentSchedule.at(nrTeams-1).at(*(unusedColors.at(i).begin())) = i;
		opponentSchedule.at(i).at(*(unusedColors.at(i).begin())) = nrTeams-1;
	}

	return opponentSchedule;
}

/**
 * Construct a random timetable.
 * Idea: use a pattern enumeration method
 **/
// TODO Requires cplex: removed for online version
//std::pair<std::vector<std::vector<int>>, std::vector<std::vector<bool>>> Interface::randomPattern(const int nrTeams, const int nrRoundRobin, const GameMode symmetry, const bool max3HomeAway, const bool max5Breaks){
//	// Generate opponent schedule for a 1RR
//	std::vector<std::vector<bool>> patternSet;
//
//	std::vector<std::vector<int>> opponentSchedule;
//
//	// Step 1. Generate patterns
//	std::cout << "Step 1. Generate patterns" << std::endl;
//	std::vector<std::vector<bool>> patterns;
//	{
//	IloEnv env;
//	try{
//		/// Construct CPLEX model
//		IloModel model(env);
//
//		// We only consider instances with an even number of teams
//		assert(nrTeams % 2 == 0);
//
//		// We only consider 2RR
//		assert(nrRoundRobin == 2);
//
//		// Time-constrained 2RR
//		const int nrSlots = nrRoundRobin*(nrTeams-1);
//
//		/// Variable declaration
//		/// 0 if pattern h contains a H in slot s, 1 if an A in slot s
//		IloBoolVarArray h(env, nrSlots);
//
//		/// 1 if the pattern contains a home break in slot s, 0 otherwise
//		IloBoolVarArray bh(env, nrSlots);
//
//		/// 1 if the pattern contains an away break in slot s, 0 otherwise
//		IloBoolVarArray ba(env, nrSlots);
//
//		/// Add constraints
//
//		/// After pattern generation we will add all complementary patterns so to increase the probability of finding a valid pattern set
//		/// Not the case anymore!
//		/// model.add(h[0] == 0);
//
//		// Regulate the total number of home games
//		if (nrRoundRobin % 2 == 0) {
//			// Each team plays exactly half of its games at home/away
//			model.add(IloDistribute(env, IloIntVarArray(env, 2, (nrTeams-1)*(nrRoundRobin)/2, (nrTeams-1)*(nrRoundRobin)/2), IloIntArray(env, 2, 0, 1), h));
//		} else {
//			model.add((nrTeams-1)*(nrRoundRobin-nrRoundRobin%2)/2 <= IloCount(h, 0) <= (nrTeams-1)*(nrRoundRobin+nrRoundRobin%2)/2);
//		}
//
//		// Regulate the break variables
//		for (IloInt s = 1; s < nrSlots; ++s) {
//			model.add(bh[s] == (h[s] == h[s-1] && h[s] == 0));
//			model.add(ba[s] == (h[s] == h[s-1] && h[s] == 1));
//		}
//
//		// Reduce the total number of patterns: limit the total number of breaks per team to 6
//		/// break[t][s] == 1 if team t has a break in period s and s+1, 0 else
//		IloIntVarArray breaks(env);
//		for(IloInt s=1; s < nrSlots; ++s){
//			breaks.add(ba[s]);
//			breaks.add(bh[s]);
//		}
//		if (max5Breaks) {
//			model.add(IloCount(breaks,1) <= 5); 
//		} else {
//			model.add(IloCount(breaks,1) <= 6); 
//		}
//
//
//		// Model symmetry constraints
//		{
//			GameMode modeG = Instance::get()->getLeague(0)->getMode();	
//			const int nrSlotsPhase = nrTeams - 1 + nrTeams%2;
//			if (modeG == M) { // Mirrored scheme
//				assert(nrRoundRobin > 1);
//				for (int r = 0; r < nrRoundRobin-1; ++r) {
//					for(IloInt s = 0; s < nrSlotsPhase; ++s){ // Length of each phase
//						model.add(h[nrSlotsPhase*r+s] != h[nrSlotsPhase*(r+1)+s]);
//					}
//				}
//			} else if (modeG == I){ // Inverted scheme
//				assert(nrRoundRobin > 1);
//				for (int r = 0; r < nrRoundRobin-1; ++r) {
//					for(IloInt s = 0; s < nrSlotsPhase; ++s){ // Length of each phase
//						model.add(h[nrSlotsPhase*r+s] != h[nrSlotsPhase*(r+2)-s-1]);
//					}
//				}
//			} else if (modeG == E){ // English scheme
//				assert(nrRoundRobin > 1);
//				for (int r = 0; r < nrRoundRobin-1; ++r) {
//					for(IloInt s = 0; s < nrSlotsPhase; ++s){ // Length of each phase
//						if (s == nrSlotsPhase - 1) { // Last slot in interval r becomes first slot in interval r+1
//							model.add(h[nrSlotsPhase*r+s] != h[nrSlotsPhase*(r+1)]);
//						} else {
//							model.add(h[nrSlotsPhase*r+s] != h[nrSlotsPhase*(r+1)+s+1]);
//						}
//					}
//				}
//			} else if (modeG == F){
//				assert(nrRoundRobin > 1);
//				for (int r = 0; r < nrRoundRobin-1; ++r) {
//					for(IloInt s = 0; s < nrSlotsPhase; ++s){ // Length of each phase
//						if (s == 0) { // First slot interval r is last slot interval r+1
//							model.add(h[nrSlotsPhase*r+s] != h[nrSlotsPhase*(r+2)-1]);
//						} else {
//							model.add(h[nrSlotsPhase*r+s] != h[nrSlotsPhase*(r+1)+s-1]);
//						}
//					}
//				}
//			} else if (modeG == P) {
//				// Nothing can be done here...
//			}
//		}
//
//		if (max3HomeAway) {
//			// Each team can play at most 3 out of four games at home, and at most 3 out of four games away, and there are exactly nrSlots/2 h's in each pattern
//			assert(Instance::get()->getNrSlots()%2==0);
//			model.add(IloSequence(env, 1, 2, 3, h, IloIntArray(env,1,1),IloIntVarArray(env,1,nrSlots/2,nrSlots/2)));
//		}
//
//
//		/** Solver object **/
//
//		// Build a solver object
//		IloCP cp(model);
//
//		// Disable printing
//		cp.setOut(env.getNullStream());
//		cp.setWarning(env.getNullStream());
//
//		// Set time limit and logging granularity
//		//cp.setParameter(IloCP::TimeLimit, timeLimit);
//		cp.setParameter(IloCP::LogPeriod, 100000);
//
//		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//		cp.setParameter(IloCP::RandomSeed, seed);
//
//		// We set the search parameter equal to MultiPoint
//		// Problem with depth-first: successive patterns barely differ
//		// as they are close in the search tree
//		// Moreover: there are too much patterns possible
//		cp.setParameter(IloCP::SearchType, IloCP::MultiPoint);
//		// We do not want to find duplicate solutions!
//		// Also: use single thread
//		cp.setParameter(IloCP::Workers, 1);
//
//		/** Variable ordering and value selection **/
//		// First we define the different search phases, thereafter we define the order
//
//		// Random patterns: generate as many different patterns as possible
//		IloSearchPhase hVar(env, h, IloSelectRandomVar(env), IloSelectRandomValue(env));
//
//		IloSearchPhaseArray phaseArray(env);	
//		phaseArray.add(hVar);
//		cp.setSearchPhases(phaseArray);
//
//		// Start the search
//		cp.startNewSearch();
//
//		// Generate all patterns
//		bool solFound = false;
//		int solNr = 0;
//		while(cp.next()){
//			solFound = true;
//			patterns.push_back(std::vector<bool> (nrSlots));
//			for (IloInt s = 0; s < nrSlots; ++s) {
//				patterns.at(solNr).at(s) = cp.getValue(h[s]);
//			}
//			++solNr;
//			if (solNr%100 == 0) {
//				std::cout << solNr << std::endl;
//			}
//			if (solNr > 2000) {
//				break;
//			}
//		}
//		std::cout << "Generated " << patterns.size() << " patterns." << std::endl;
//
//		// Stop the engine
//		cp.endSearch();
//		cp.end();	
//
//	} catch (IloException &ex){
//		env.out() << "Unexpected error. Caught " << ex << std::endl;
//
//		// Save the solution
//		//Interface::get()->writeSolutionXml(solName);
//	}
//	env.end();
//	}
//
//
//	// Step 2. Generate a pattern set
//	std::cout << "Step 2. Generate a pattern set" << std::endl;
//	
//	// obtain a time-based seed:
//	bool foundASol = false;
//	while (!foundASol){
//		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//		std::shuffle(patterns.begin(), patterns.end(), std::default_random_engine(seed));
//		std::vector<std::vector<bool>> patternsBis(patterns.begin(), patterns.begin()+500);
//		IloEnv env;
//		try{
//			/// Construct CPLEX model
//			IloModel model(env);
//
//			// We only consider instances with an even number of teams
//			assert(nrTeams % 2 == 0);
//
//			// We only consider 2RR
//			assert(nrRoundRobin == 2);
//
//			// Time-constrained 2RR
//			const int nrSlots = nrRoundRobin*(nrTeams-1);
//
//			// Total number of patterns generated in step 1
//			const int nrPatterns = patternsBis.size();
//
//			/// Variable declaration
//			/// 1 if pattern p is chosen, 0 otherwise
//			IloBoolVarArray x(env, nrPatterns);
//
//			/// Add constraints
//			
//			// Need to select exactly |T| patterns
//			IloExpr sumPatterns(env);
//			for(int p=0; p < nrPatterns;++p){
//				sumPatterns += x[p];
//			}
//			model.add(sumPatterns == nrTeams);
//			
//			// In each time slot, |T|/2 teams play home, and the same number of teams plays away
//			for(IloInt s=0; s < nrSlots; ++s){
//				IloExpr sumHome(env);
//				for(int p=0; p < nrPatterns; ++p){
//					if(patternsBis.at(p).at(s)){
//						sumHome += x[p];
//					}
//				}
//				model.add(sumHome == nrTeams/2); // Half of the teams plays away
//			}
//
//			/** Solver object **/
//
//			// Build a solver object
//			IloCplex cplex(model);
//
//			// Disable printing
//			cplex.setWarning(env.getNullStream());
//			cplex.setOut(env.getNullStream());
//			
//			int nrSet = 0;
//			while(cplex.solve()){
//				std::cout << "Pattern set " << nrSet++ << std::endl;
//
//				// Retrieve solution
//				IloExpr sumChosen(env);
//				patternSet.clear();
//				for (int p = 0; p < nrPatterns; ++p) {
//					if (cplex.getIntValue(x[p]) == 1) {
//						patternSet.push_back(patternsBis[p]);	
//						sumChosen += x[p];
//					}	
//				}
//
//				// Try to construct a timetable
//				IloEnv envInner;
//				try {
//					/// Construct CPLEX model
//					IloModel modelInner(envInner);
//
//					// Variable declaration
//					// x[i][j][s] 1 if home team i plays against away team j in time slot s
//					IloBoolVarArray3 x(envInner, nrTeams);
//					for(IloInt i=0; i < nrTeams; ++i){
//						x[i] = IloBoolVarArray2 (envInner, nrTeams);
//						for(IloInt j=0; j < nrTeams; ++j){
//							x[i][j] = IloBoolVarArray (envInner, nrSlots);
//						}
//					}
//
//					/** Add Constraints **/
//
//					/// Teams cannot compete each other if their pattern does not match
//					/// We assume here that team i is assigned to pattern j etc.
//					for(IloInt i=0; i < nrTeams; ++i){
//						for (IloInt j = 0; j < nrTeams; ++j) {
//							if (i == j) { continue; }
//							for (IloInt s = 0; s < nrSlots; ++s) {
//								if (patternSet.at(i).at(s) != 0 || patternSet.at(j).at(s) != 1) {
//									modelInner.add(x[i][j][s] == 0);
//								}
//							}
//						}
//					}
//			
//					/// Each team plays exactly once in every time slot
//					for(IloInt i=0; i < nrTeams; ++i){
//						for(IloInt s=0; s < nrSlots; ++s){
//							IloExpr totGames(envInner);
//							for(int j=0; j < nrTeams; ++j){
//								if(j==i) { continue; }
//								totGames += x[i][j][s] + x[j][i][s];
//							}
//							modelInner.add(totGames == 1);
//						}	
//					}
//
//					/// Each team plays the required number of times against every other team
//					assert(nrRoundRobin == 2);
//					for(IloInt i=0; i < nrTeams; ++i){
//						for(IloInt j=0; j < nrTeams; ++j){
//							if (i == j) { continue; }
//							IloExpr games(envInner);
//							for(int s=0; s < nrSlots; ++s){
//								games += x[i][j][s];
//							}
//							modelInner.add(games == 1);
//						}
//					}
//
//					/// For each pair of teams, there are at least min time slots between the mutual games
//					const int min = 2;
//					for(IloInt i=0; i < nrTeams; ++i){
//						for(IloInt j=i+1; j < nrTeams; ++j){
//							for(IloInt s1=0; s1 < nrSlots; s1++){
//								IloExpr games(envInner);
//								for(IloInt s2=s1; s2 < nrSlots && s2 <= s1 + min ; s2++){
//									games += x[i][j][s2] + x[j][i][s2];
//								}
//								modelInner.add(games <= 1);
//							}
//						}
//					}
//
//					// Model symmetry constraints
//					assert(symmetry == P || symmetry == NONEGAME);
//
//					if (symmetry == P) {
//						assert(nrRoundRobin == 2);
//						assert(nrTeams%2 == 0);
//						const int nrSlotsPhase = nrTeams-1;
//						for (int r = 0; r < nrRoundRobin-1; ++r) {
//							for (IloInt i = 0; i < nrTeams; ++i) {
//								for (IloInt j = i+1; j < nrTeams; ++j) {
//									IloExpr games(envInner);
//									for(IloInt s = 0; s < nrSlotsPhase; ++s){
//										games += x[i][j][nrSlotsPhase*r+s] + x[j][i][nrSlotsPhase*r+s];
//									}
//									// Each pair meets exactly once in every phase
//									modelInner.add(games == 1);
//								}
//							}
//						}
//					}
//
//					/// Optimize
//					IloCplex cplexInner(modelInner);
//					// Disable printing
//					cplexInner.setOut(envInner.getNullStream());
//					cplexInner.setWarning(envInner.getNullStream());
//
//					cplexInner.solve();
//
//					if (cplexInner.getStatus() == IloAlgorithm::Infeasible){
//						envInner.out() << "HAP set infeasible" << std::endl;
//					} else {
//						std::cout << "Found a feasible HAP set!" << std::endl;
//						for(auto row : patternSet){
//							for(auto c : row){
//								std::cout << c << "\t";
//							}
//							std::cout << std::endl;
//						}
//						opponentSchedule.clear();
//						for (int i = 0; i < nrTeams; ++i) {
//							opponentSchedule.push_back(std::vector<int>(nrSlots));	
//						}
//						for (int i = 0; i < nrTeams; ++i) {
//							for (int j = 0; j < nrTeams; ++j) {
//								if (i==j) {
//									continue;
//								}
//								for (int s = 0; s < nrSlots; ++s) {
//									if ((patternSet.at(i).at(s)!=patternSet.at(j).at(s)) && cplexInner.getIntValue(x[i][j][s])) {
//										opponentSchedule.at(i).at(s) = j;
//										opponentSchedule.at(j).at(s) = i;
//									}	
//								}	
//							}	
//						}
//						foundASol = true;
//						break;
//					}
//
//				} catch(IloException &ex){
//					envInner.out() << "Unexpected error. Caught " << ex << std::endl;
//				}
//				envInner.end();
//
//				// Forbid the same pattern set from being found again
//				model.add(sumChosen <= nrTeams-1);
//			}
//		} catch (IloException &ex){
//			env.out() << "Unexpected error. Caught " << ex << std::endl;
//		}
//		env.end();
//	}
//
//	return std::make_pair(opponentSchedule, patternSet);
//}


/**
 * Construct a random timetable.
 * Idea: use Vizing algorithm to generate a 1RR
 **/
std::pair<std::vector<std::vector<int>>, std::vector<std::vector<bool>>> Interface::randomVizing(const int nrTeams, const int nrRoundRobin, const GameMode symmetry){

	const int nrSlotsPhase = nrTeams - 1 + nrTeams%2;
	const int nrSlots = nrRoundRobin*nrSlotsPhase;

	// Generate opponent schedule for a 1RR
	std::vector<std::vector<int>> opponentSchedule = vizing1RR(nrTeams);

	// Initialize other part of opponentSchedule at -1
	for(int i=0; i < nrTeams; ++i){
		opponentSchedule.at(i).resize(nrSlots);
	}

	// Generate a random pattern set for the first half
	std::vector<std::vector<bool>> patternSet(nrTeams, std::vector<bool> (nrSlots));
	for(int i=0; i < nrTeams; ++i){
		for (int s = 0; s < nrSlotsPhase; ++s) {
			const int opponent = opponentSchedule.at(i).at(s);
			if (opponent > i) {
				const int randBool = std::rand() % 2;
				patternSet.at(i).at(s) = randBool;
				patternSet.at(opponent).at(s) = (randBool == 0) ? 1 : 0;
			} 
		}
	}

	// Generate the other parts of kRR
	for (int k = 1; k < nrRoundRobin; ++k) {
		switch (symmetry) {
			case NONEGAME:
			case P:
			{
				// Generate k other 1RR's and shuffle the results in case of NONEGAME
				std::vector<std::vector<int>> dummy = vizing1RR(nrTeams);
				for (int i = 0; i < nrTeams; ++i) {
					for (int s = 0; s < nrSlotsPhase; ++s) {
						opponentSchedule.at(i).at(k*nrSlotsPhase+s) = dummy.at(i).at(s);	
					}	
				}
			
				// Mirror the home advantage of the games in the previous phase
				for(int i=0; i < nrTeams; ++i){
					for(int s = nrSlotsPhase*k; s < nrSlotsPhase*(k+1); ++s){
						const int opponent = opponentSchedule.at(i).at(s);
						if (opponent > i) {
							// What was the home advantage in the previous part?
							bool prevHome;
							bool found = false;
							for(int s = nrSlotsPhase*(k-1); s < nrSlotsPhase*(k); ++s){
								if (opponentSchedule.at(i).at(s) == opponent) {
									prevHome = patternSet.at(i).at(s);
									found = true;
									break;
								}
							}
							assert(found == true);
							// Mirror the home advantage
							patternSet.at(i).at(s) = (prevHome == 0) ? 1 : 0;
							patternSet.at(opponent).at(s) = prevHome;
						}
					}
				
				}
				break;
			}
			case M:
			{
				for (int i = 0; i < nrTeams; ++i) {
					for(int s = 0; s < nrSlotsPhase; ++s){ // Length of each phase
						patternSet.at(i).at(nrSlotsPhase*k+s) = !(patternSet.at(i).at(nrSlotsPhase*(k-1)+s));
						opponentSchedule.at(i).at(nrSlotsPhase*k+s) = opponentSchedule.at(i).at(nrSlotsPhase*(k-1)+s);
					}
				}	
	
				break;
			}
			case I:
			{
				for (int i = 0; i < nrTeams; ++i) {
					for(int s = 0; s < nrSlotsPhase; ++s){ // length of each phase
						patternSet.at(i).at(nrSlotsPhase*k+s) = !(patternSet.at(i).at(nrSlotsPhase*(k)-s-1));
						opponentSchedule.at(i).at(nrSlotsPhase*k+s) = opponentSchedule.at(i).at(nrSlotsPhase*(k)-s-1);
					}
				}
				break;
			}
			case E:
			{
				for (int i = 0; i < nrTeams; ++i) {
					for(int s = 0; s < nrSlotsPhase; ++s){ // length of each phase
						if (s == 0) { // First slot in interval r corresponds to last slot in interval r-1
							patternSet.at(i).at(nrSlotsPhase*k) = !(patternSet.at(i).at(nrSlotsPhase*k - 1));
							opponentSchedule.at(i).at(nrSlotsPhase*k) = opponentSchedule.at(i).at(nrSlotsPhase*k - 1);
						} else { // i-th slot of interval r corresponds to (i-1)-th slot in interval r+1
							patternSet.at(i).at(nrSlotsPhase*k+s) = !(patternSet.at(i).at(nrSlotsPhase*(k-1) + s - 1));
							opponentSchedule.at(i).at(nrSlotsPhase*k+s) = opponentSchedule.at(i).at(nrSlotsPhase*(k-1) + s - 1);
						}					
					}
				}
				break;
			}
			case F:
			{
				for (int i = 0; i < nrTeams; ++i) {
					for(int s = 0; s < nrSlotsPhase; ++s){ // length of each phase
						if (s == nrSlotsPhase-1) { // Last slot interval r is first slot interval r-1
							patternSet.at(i).at(nrSlotsPhase*(k+1)-1) = !(patternSet.at(i).at(nrSlotsPhase*(k-1)));
							opponentSchedule.at(i).at(nrSlotsPhase*(k+1)-1) = opponentSchedule.at(i).at(nrSlotsPhase*(k-1));
						} else { // i-th slot in interval r corresponds to (i+1)-th slot of interval r-1
							patternSet.at(i).at(nrSlotsPhase*k+s) = !(patternSet.at(i).at(nrSlotsPhase*(k-1)+s+1));
							opponentSchedule.at(i).at(nrSlotsPhase*k+s) = opponentSchedule.at(i).at(nrSlotsPhase*(k-1)+s+1);
						}
					}
				}
				break;
			}
			default:
				assert(2 < 1);	
		}
	}

	// If no symmetry is required: we additionally need to shuffle the end result
	if (symmetry == NONEGAME) {
		// Shuffle the rounds
		std::vector<int> colIds(nrSlots);
		for(int i=0; i < nrSlots; i++){ colIds.at(i) = i; }
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle (colIds.begin(), colIds.end(), std::default_random_engine(seed));
		std::vector<std::vector<int>> opponentScheduleShuffled(nrTeams, std::vector<int> (nrSlots));
		std::vector<std::vector<bool>> patternSetShuffled(nrTeams, std::vector<bool> (nrSlots));

		for(int i=0; i < nrTeams; i++){
			for(int s=0; s < nrSlots; s++){
				opponentScheduleShuffled.at(i).at(s) = opponentSchedule.at(i).at(colIds.at(s));
				patternSetShuffled.at(i).at(s) = patternSet.at(i).at(colIds.at(s));
			}
		}
		opponentSchedule = opponentScheduleShuffled;
		patternSet = patternSetShuffled;
		
	}

	return std::make_pair(opponentSchedule, patternSet);
}

/**
 * Generate a random instance 
 * nrTeams: total number of teams in the instance (must be even)
 * nrRoundRobin: number of time slots the teams have to play
 * symmetry: symmetric structure of the round robin
 * objMode: objective to be optimized
 * constraintsMap: key=constraint name (e.g. CA1Hard or CA1Soft), value is number of constraints to be generated
 */
// TODO TODO
// This function makes use of cplex and is therefore temporary disabled
//void Interface::generateRandomInstance(const int nrTeams, const int nrRoundRobin, const GameMode symmetry, const ObjMode objMode, const std::map<std::string, int> constraintsMap, std::string fileName, std::string solName){
//	std::cout << "Generating random instance... " << std::endl;
//							for(auto f : constraintsMap){
//								std::cout << f.first << ":\t" << f.second << std::endl;
//							}
//
//
//	// We assume that the instance is time constrained, and that the number of teams is even
//	const int nrSlotsPhase = nrTeams - 1 + nrTeams%2;
//	const int nrSlots = nrRoundRobin*nrSlotsPhase;
//	assert(nrTeams%2 == 0);
//
//	// Add league, teams, and slots to instance
//	addLeague(new League(0, "League 0", nrRoundRobin, symmetry, C));
//	for (int i = 0; i < nrTeams; ++i) {
//		addTeam(new Team(i, "Team " + std::to_string(i), 0, {}));
//	}
//	for (int s = 0; s < nrSlots; ++s) {
//		addSlot(new Slot(s, "Slot " + std::to_string(s), {})); 
//	}
//
//	// Generate meeting objects
//	generateMeetings();
//
//	// Generate a random timetable with at most 6 breaks per team
//	bool max3HomeAway = (constraintsMap.find("CA3Hard")!=constraintsMap.end() && constraintsMap.at("CA3Hard") > 0);
//	bool max5Breaks = (constraintsMap.find("BR2Hard")!=constraintsMap.end() && constraintsMap.at("BR2Hard") > 0);
//	std::cout << "Start random P" << std::endl;
//	std::pair<std::vector<std::vector<int>>, std::vector<std::vector<bool>>> output = randomPattern(nrTeams, nrRoundRobin, symmetry, max3HomeAway, max5Breaks);
//	std::cout << "Random P done" << std::endl;
//
//	// Save the initial timetable
//	for(int i=0; i < nrTeams; ++i){
//		for (int s = 0; s < nrSlots; ++s) {
//			if (output.second.at(i).at(s)) {
//				scheduleMeeting(i, output.first.at(i).at(s), s);
//			}	
//		}
//	}
//	std::cout << "Timetable saved" << std::endl;
//
//	//// Add objective
//	assert(objMode==SC);
//	addObjective(objMode);
//
//	//// Random number generator between 0 and 99
//	//std::uniform_int_distribution<int> randInt100(0, 99);
//	//if (objMode == TR) {
//	//	// Need to generate distance matrix
//	//	for (int i = 0; i < nrTeams; ++i) {
//	//		// Distance to own venue is zero
//	//		addDistance(i,i,0);
//	//		for (int j = i+1; j < nrTeams; ++j) {
//	//			// Symmetric distance matrix
//	//			const int randInt = randInt100(randEngine);
//	//			addDistance(i,j, randInt);
//	//			addDistance(j,i, randInt);
//	//		}
//	//	}
//	//} else if (objMode == CR){
//	//	// Need to generate (non-symmetric) cost matrix
//	//	for (int i = 0; i < nrTeams; ++i) {
//	//		// Cost against ourselves is zero
//	//		for (int j = 0; j < nrTeams; ++j) {
//	//			for (int s = 0; s < nrSlots; ++s) {
//	//				if (i == j) {  
//	//					addCost(i, j, s, 0);
//	//				} else {
//	//					addCost(i, j,s, randInt100(randEngine));
//	//				}
//	//			}
//	//		}
//	//	}
//	//}
//
//	// Add meta data
//	addInstanceName(fileName);
//	addContributor("ITC2021");
//	addDataType(ARTIFICIAL);
//	addDescription("See www/sportscheduling.ugent.be/ITC2021");
//
//	std::cout << "Meta added" << std::endl;
//
//	// Pairs to sample from, so as to avoid that duplicate CA4 constraints are added
//	std::vector<std::pair<int, int>> teamPairsCA4Hard;
//	std::vector<std::pair<int, int>> teamPairsCA4Soft;
//	for(int i = 0; i < Instance::get()->getNrTeams(); ++i){
//		for(int j=i+1; j < Instance::get()->getNrTeams(); ++j){
//			teamPairsCA4Hard.push_back(std::make_pair(i,j));
//			teamPairsCA4Soft.push_back(std::make_pair(i,j));
//		}
//	}
//	
//	// Add constraints
//	for(auto c : constraintsMap){
//		if (c.first == "CA1Hard") {
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new CA1(HARD,1));
//			}
//		} else if(c.first == "CA1Soft"){
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new CA1(SOFT,1));
//			}
//		} else if (c.first == "CA2Hard") {
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new CA2(HARD,1));
//			}
//		} else if (c.first == "CA2Soft") {
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new CA2(SOFT,5));
//			}
//		} else if (c.first == "CA3Hard") {
//			assert(c.second <= 2); // At most 2 CA3Hard constraints
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new CA3(HARD,1));
//			}
//		} else if (c.first == "CA3Soft") {
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new CA3(SOFT,5));
//			}
//		} else if (c.first == "CA4Hard") {
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new CA4(HARD,1, teamPairsCA4Hard));
//			}
//		} else if (c.first == "CA4Soft") {
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new CA4(SOFT,5, teamPairsCA4Soft));
//			}
//		} else if (c.first == "BR1Hard") {
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new BR1(HARD,1));
//			}
//		} else if (c.first == "BR1Soft") {
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new BR1(SOFT,5));
//			}
//		} else if (c.first == "BR2Hard") {
//			// There is at most one BR2 hard constraint
//			if (c.second > 0) {
//				assert(c.second == 1);
//				Instance::get()->addConstraint(new BR2(HARD,1));
//			}
//		} else if (c.first == "BR2Soft") {
//			if (c.second > 0) {
//				assert(c.second <= nrSlots);
//				for (int i = 0; i < c.second; ++i) {
//					Instance::get()->addConstraint(new BR2(SOFT,10));
//				}
//			}
//		} else if (c.first == "GA1Hard") {
//			if (c.second > 0) {
//				for (int i = 0; i < c.second; ++i) {
//					Instance::get()->addConstraint(new GA1(HARD,1));
//				}
//			}
//		} else if (c.first == "GA1Soft") {
//			for (int i = 0; i < c.second; ++i) {
//				Instance::get()->addConstraint(new GA1(SOFT,1));
//			}
//		} else if (c.first == "FA2Soft") {
//			if (c.second > 0) {
//				// There is at most one FA2 constraint
//				assert(c.second == 1);
//				Instance::get()->addConstraint(new FA2(SOFT,10));
//			}
//		} else if (c.first == "SE1Soft") {
//			if (c.second > 0) {
//				// There is at most one SE1 constraint
//				assert(c.second == 1);
//				Instance::get()->addConstraint(new SE1(SOFT,10));
//			}
//		} else {
//			std::cout << "Constraint name '" << c.first << "' unkown or not yet implemented" << std::endl;
//			assert(2<1);
//		}
//	}
//	std::cout << "Constraints added" << std::endl;
//
//	//// Verify the instance
//	checkConstr();
//	ObjCost obj = Instance::get()->getObjectiveValue();
//	std::cout << "Objective: " << obj.first << " - " << obj.second << std::endl;
//
//	//// Save the XML file	
//	writeInstanceXml(fileName);
//	writeSolutionXml(solName);
//
//	return;
//}
