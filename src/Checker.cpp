#include "Checker.hpp"

void	Checker::trim(std::string *str)
{
	(*str).erase(0, (*str).find_first_not_of(" \t"));
	(*str).erase((*str).find_last_not_of(" \t") + 1);
}

void	Checker::trimComments(std::string *str)
{
	size_t pos = (*str).find("#");
	if (pos != std::string::npos)
		str->erase(pos);
}

bool 	Checker::errNumbers(long long int num)
{
	int numbers[] = {200, 201, 204, 301, 302, 303, 307, 400, 401, 403, 404, 405, 409, 413, 414, 415, 429, 500, 501, 502, 503, 504, 505};
	std::vector<int> errNumbers(numbers, numbers + sizeof(numbers) / sizeof(int));
	return (std::find(errNumbers.begin(), errNumbers.end(), num) != errNumbers.end());
}

bool 	Checker::validPath(const std::string& path)
{
	struct stat info;
	return (stat(path.c_str(), &info) == 0);
}

bool 	Checker::validSysPath(const std::string& path)
{
	return (access(path.c_str(), X_OK) == 0);
}

bool 	Checker::isBlankOrEmpty(const std::string& line)
{
	if (line.empty())
		return true;

	for (size_t i = 0; i < line.size(); i++)
		if (line[i] < 9 || (line[i] > 13 && line[i] != 32))
			return false;
	return true;
}

void	Checker::runTime(const std::string& error, const std::string& line)
{
	if (DEBUG_CHECK) { std::cout << ORNG << line << RST << std::endl; }
	throw std::runtime_error(error);
}

void	Checker::listenCheck(const std::string& line)
{
	size_t pos = line.find_last_of(32);
	if (pos != std::string::npos)
	{
		std::string arg = line.substr(pos + 1);
		if (!arg.empty() && arg[arg.size() - 1] == ';')
		{
			arg.erase(arg.size() - 1);
			for (size_t i = 0; i < arg.size(); i++)
				if (!isdigit(arg[i]))
					runTime("Invalid port", line);
			long long int port = std::stoll(arg);
			if (port > 65535)
				runTime("Invalid port", line);
		}
		else
			runTime("Invalid port", line);
	}
	else
		runTime("Invalid port", line);
}

void 	Checker::nameCheck(const std::string& line)
{
	size_t pos = line.find_last_of(32);
	if (pos != std::string::npos)
	{
		std::string arg = line.substr(pos + 1);
		if (!arg.empty() && arg[arg.size() - 1] == ';')
		{
			arg.erase(arg.size() - 1);
			if (arg != "localhost" && arg != "127.0.0.1")
				runTime("Invalid host name", line);
		}
		else
			runTime("Invalid host name", line);
	}
	else
		runTime("Invalid host name", line);
}

void 	Checker::rootCheck(const std::string& line)
{
	size_t pos = line.find_last_of(32);
	if (pos != std::string::npos)
	{
		std::string arg = line.substr(pos + 1);
		if (!arg.empty() && arg[arg.size() - 1] == ';')
		{
			arg.erase(arg.size() - 1);
			if (!validPath(arg))
				runTime("Invalid root", line);
			else
				_root = arg;
		}
		else
			runTime("Invalid root", line);
	}
	else
		runTime("Invalid root", line);
}

void	Checker::indexCheck(const std::string& line)
{
	size_t pos = line.find_last_of(32);
	if (pos != std::string::npos)
	{
		std::string arg = line.substr(pos + 1);
		if (!arg.empty() && arg[arg.size() - 1] == ';')
		{
			arg.erase(arg.size() - 1);
			if (!_root.empty())
			{
				arg = _root + "/" + arg;
				if (!validPath(arg))
					runTime("Invalid index", line);
			}
			else
				if (!validPath(arg))
					runTime("Invalid index", line);
		}
		else
			runTime("Invalid index", line);
	}
	else
		runTime("Invalid index", line);
}

void	Checker::maxBodySize_Check(const std::string& line)
{
	size_t pos = line.find_last_of(32);
	if (pos != std::string::npos)
	{
		std::string arg = line.substr(pos + 1);
		if (!arg.empty() && arg[arg.size() - 1] == ';')
		{
			arg.erase(arg.size() - 1);
			for (size_t i = 0; i < arg.size(); i++)
				if (!isdigit(arg[i]))
					runTime("Invalid max_body_size", line);
		}
		else
			runTime("Invalid max_body_size", line);
	}
	else
		runTime("Invalid max_body_size", line);
}

void	Checker::errorPage_Check(const std::string& line)
{
		std::string keyword, code, path;
		std::istringstream iss(line);
		if (!(iss >> keyword >> code >> path))
		runTime("Invalid Params error_page", line);
	
	for (size_t i = 0; i < code.size(); i++)
		if (!isdigit(code[i]))
			runTime("Invalid Code error_page", line);
	long long int num = stoll(code);
	if (!errNumbers(num))
		runTime("Invalid Code error_page", line);

	if (path[path.size() - 1] == ';')
	{
		path.erase(path.size() - 1);
		if (!validPath(path))
			runTime("Invalid Path error_page", line);
	}
	else
		runTime("Invalid Params error_page", line);
}

