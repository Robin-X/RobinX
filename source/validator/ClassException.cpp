#include "ClassException.h"
#include <sstream>

RuntimeException::RuntimeException(const std::string& warning, const std::string& file, const int line) { 
	++nrExceptions;

	std::stringstream ss;
	ss << "[Exception " << nrExceptions << "] "
		<< "[" << file << ":" << line << "] "
		<< warning;

	warningMsg = ss.str();
}

int RuntimeException::nrExceptions = 0;
