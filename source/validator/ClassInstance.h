/*********************************************************************************************************
Instance class stores all input-related data, implemented via the singleton pattern
Singleton pattern:
	Ensure that the class has only one instance, that is globally acessible.
	The former is implemented by making the constructor private, and by introducing a static function.
	Lazy variant: initialize on first use 
*********************************************************************************************************/
// Definitions

#ifndef CLASSINSTANCE_H
#define CLASSINSTANCE_H

// Include other
#include "Globals.h"
#include "ClassConstraint.h"
#include "ClassMeeting.h"
#include "ClassLeague.h"
#include "ClassLeagueGroup.h"
#include "ClassSlot.h"
#include "ClassSlotGroup.h"
#include "ClassTeam.h"
#include "ClassTeamGroup.h"
#include "ClassException.h"

class Instance
{
public:
	// Singleton pattern: public static accessor
	static Instance* get() {
		if (!iInstance) { iInstance = new Instance; }
		return iInstance;	
	}

	static void reset() {
		delete iInstance;
		iInstance = NULL;
	}	
	
	// Modify league map
	void addLeague(League*);
	League* getLeague(int id) { return leagues.at(id); }
	LeagueMap getLeagues() const{ return leagues; } 
	int getNrLeagues() const{ return leagues.size(); }
	
	// Modify league group map
	void addLeagueGroup(LeagueGroup*);
	LeagueGroup* getLeagueGroup(int id) { return leagueGroups.at(id); }
	LeagueGroupMap getLeagueGroups() const{ return leagueGroups; }

	// Modify team map
	void addTeam(Team*);
	Team* getTeam(const int id) const{ return teams.at(id); } 
	Team* getRandTeam();
	TeamSet getRandTeams(const std::vector<int> values, const std::vector<int> weights, std::vector<int> teamIds);
	TeamMap getTeams() const{ return teams; }
	int getNrTeams() const{ return teams.size(); }
	TeamSet collectTeams(TeamSet teams, TeamGroupSet teamGroups);

	// Modify team group map
	void addTeamGroup(TeamGroup*);
	TeamGroup* getTeamGroup(int id) { return teamGroups.at(id); }
	TeamGroupMap getTeamGroups() const{ return teamGroups; }

	// Modify slot map
	void addSlot(Slot*);
	Slot* getSlot(const int id) const{ return slots.at(id); }
	Slot* getRandSlot();
	SlotSet getRandSlots(const std::vector<int> values, const std::vector<int> weights);
	SlotMap getSlots() const{ return slots; }
	int getNrSlots() const{ return slots.size(); }
	SlotSet collectSlots(SlotSet slots, SlotGroupSet slotGroups);

	// Modify slot group map
	void addSlotGroup(SlotGroup*);
	SlotGroup* getSlotGroup(int id) { return slotGroups.at(id); }
	SlotGroupMap getSlotGroups() const{ return slotGroups; }

	// Modify constraint list
	void addConstraint(Constraint* c) { constraints.push_back(c); }
	ConstraintList getCnstrs() { return constraints; }

	// Modify objective
	void setObjective(const ObjMode o) { objectiveMode = o; }
	ObjMode getObjective() const { return objectiveMode; }
	void setObjectiveValue(const ObjCost c) { objectiveValue = c; }
	ObjCost getObjectiveValue() const{ return objectiveValue; }

	// Modify distances and costs
	void setDistance(Team* t1, Team* t2, const int dist) { distances[std::make_pair(t1,t2)] = dist; }
	int getDistance(Team* t1, Team* t2) const { return distances.at(std::make_pair(t1,t2)); }
	int getDistance(int teamId1, int teamId2) const { return getDistance(getTeam(teamId1), getTeam(teamId2)); }
	AttrMapList serializeDistances();
	DistanceMap getDistances() { return distances; }
	bool isSetDistance(Team* t1, Team* t2) const { return distances.count(std::make_pair(t1,t2)); }

	void setCOEWeight(Team* t1, Team* t2, const int weight) { COEWeights[std::make_pair(t1,t2)] = weight; }
	int getCOEWeight(Team* t1, Team* t2) const { return COEWeights.at(std::make_pair(t1,t2)); }
	int getCOEWeight(int teamId1, int teamId2) const { return getCOEWeight(getTeam(teamId1), getTeam(teamId2)); }
	AttrMapList serializeCOEWeights();
	DistanceMap getCOEWeights() { return COEWeights; }
	bool isSetCOEWeight(Team* t1, Team* t2) const { return COEWeights.count(std::make_pair(t1,t2)); }
	bool isSetCOEWeight(int teamId1, int teamId2) const { return isSetCOEWeight(getTeam(teamId1), getTeam(teamId2)); }

	void setCost(Team* t1, Team* t2, Slot* s, const int cost) { costs[std::make_tuple(t1, t2, s)] = cost; }
	int getCost(Team* t1, Team* t2, Slot* s) const { return costs.at(std::make_tuple(t1, t2, s)); }
	int getCost(int teamId1, int teamId2, int slotId) const { return getCost(getTeam(teamId1), getTeam(teamId2), getSlot(slotId)); }
	CostMap getCosts() { return costs; }
	AttrMapList serializeCosts();
	bool isSetCost(Team* t1, Team* t2, Slot* s) const { return costs.count(std::make_tuple(t1,t2, s)); }

