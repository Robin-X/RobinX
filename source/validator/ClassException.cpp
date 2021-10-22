#include "ClassException.h"

RuntimeException::RuntimeException(const std::string& warning, const std::string& file, const int line) { 
	++nrExceptions;
	//rlutil::setColor(rlutil::RED);
	std::cerr << "Exception " << nrExceptions << ":" << std::endl;
	std::cerr << "File: " << file << "\t line: " << line << std::endl;
	//rlutil::resetColor();
	warningMsg = warning; 
}

int RuntimeException::nrExceptions = 0;
