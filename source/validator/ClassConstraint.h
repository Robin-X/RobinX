// Include std
#include <string>
#include <map>
#include <set>
#include <list>
#include <algorithm>    // std::find
#include <iomanip>      // std::setw
#include <array>

#ifndef CLASSCONSTRAINT_H
#define CLASSCONSTRAINT_H

// Include other
#include "Globals.h"
#include "ClassLeague.h"
#include "ClassSlot.h"
#include "ClassSlotGroup.h"
#include "ClassTeam.h"
#include "ClassTeamGroup.h"
#include "ClassInstance.h"
#include "ClassInterface.h"
#include <unordered_map>

// Abstract Base Class
class Constraint
{
public:
	// constructors
	Constraint(CType c, int p, std::string name) : type(c), penalty(p), name(name) {};
	virtual ~Constraint() {}

	// Public member functions
	int getPenalty() const { return penalty; }
	void setPenalty(int p) { penalty  = p; }
	
	// Generate a map with all tag attributes to be serialized
	virtual AttrMap serialize() = 0;

	// Check constraint
	virtual ObjCost checkConstr() = 0;

	// Get name
	virtual std::string getName() const{ return name; }

	// Get type
	CType getType() const { return type; }
	void setType(const CType t) { type = t; }

protected:
	// Data
	CType type; 		// HARD or SOFT
	int penalty; 		
	const std::string name;
};

class BA1 : public Constraint 
{
	// All games should be scheduled: HARD + penalty of one
	// Can only be created by user for soft variant
public:
	BA1 (CType c, int p);
	~BA1 () {}
	AttrMap serialize();
	ObjCost checkConstr();
};

class BA2 : public Constraint
{
	// At most one game a slot only: HARD + penalty of one
	// Can only be created by the instance class for internal use!
friend class Instance;
private:
	BA2(CType c, int p);
	~BA2 () {}
	AttrMap serialize();
	ObjCost checkConstr();
};

class CA1 : public Constraint 
{
public:
	CA1 (CType c, int p, std::array<IdList, 2> t_tg_Id1, int min, int max, HomeMode m, std::array<IdList, 2> s_sg_Id);
	CA1 (CType c, int p);
	~CA1 () {}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams;
	TeamGroupSet  teamGroups;
	int min;
	int max;
	HomeMode mode;
	SlotSet slots;
	SlotGroupSet slotGroups;
};

class CA2 : public Constraint 
{
public:
	CA2 (CType c, int p, std::array<IdList, 2> t_tg_Id1, int min, int max, HomeMode m1, GlobMode m2, std::array<IdList, 2> t_tg_Id2, std::array<IdList, 2> s_sg_Id);
	CA2 (CType c, int p);
	~CA2 () {}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams1, teams2;
	TeamGroupSet  teamGroups1, teamGroups2;
	int min;
	int max;
	HomeMode mode1;
	GlobMode mode2;
	SlotSet slots;
	SlotGroupSet slotGroups;
};

class CA3 : public Constraint 
{
public:
	CA3 (CType c, int p, std::array<IdList, 2> t_tg_Id1, int min, int max, HomeMode m1, std::array<IdList, 2> t_tg_Id2, int intp, CMode m2);
	CA3 (CType c, int p);
	~CA3 () {}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams1, teams2;
	TeamGroupSet  teamGroups1, teamGroups2;
	int min;
	int max;
	HomeMode mode1;
	CMode mode2;
	int intp;
};

class CA4 : public Constraint 
{
public:
	CA4 (CType c, int p, std::array<IdList, 2> t_tg_Id1, int min, int max, HomeMode m1, std::array<IdList, 2> t_tg_Id2, GlobMode m2, std::array<IdList, 2> s_sg_Id);
	CA4 (CType c, int p, std::vector<std::pair<int, int>> teamPairs);
	~CA4 () {}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams1, teams2;
	TeamGroupSet  teamGroups1, teamGroups2;
	int min;
	int max;
	HomeMode mode1; 
	GlobMode mode2;
	SlotSet slots;
	SlotGroupSet slotGroups;
};

class CA5 : public Constraint 
{
public:
	CA5 (CType c, int p, std::array<IdList, 2> t_tg_Id1, int min, int max, std::array<IdList, 2> t_tg_Id2, std::array<IdList, 2> s_sg_Id2);
	CA5 (CType c, int p);
	~CA5 () {}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams1, teams2;
	TeamGroupSet teamGroups1, teamGroups2;
	int min;
	int max;
	SlotSet slots;
	SlotGroupSet slotGroups;
};

class GA1 : public Constraint{
public:
	GA1(CType c, int p, std::array<IdList, 2> s_sg_Id, int min, int max, MeetingIdList meetingIds);
	GA1 (CType c, int p);
	~GA1(){}
	AttrMap serialize();
	ObjCost checkConstr();

	int min;
	int max;
	MeetingSet meetings;
	SlotSet slots;
	SlotGroupSet slotGroups;
};

class GA2 : public Constraint{
public:
	GA2(CType c, int p, std::array<IdList, 2> t_tg_Id1, HomeMode mode1, std::array<IdList, 2> t_tg_Id2, std::array<IdList, 2> s_sg_Id1, std::array<IdList, 2> t_tg_Id3, CompareMode mode2, HomeMode mode3, std::array<IdList, 2> t_tg_Id4, std::array<IdList, 2> s_sg_Id2);
	GA2 (CType c, int p);
	~GA2(){}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams1;
	TeamGroupSet teamGroups1;

