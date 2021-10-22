/*********************************************************************************************************
Interface class, implemented via the singleton pattern
Factory pattern:
	Wikipedia: Factory method pattern 
	Create objects without having to know the exact class of the object that will be created.
	"This is done by creating objects by calling a factory method—either specified in an interface 
	and implemented by child classes, or implemented in a base class and optionally overridden 
	by derived classes—rather than by calling a constructor."
Singleton pattern:
	Ensure that the class has only one instance, that is globally acessible.
	The former is implemented by making the constructor private, and by introducing a static function.
	Lazy variant: initialize on first use 
*********************************************************************************************************/
#ifndef CLASSFACTORY_H
#define CLASSFACTORY_H

// Include other
#include "../validator/Globals.h"
#include "../validator/ClassInstance.h"
#include "../validator/ClassConstraint.h"
#include <assert.h>
#include "tinyXMLParser.h"
#include "ClassException.h"

class Interface
{
public:
	// Singleton pattern: public static accessor
	static Interface* get() {
		if (!fInstance) { fInstance = new Interface; }
		return fInstance;	
	}

	static void reset() {
		delete fInstance;
		fInstance = NULL;
	}
	
	// Public member functions
	
	// Resources
	void addLeague(League* l);
	void addLeagueGroup(LeagueGroup* lg);
	void addSlot(Slot* s);
	void addSlotGroup(SlotGroup* sg);
	void addTeam(Team* t);
	void addTeamGroup(TeamGroup* tg);	

	// Objective
	void addObjective(const ObjMode o);
	void addObjectiveValue(const int infeasibilityValue, const int feasibilityValue);
	void addLowerBound(const int lb_inf, const int lb_feas);
	
	// Constraints
	void addConstraint(Constraint* c);
	ObjCost checkConstr(bool silent=false);

	// Classify
	std::string classify();

	// Data
	void addDistance(int teamId1, int teamId2, int dist);
	void addCOEWeight(int teamId1, int teamId2, int weight);
	void addCost(int homeId, int awayId, int slotId, int cost);

	// Parser
	void addInstanceName(const std::string name, const std::string schema = "");
	void readInstanceXml(std::string xmlFile, std::string xmlSchema = "");	
	void readInstanceXmlString(std::string xmlContent, std::string xmlSchema = "");	
	void writeInstanceXml(std::string xmlFile);
	void addSolutionName(const std::string name, const std::string schema = "");
	void readSolutionXml(std::string xmlFile, std::string xmlSchemaIn = "", std::string xmlSchemaSol = "", bool readIn=false);
	void readSolutionXmlString(std::string xmlContent, std::string xmlSchemaIn = "", std::string xmlSchemaSol = "", bool readIn=false);
	void writeSolutionXml(std::string xmlFile);
	void writeBoundXml(std::string xmlFile);
	
	// Meta data	
	void addDataType(const DataType d);
	void addContributor(const std::string s);	
	void addDate(const int year, const int month=-1, const int day=-1);
	void addCountry(const std::string s);
	void addDescription(const std::string s);
	void addRemarks(const std::string s);
	void addSolutionMethod(const SolutionMethod s);

	// Meetings
	void addMeeting(int teamId1, int teamId2, bool noHome, int slot = -1);
	void generateMeetings();
	void scheduleMeeting(int homeId, int awayId, int slot);

	/// Generate a canonical timetable
	std::pair<std::vector<std::vector<int>>, std::vector<std::vector<bool>>> canonicalPattern(const int nrTeams, const int nrRoundRobin = 1, const bool modifiedPattern = false, const GameMode symmetry = M);	

	/// Generate a random timetable
	std::pair<std::vector<std::vector<int>>, std::vector<std::vector<bool>>> randomVizing(const int nrTeams, const int nrRoundRobin, const GameMode symmetry);	

	/// Generate a random timetable by enumerating patterns
	// TODO This function requires cplex, and is therefore temporary disabled (for github online version)
	//std::pair<std::vector<std::vector<int>>, std::vector<std::vector<bool>>> randomPattern(const int nrTeams, const int nrRoundRobin, const GameMode symmetry, const bool max3HomeAway, const bool max4Breaks);	

	/// Generate a random instance
	// TODO This function requires cplex, and is therefore temporary disabled (for github online version)
	//void generateRandomInstance(const int nrTeams, const int nrRoundRobin, const GameMode symmetry, const ObjMode objMode, const std::map<std::string, int> constraintsMap, std::string fileName, std::string solName);

private:
	// constructor
	Interface();
	virtual ~Interface();

protected:
	// Type here all protected variables
	static Interface* fInstance;
};

#endif /* CLASSFACTORY_H */
