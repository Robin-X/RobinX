#ifndef CLASSEXCEPTION_H
#define CLASSEXCEPTION_H

// Include other
#include "rlutil.h" 	/* setColor, reset color */

// Define a macro to send the line where an exception was thrown
#define throw_line_robinx(e, msg) throw(e(msg, __FILE__, __LINE__));

/****************
*  Base Class  *
****************/

class RuntimeException { // Generic run-time warning
	private:
		std::string warningMsg;
	public:
		RuntimeException(const std::string& warning, const std::string& file, int line);

		// By deriving all warnings from this base class, for any warning w,
		// we can output w's warning message by invoking the inherited what() function
		virtual std::string what() const { return warningMsg; }

		static int nrExceptions; 	// Store the total number of exceptions
};

/***********************
 * Algorithm exception *
 * ********************/
class IPModelException : public RuntimeException {
	public:
		IPModelException(const std::string& msg, const std::string& file, const int line) : RuntimeException(msg, file, line) {}
};

/********************
*  Parse Exceptions  *
********************/
// For warnings that occur during data parsing: use one of the classes below

class ParseException : public RuntimeException {
	public:
		ParseException(const std::string& msg, const std::string& file, const int line) : RuntimeException(msg, file, line) {}
};

// Exceptions generated during reading by XML parser
class XmlValidationException : public ParseException {
	public:
		XmlValidationException(const std::string& msg, const std::string& file, const int line) : ParseException(msg, file, line) {}
};

class XmlReadingException : public ParseException {
	public:
		XmlReadingException(const std::string& msg, const std::string& file, const int line) : ParseException(msg, file, line) {}
};

// Exceptions generated during reading by factory class
class InterfaceReadingException : public ParseException {
	public:
		InterfaceReadingException(const std::string& msg, const std::string& file, const int line) : ParseException(msg, file, line) {}
};

/********************
*  Logic Exceptions  *
********************/
// Runtime errors are beyond the control of programmer, for example, a "network service unavailable" error.
// Logic errors are caused by programming mistakes, for example, an "index out of range" error.

class LogicException : public RuntimeException {
	public:
		LogicException(const std::string& msg, const std::string& file, const int line) : RuntimeException(msg, file, line) {}
};

class InvalidArgumentException : public LogicException {
	public:
		InvalidArgumentException(const std::string& msg, const std::string& file, const int line) : LogicException(msg, file, line) {}
};

class OutOfBoundsException : public LogicException {
	public:
		OutOfBoundsException(const std::string& msg, const std::string& file, const int line) : LogicException(msg, file, line) {}
};

class FileNotFoundException : public LogicException {
	public:
		FileNotFoundException(const std::string& msg, const std::string& file, const int line) : LogicException(msg, file, line) {}
};

#endif /* CLASSEXCEPTION_H */

