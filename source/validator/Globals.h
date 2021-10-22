#include <string>
#include <cmath>
#include <random>

// Forward declarations
class League;
class LeagueGroup;
class Slot;
class SlotGroup;
class Team;
class TeamGroup;
class Interface;
class Input;
class Meeting;
class Constraint;

#ifndef GLOBALS_H
#define GLOBALS_H

enum CType 			: unsigned int;
enum HomeMode 		: unsigned int;
enum GlobMode 		: unsigned int;
enum CMode 			: unsigned int;
enum AFMode 		: unsigned int; 
enum CompareMode 	: unsigned int; 
enum Compactness 	: unsigned int; 
enum GameMode		: unsigned int; 
enum ObjMode 		: unsigned int;
enum SolutionMethod : unsigned int;


// std
#include <list>
#include <map>
#include <set>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

// Typedefs
typedef std::list<int>							IdList;
typedef std::list<Constraint*>					ConstraintList;
typedef std::list<Constraint*>::iterator		ConstraintListIt;
typedef std::map<int, Team*>					TeamMap; 
typedef std::map<int, Team*>::iterator			TeamMapIt; 
typedef std::set<Team*>							TeamSet;
typedef std::set<Team*>::iterator				TeamSetIt;
typedef std::map<int, TeamGroup*>				TeamGroupMap;
typedef std::map<int, TeamGroup*>::iterator		TeamGroupMapIt;
typedef std::set<TeamGroup*>					TeamGroupSet;
typedef std::set<TeamGroup*>::iterator			TeamGroupSetIt;
typedef std::map<int, League*>					LeagueMap;
typedef std::map<int, League*>::iterator		LeagueMapIt;
typedef std::set<League*>						LeagueSet;
typedef std::set<League*>::iterator				LeagueSetIt;
typedef std::map<int, LeagueGroup*>				LeagueGroupMap;
typedef std::map<int, LeagueGroup*>::iterator	LeagueGroupMapIt;
typedef std::set<LeagueGroup*>					LeagueGroupSet;
typedef std::set<LeagueGroup*>::iterator		LeagueGroupSetIt;
typedef std::set<League*>						LeagueSet;
typedef std::set<League*>::iterator				LeagueSetIt;
typedef std::map<std::pair<Team*, Team*>, int> 	DistanceMap;
typedef std::map<std::tuple<Team*, Team*, Slot*>, int> 	CostMap;
typedef std::map<int, Slot*>					SlotMap;
typedef std::map<int, Slot*>::iterator			SlotMapIt;
typedef std::set<Slot*>							SlotSet;
typedef std::set<Slot*>::iterator				SlotSetIt;
typedef std::map<int, SlotGroup*>				SlotGroupMap;
typedef std::map<int, SlotGroup*>::iterator		SlotGroupMapIt;
typedef std::set<SlotGroup*>					SlotGroupSet;
typedef std::set<SlotGroup*>::iterator			SlotGroupSetIt;
typedef std::list<Meeting*>						MeetingList;
typedef std::list<Meeting*>::iterator			MeetingListIt;
typedef std::list<std::pair<int,int>>			MeetingIdList;
typedef std::set<std::pair<int,int>>			MeetingSet;
typedef std::map<std::string, std::string>		AttrMap;
typedef std::list<AttrMap>						AttrMapList;
typedef std::pair<int, int>						ObjCost; 		// pair<infeasibility cost, feasibility cost>
typedef std::list<std::pair<int, int>>			SlotPairList;

