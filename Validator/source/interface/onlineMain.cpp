// Include std
#include <iostream>

// Include other
#include "ClassInterface.h"

// Input parser class, see: https://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c#868894
class InputParser{
    public:
        InputParser (int &argc, char **argv){
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }
        /// @author iain
        const std::string& getCmdOption(const std::string &option) const{
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }
        /// @author iain
        bool cmdOptionExists(const std::string &option) const{
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }
    private:
        std::vector <std::string> tokens;
};


int main(int argc, char *argv[])
{
	// Display welcome text
	std::string welcomeText = "RobinX Validator 2.0\nGNU General Public License v3.0.\nSee the README for more details or type '-h' for more help.\n";
	std::cout << welcomeText << std::endl;

	// Initialize arguments
	std::string instanceFile = "";
	std::string instanceString = "";
	std::string solutionFile = "";
	std::string solutionString = "";

	// Read arguments
	bool validation = true; // Set to true if enough information to validate.
	InputParser input(argc, argv);
	if(input.cmdOptionExists("-h")){
		std::cout << "Help argument enabled." << std::endl;
		std::cout << "The following arguments are available:" << std::endl;
		const int width = 10;
		std::cout << std::left << std::setw(width) << "-i" << "Name of the instance XML file." << std::endl;
		std::cout << std::left << std::setw(width) << "-s" << "Name of the solution XML file. If no instance file is given, RobinX retrieves instance path given in solution file." << std::endl;
		std::cout << std::left << std::setw(width) << "-iString" << "String without spaces giving the content of the instance XML file." << std::endl;
		std::cout << std::left << std::setw(width) << "-sString" << "String without spaces giving the content of the solution XML file. If no instance file is given, RobinX retrieves instance path given in solution file." << std::endl;
		std::cout << std::left << std::setw(width) << "-h" << "Print the content of this help message." << std::endl;
		return 0;
	}

	try {
		bool readIn = false; // True if instance path need to be retrieved from the solution file
		if(input.cmdOptionExists("-i")){
			assert(!input.cmdOptionExists("-iString"));
			instanceFile = input.getCmdOption("-i");
			Interface::get()->readInstanceXml(instanceFile);
		} else if (input.cmdOptionExists("-iString")) {
			instanceString = input.getCmdOption("-iString");	
			Interface::get()->readInstanceXmlString(instanceString);
		} else {
			// No instance specified
			readIn = true;
		}

		if(input.cmdOptionExists("-s")){
			assert(!input.cmdOptionExists("-sString"));
			solutionFile = input.getCmdOption("-s");
			Interface::get()->readSolutionXml(solutionFile, "", "", readIn);
		} else if (input.cmdOptionExists("-sString")){
			solutionString = input.getCmdOption("-sString");
			Interface::get()->readSolutionXmlString(solutionString, "", "", readIn);
		} else {
			std::stringstream msg;
			msg << "No solution file given." << std::endl;
			throw_line_robinx(InterfaceReadingException, msg.str());
		}	
	}catch(InterfaceReadingException e) {
		std::cerr << e.what() << std::endl;
		validation = false;
	}

	// Validate
	if (validation) {
		// TODO Set to false to have verbose mode, not silent
		ObjCost obj = Interface::get()->checkConstr(true);
		std::cout << "       -------------------------------------------------------------------" << std::endl;
		std::cout << std::setw(30) << std::left << "       Objective:"  << std::setw(20) << obj.first << std::setw(10) << obj.second << std::right << std::endl;
		std::cout << "       -------------------------------------------------------------------" << std::endl;
		return obj.second;
	} else {
		std::cout << "Some error occurred. Validation was canceled."  << std::endl;
		return 0;
	}
}
