#pragma once

#include "lib.hpp"

class Checker
{
	private:

		std::string 	_root;
		std::string 	_location;

		// Utils
		void	trim(std::string *str);
		void 	trimComments(std::string *str);
		bool 	errNumbers(long long int num);
		bool 	validPath(const std::string& path);
		bool 	validSysPath(const std::string& path);
		bool 	isBlankOrEmpty(const std::string& line);

		// Error
		void	runTime(const std::string& error, const std::string& line);

		// Checks
		void	listenCheck(const std::string& line);
		void 	nameCheck(const std::string& line);
		void 	rootCheck(const std::string& line);
		void	indexCheck(const std::string& line);
		void	maxBodySize_Check(const std::string& line);
		void	errorPage_Check(const std::string& line);
		void 	listingCheck(const std::string& line);
		void 	locationCheck(const std::string& line, std::ifstream *ifs);

	public:
		Checker(const std::string& config);
		~Checker() {};
};