// Enumerations: see https://stackoverflow.com/questions/21456262/enum-to-string-in-c11
enum CType : unsigned int {HARD, SOFT};
const std::string CTypeToStr[] = {"HARD", "SOFT"};
const std::map<std::string, CType> CTypeMap = {{"HARD", HARD}, {"SOFT", SOFT}};
enum  HomeMode : unsigned int {H, A, HA};
const std::string HomeModeToStr[] = {"H", "A", "HA"};
const std::map<std::string, HomeMode> HomeModeMap = {{"H", H}, {"A", A}, {"HA", HA}};
enum GlobMode : unsigned int {EVERY, GLOBAL};
const std::string GlobModeToStr[] = {"EVERY", "GLOBAL"};
const std::map<std::string, GlobMode> GlobModeMap = {{"EVERY", EVERY}, {"GLOBAL", GLOBAL}};
enum CMode : unsigned int {SLOTS, GAMES};
const std::string CModeToStr[] = {"SLOTS", "GAMES"};
const std::map<std::string, CMode> CModeMap = {{"SLOTS", SLOTS}, {"GAMES", GAMES}};
enum  AFMode : unsigned int {ANY, FINAL};
const std::string AFModeToStr[] = {"ANY", "FINAL"};
const std::map<std::string, AFMode> AFModeMap = {{"ANY", ANY}, {"FINAL", FINAL}};
enum  CompareMode : unsigned int {EQ, NEQ, LE, LEQ, GE, GEQ};
const std::string CompareModeToStr[] = {"EQ", "NEQ", "LE", "LEQ", "GE", "GEQ"};
const std::map<std::string, CompareMode> CompareModeMap =  {{"EQ", EQ}, {"NEQ", NEQ}, {"LE", LE}, {"LEQ", LEQ}, {"GE", GE}, {"GEQ", GEQ}};
enum  Compactness : unsigned int {C, R, NONECOM};
const std::string CompactnessToStr[] = {"C", "R", "NULL"};
const std::map<std::string, Compactness> CompactnessMap = {{"C", C}, {"R", R}, {"NULL", NONECOM}};
enum  GameMode : unsigned int {M, I, E ,F, P, NONEGAME};
const std::string GameModeToStr[] = {"M", "I", "E" ,"F", "P", "NULL"};
const std::map<std::string, GameMode> GameModeMap = {{"M", M}, {"I", I}, {"E", E}, {"F", F}, {"P", P}, {"NULL", NONEGAME}};
enum  ObjMode : unsigned int {BM, CO, CR, TR, SC, NONEOBJ};
const std::string ObjModeToStr[] = {"BM", "CO", "CR", "TR", "SC", "NONE"};
const std::map<std::string, ObjMode> ObjModeMap = {{"BM", BM}, {"CO", CO}, {"CR", CR}, {"TR", TR}, {"SC", SC}, {"NONE", NONEOBJ}};
enum  DataType : unsigned int {ARTIFICIAL, REAL, NONEDAT};
const std::string DataTypeToStr[] = {"A", "R", "NULL"};
const std::map<std::string, DataType> DaTaTypeMap = {{"A", ARTIFICIAL}, {"R", REAL}, {"", NONEDAT}};
enum SolutionMethod : unsigned int {IP, CP, ALG, HEU, NONESOL};
const std::string SolutionMethodToStr[] = {"IP", "CP",  "A", "H", "NULL"};
const std::map<std::string, SolutionMethod> SolutionMethodMap = {{"IP", IP}, {"CP", CP}, {"A", ALG}, {"H", HEU}, {"NULL", NONESOL}};
enum BreakMode : unsigned int {REGULAR, GENERALIZED};
const std::string BreakModeToStr[] = {"REGULAR", "GENERALIZED"};
const std::map<std::string, BreakMode> BreakModeMap = {{"REGULAR", REGULAR}, {"GENERALIZED", GENERALIZED}};

// Collect all id's of datastructure of objects with getId() function into a semi-colon based string
template <class T>
std::string idToString(T objects){
	std::string str = "";
	for (auto object : objects) {
		str += std::to_string(object->getId()) + ";";
	}
	// Remove the last semicolon
	return str.substr(0, str.size() - 1);
}
extern std::default_random_engine randEngine; 	/// Global random number generator


#endif /* GLOBALS_H */

