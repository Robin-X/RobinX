/********************************
*  CLASS TINYXML: PARSER  *
********************************/

/* 
 * Provide functionality to parse instance and solution
 * XML file with tinyxml2.
 */

// Include std
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>    // runtime_error
#include <fstream>

// Include tinyxml2 parser
#include "tinyxml2.h"

#ifndef TINYPARSER_H
#define TINYPARSER_H

// Include other
#include "ClassInterface.h"
#include "ClassException.h"

// Macro to check success of XML function
// See: https://shilohjames.wordpress.com/2014/04/27/tinyxml2-tutorial/
#ifndef XMLCheckResult
	#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { std::cout << "Error: " << a_eResult << std::endl; }
#endif

class TinyParser
{

public:
	// constructors
	TinyParser() {};
	~TinyParser() { delete doc; }	


	// Read file or string content
	void readFile(const std::string fileName);
	void readString(const std::string content);

	// Save doc
	void saveXml(const std::string docName);

	// Auxiliary functions
	int getIntAttr(const tinyxml2::XMLElement* el, std::string attrName);
	std::string getStringAttr(const tinyxml2::XMLElement* el, std::string attrName);

//	// Try to open XML file
//	// If a string is given: parse string instead of file name
//	void initializetinyXML2(const std::string str="");
//
//	// From XML to data structure
	void deserializeInstance();
	void deserializeSolution(const bool readIn=false);

//	// From data structure to XML
//	void serializeInstance(std::string fileName);
//	void serializeSolution(std::string fileName);
//
//	/*******************
//	*  Serialization  *
//	*******************/
	// From data structure to XML
	void serializeInstance(std::string fileName);
	void serializeSolution(std::string fileName);
	void serializeBound(std::string fileName);

	
	tinyxml2::XMLElement* serialize(AttrMap attrs);
	void serializeGames();


public:
	// Type here all protected member functions
	
	/***********************
	 * Auxiliary functions *
	 * *********************/
	std::array<IdList, 2> readTeamTags(const tinyxml2::XMLElement* c, int nr = -1);
	std::array<IdList, 2> readSlotTags(const tinyxml2::XMLElement* c, int nr = -1);

	// Detokenize a colon separated list of integers
	std::list<int> detokenizeIntString(std::string str);

	// Detokenize a colon separated list of meetings
	MeetingIdList detokenizeMeetings(std::string str);

	// Detokenize a colon seperated list of slot pairs
	SlotPairList detokenizeSlotPairs(std::string str);
		
	// Add a node
	void addChildNode(tinyxml2::XMLElement* parentNode, std::string childStr, std::string content="");

	/*********************
	*  Deserialization  *
	*********************/
	// Instance
	void readMetaData();
	void readData();
	void readDistances();
	void readCOEWeights();
	void readCosts();
	void readObjectiveFunction();
	void readLeagues();
	void readSlots();
	void readSlotGroups();
	void readTeams();
	void readTeamGroups();
	void readAdditionalGames();
	void readConstr(); 		// Should be last!	
	void readBaseConstr();
	void readBA1();
	void readCapacityConstr();
	void readCA1();
	void readCA2();
	void readCA3();
	void readCA4();
	void readCA5();
	void readGameConstr();
	void readGA1();
	void readGA2();
	void readBreakConstr();
	void readBR1();
	void readBR2();
	void readBR3();
	void readBR4();
	void readFairnessConstr();
	void readFA1();
	void readFA2();
	void readFA3();
	void readFA4();
	void readFA5();
	void readFA6();
	void readSeparationConstr();
	void readSE1();
	void readSE2();

	// Solution
	void readMetaDataSol();
	void readGames(); 

public:
	// Type here all protected member variables
	std::string xmlFileName;
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument(); // Can be large: put on heap
};

#endif /* TINYPARSER_H */