void 	Checker::listingCheck(const std::string& line)
{
	size_t pos = line.find_last_of(32);
	if (pos != std::string::npos)
	{
		std::string arg = line.substr(pos + 1);
		if (!arg.empty() && arg[arg.size() - 1] == ';')
		{
			arg.erase(arg.size() - 1);
			if (arg != "on" && arg != "off")
				runTime("Invalid Interruptor listing", line);
		}
		else
			runTime("Invalid Interruptor listing", line);
	}
	else
		runTime("Invalid listing", line);
}

void 	Checker::locationCheck(const std::string& line, std::ifstream *ifs)
{
	std::string keyword, directory, openAccolade;
	std::istringstream iss(line);
	if (!(iss >> keyword >> directory >> openAccolade))
		runTime("Invalid location", line);

	if (directory == "/redirect")
		;
	else if (!_root.empty())
	{
		directory = _root + directory;
		if (!validPath(directory))
			runTime("Invalid location", line);
	}
	else
		if (!validPath(directory))
			runTime("Invalid location", line);
	_location = directory;
	
	if (openAccolade != "{")
		runTime("Invalid location", line);
	
	std::string str;
	while (getline(*ifs, str))
	{
		trimComments(&str);
		trim(&str);
		if (str.empty())
			continue;
		if (str == "}")
			break ;
		if (str[str.size() - 1] != ';')
			runTime("Invalid Params in location", str);
		str.erase(str.size() - 1);
		
		if (str.find("methods") == 0)
		{
			std::istringstream ss(str);
			std::string word;
			if (!(ss >> word) || word != "methods")
				runTime("Invalid Methods in location", str);

			while (ss >> word)
				if (word != "GET" && word != "POST" && word != "DELETE")
					runTime("Invalid Methods in location", str);
		}
		else if (str.find("return") == 0)
		{
			std::istringstream ss(str);
			std::string params;
			if (!(ss >> params) || params != "return")
				runTime("Invalid Params in location", str);
			
			if (!(ss >> params))
				runTime("Invalid Params in location", str);
			for (size_t i = 0; i < params.size(); i++)
				if (!isdigit(params[i]))
					runTime("Invalid Code return in location", str);
			long long int num = stoll(params);
			if (!errNumbers(num))
				runTime("Invalid Code return in location", str);

			if (!(ss >> params))
				runTime("Invalid Params in location", str);
			if (!_root.empty())
			{
				params = _root + params;
				if (!validPath(params))
					runTime("Invalid Path in location", str);
			}
			else
				if (!validPath(params))
					runTime("Invalid Path in location", str);
		}
		else if (str.find("index") == 0)
		{
			size_t pos = str.find_last_of(32);
			std::string arg = str.substr(pos + 1);
			std::string path = _location + "/" + arg;
			if (!validPath(path))
				runTime("Invalid Index in location", str);
		}
		else if (str.find("cgi_path") == 0)
		{
			std::istringstream ss(str);
			std::string pathCgi;
			if (!(ss >> pathCgi) || pathCgi != "cgi_path")
				runTime("Invalid Params in location", str);
			while (ss >> pathCgi)
				if (!validSysPath(pathCgi))
					runTime("Invalid Cgi Path", str);
		}
		else if (str.find("cgi_ext") == 0)
		{
			std::istringstream ss(str);
			std::string extCgi;
			if (!(ss >> extCgi) || extCgi != "cgi_ext")
				runTime("Invalid Params in location", str);
			while (ss >> extCgi)
				if (extCgi != ".py" && extCgi != ".php" && extCgi != ".sh")
					runTime("Invalid Cgi Extension", str);
		}
		else
			runTime("Configuration File Corrupted in location", str);
	}
}

Checker::Checker(const std::string& config)
{
	if (config.size() > 5 && config.substr(config.size() - 5) == ".conf")
	{
		std::ifstream ifs(config);
		if (!ifs.is_open())
			throw std::runtime_error("Impossible to open the file");

		std::string line;
		while (getline(ifs, line))
		{
			trimComments(&line);
			trim(&line);
			if (line.find("server") == 0 || line.find("}") == 0)
				continue;
			else if (line.find("listen") == 0)
				listenCheck(line);
			else if (line.find("name") == 0)
				nameCheck(line);
			else if (line.find("root") == 0)
				rootCheck(line);
			else if (line.find("index") == 0)
				indexCheck(line);
			else if (line.find("max_body_size") == 0)
				maxBodySize_Check(line);
			else if (line.find("error_page") == 0)
				errorPage_Check(line);
			else if (line.find("listing") == 0)
				listingCheck(line);
			else if (line.find("location") == 0)
				locationCheck(line, &ifs);
			else if (isBlankOrEmpty(line))
				continue;
			else
				runTime("Configuration File Corrupted", line);
		}
		ifs.close();
	}
	else
		throw std::runtime_error("Invalid file, expected : '.conf'");
	if (DEBUG_CHECK) { std::cout << LIME << "Configuration to standards" << RST << std::endl; }
};
