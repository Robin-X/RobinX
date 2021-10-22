#include "ClassMeeting.h"

Meeting::Meeting(Team* t1, Team* t2, bool mode, int slot) : team1(t1), team2(t2), noHome(mode) {
	if (slot >= 0) { assignedSlot = Instance::get()->getSlot(slot); }
}

// Comparator functions
// Sort meetings in increasing scheduled slot
bool compMeetingScheduledSlot(const Meeting* meeting1, const Meeting* meeting2){
	return meeting1->getAssignedSlot()->getId() < meeting2->getAssignedSlot()->getId(); 
}
// All meetings with noHome = false are upfront
bool compMeetingNoHome(const Meeting* meeting1, const Meeting* meeting2){
	return meeting1->getNoHome() < meeting2->getNoHome(); 
}