	TeamSet teams2;
	TeamGroupSet teamGroups2;

	TeamSet teams3;
	TeamGroupSet teamGroups3;

	TeamSet teams4;
	TeamGroupSet teamGroups4;

	HomeMode hMode1;
	HomeMode hMode2;
	CompareMode cMode;
	SlotSet slots1, slots2;
	SlotGroupSet slotGroups1, slotGroups2;
};

class BR1 : public Constraint{
public:
	BR1(CType c, int p, std::array<IdList, 2> t_tg_Id, int intp, CompareMode mod1, HomeMode mod2, std::array<IdList, 2> s_sg_Id);
	BR1 (CType c, int p);
	~BR1(){}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams;
	TeamGroupSet teamGroups;
	CompareMode cMode;
	int intp;
	HomeMode hMode;
	SlotSet slots;
	SlotGroupSet slotGroups;
};

class BR2 : public Constraint{
public:
	BR2(CType c, int p, std::array<IdList, 2> t_tg_Id, CompareMode mod2, int intp, std::array<IdList, 2> s_sg_Id);
	BR2 (CType c, int p);
	~BR2(){}
	AttrMap serialize();
	ObjCost checkConstr();

	//BreakMode bMode;
	TeamSet teams;
	TeamGroupSet teamGroups;
	CompareMode cMode;
	int intp;
	SlotSet slots;
	SlotGroupSet slotGroups;
	HomeMode hMode = HA;
};

class BR3 : public Constraint{
public:
	BR3(CType c, int p, std::array<IdList, 2> t_tg_Id, BreakMode mod1, HomeMode mod2, int intp);
	BR3 (CType c, int p);
	~BR3(){}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams;
	TeamGroupSet teamGroups;
	BreakMode bMode;
	HomeMode hMode;
	int intp;
};

class BR4 : public Constraint{
public:
	BR4(CType c, int p, CompareMode m, std::array<IdList, 2> t_tg_Id, int min, std::array<IdList, 2> s_sg_Id);
	BR4 (CType c, int p);
	~BR4(){}
	AttrMap serialize();
	ObjCost checkConstr();

	CompareMode cMode;
	TeamSet teams;
	TeamGroupSet teamGroups;
	int min;
	SlotSet slots;
	SlotGroupSet slotGroups;
};

class FA1 : public Constraint{
public:
	FA1(CType c, int p, std::array<IdList, 2> t_tg_Id, int intp, std::array<IdList, 2> s_sg_Id);
	FA1 (CType c, int p);
	~FA1(){}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams;
	TeamGroupSet teamGroups;
	int intp;
	SlotSet slots;
	SlotGroupSet slotGroups;
};

class FA2 : public Constraint{
public:
	FA2(CType c, int p, std::array<IdList, 2> t_tg_Id, HomeMode hmode, int intp, std::array<IdList, 2> s_sg_Id);
	FA2 (CType c, int p);
	~FA2(){}
	AttrMap serialize();
	ObjCost checkConstr();
	
	TeamSet teams;
	TeamGroupSet teamGroups;
	HomeMode hMode;
	int intp;
	SlotSet slots;
	SlotGroupSet slotGroups;
};

class FA3 : public Constraint{
public:
	FA3(CType c, int p, std::array<IdList, 2> t_tg_Id);
	FA3 (CType c, int p);
	~FA3(){}
	AttrMap serialize();
	ObjCost checkConstr();

    TeamSet teams;
    TeamGroupSet teamGroups;
};

class FA4 : public Constraint{
public:
	FA4(CType c, int p, std::array<IdList, 2> t_tg_Id, int intp);
	FA4 (CType c, int p);
	~FA4(){}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams;
	TeamGroupSet teamGroups;
	int intp;
};

class FA5 : public Constraint{
public:
	FA5(CType c, int p, std::array<IdList, 2> t_tg_Id, std::array<IdList, 2> s_sg_Id, int intp);
	FA5 (CType c, int p);
	~FA5(){}
	AttrMap serialize();
	ObjCost checkConstr();

	TeamSet teams;
	TeamGroupSet teamGroups;
	SlotSet slots;
	SlotGroupSet slotGroups;
	int intp;
};

class FA6 : public Constraint{
public:
	FA6(CType c, int p, std::array<IdList, 2> s_sg_Id, int intp);
	FA6 (CType c, int p);
	~FA6(){}
	AttrMap serialize();
	ObjCost checkConstr();

	SlotSet slots;
	SlotGroupSet slotGroups;
	int intp;
};

class SE1 : public Constraint{
public:
	SE1(CType c, int p, std::array<IdList, 2> t_tg_Id, int min);
	SE1 (CType c, int p);
	~SE1(){}
	AttrMap serialize();
	ObjCost checkConstr();

    	TeamSet teams;
    	TeamGroupSet teamGroups;
	int min;
	CMode mode1 = SLOTS;
};

class SE2 : public Constraint{
public:
	SE2(CType c, int p, std::array<IdList, 2> t_tg_Id, SlotPairList slotPairings);
	SE2 (CType c, int p);
	~SE2(){}
	AttrMap serialize();
	ObjCost checkConstr();
	
    TeamSet teams;
    TeamGroupSet teamGroups;
	SlotPairList slotPairings;
};

#endif /* CLASSCONSTRAINT_H */