	// Modify meetings
	void addMeeting(Team* t1, Team* t2, bool noHome, int slot);
	void generateMeeting(League* l);
	void scheduleMeeting(Team* h, Team* a, Slot* s);
	void clearSchedule();

	// Modify meta-data
	void setInstanceName(const std::string n, const std::string schema) { instanceName = n; xmlSchemaIn = schema; }
	std::string getInstanceName() const { return instanceName; }
	std::string getSchemaIn() const { return xmlSchemaIn; }
	void setSolutionName(const std::string n, const std::string schema) { solutionName = n; xmlSchemaSol = schema; }
	std::string getSolutionName() const { return solutionName; }
	std::string getSchemaSol() const { return xmlSchemaSol; }
	void setDataType(const DataType m) { dataType = m; }
	DataType getDataType() const{ return dataType; }
	void setContributor(const std::string s) { contributor = s; }
	std::string getContributor() const{ return contributor; }
	void setDate(const int day, const int month, const int year) {date= std::make_tuple(day, month, year);}
	std::tuple<int, int, int> getDate() const{ return date; }
	void setCountry(const std::string s) {country = s;}
	std::string getCountry() const{ return country; }
	void setDescription(const std::string s) { description = s; }
	std::string getDescription() const{ return description; }
	void setRemarks(const std::string s) { remarks = s; }
	std::string getRemarks() const{ return remarks; }
	void setSolutionMethod(const SolutionMethod s) { solMethod = s; }
	SolutionMethod getSolutionMethod() const{ return solMethod; }
	void setLowerBound(const ObjCost lb) { lowerBound = lb; }
	ObjCost getLowerBound() const{ return lowerBound; }

	// Querry games
	MeetingList getMeetings() const{ return meetings; }
	MeetingList getMeetingsTeam(TeamSet teams, HomeMode mode);
	MeetingList getMeetingsSlot(SlotSet slots);
	MeetingList getMeetingsTeamSlot(TeamSet teams, SlotSet slots, HomeMode mode);
	MeetingList getMeetingsTeamSlotGroup(TeamSet teams, SlotGroupSet slotGroups, HomeMode mode);
	MeetingList getMeetingsTeamTeamSlot(TeamSet teams1, TeamSet teams2, SlotSet slots, HomeMode mode);
	MeetingList getMeetingsTeamTeam(TeamSet teams1, TeamSet teams2, HomeMode mode);

	// Calculate the carry-over effects value
	int carryOver(League* l);

	// Calculate the total distance traveled by a team
	int distance(Team* t, SlotSet slots={});

	// Calculate the total number of breaks a team has
	int breaks(Team* t);

	// Calculate the total costs or revenues invoked by the home games of a team
	int cost(Team* t);

	// Check all constraints
	void checkConstr(bool silent);	
	int calculateObj();

	// Classify the instance
	std::string classify();

	// Print the solution
	void printSolution();


private:
	// constructor
	Instance() {}
	virtual ~Instance();

protected:
	// Type here all protected variables
	
	static Instance* iInstance;
	
	TeamMap teams; 				// Map with ownership over all team objects: key = id, value = pointer to team object
	TeamGroupMap teamGroups; 		// Map with ownership over all team group objects: key = id, value = pointer to team group object

	SlotMap slots; 				// Map with ownership over all slot objects: key = id, value = pointer to slot object
	SlotGroupMap slotGroups; 		// Map with ownership over all slot group objects: key = id, value = pointer to slot group object
	
	LeagueMap leagues; 			// Map with ownership over all league objects: key = id, value = pointer to league object
	LeagueGroupMap leagueGroups; 		// Map with ownership over all league group objects: key = id, value = pointer to league group object

	ConstraintList constraints; 		// List with ownership over all constraint objects: value = pointer to constraint object

	MeetingList meetings; 			// List with ownership over all meeting objects: value = pointer to meeting object

	DistanceMap distances; 			// Map that contains all pairwise distances between all teams. distances[pair(team1, team2)]

	DistanceMap COEWeights; 		// Map that contains all pairwise carry over weights between all teams. COEWeights[pair(team1, team2)]

	CostMap costs; 				// Map that contains all expected costs or revnues . distances[pair(meeting, slot)]

	ObjMode objectiveMode = NONEOBJ;	// Objective mode
	
						// Cost of solution: (infeasibility value, feasibility value)
	ObjCost objectiveValue = std::make_pair(-1,-1);

	// Meta Data
	std::string instanceName = "NULL"; 	// Name of the instance
	std::string xmlSchemaIn = "NUL";
	std::string solutionName = "NULL"; 	// Name of the solution
	std::string xmlSchemaSol = "NULL";
	DataType dataType = NONEDAT; 		// Data type: real life or artificial
	std::string contributor = "NULL";	// Name of the contributor
						// day - month - year
	std::tuple<int, int, int> date = std::make_tuple(-1,-1,-1);
	std::string country = "NULL";
	std::string description = "NULL";
	std::string remarks = "NULL";
	SolutionMethod solMethod = NONESOL;
	ObjCost lowerBound = {0,0};
};

#endif /* CLASSINSTANCE_H */
