#include "utils.hpp"

std::string	strToHtml(std::string content)
{
	std::string	html = "<html>\n";
	html += "<head>\n";
	html += "<title>Webserver</title>\n";
	html += "<link rel=\"icon\" href=\"" FAVICON "\" type=\"image/x-icon\"/>\n";
	html += "</head>\n";
	html += "<body>\n";
	html += "<h1>";
	html += content;
	html += "</h1>\n";
	html += "</body>\n";
	html += "</html>\n";
	return (html);
}

std::string	fileToStr(const std::string &path)
{
	std::ifstream	file(path.c_str(), std::ios::binary);
	if (file.is_open())
	{
		std::string	content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
		return (content);
	}
	return ("");
}

std::string	getContentType(const std::string &fileExtension)
{
	static std::map<std::string, std::string>	contentTypes;
	contentTypes[".html"] = "text/html";
	contentTypes[".css"] = "text/css";
	contentTypes[".js"] = "text/javascript";
	contentTypes[".jpg"] = "image/jpeg";
	contentTypes[".jpeg"] = "image/jpeg";
	contentTypes[".png"] = "image/png";
	contentTypes[".gif"] = "image/gif";
	contentTypes[".bmp"] = "image/bmp";
	contentTypes[".ico"] = "image/x-icon";
	contentTypes[".svg"] = "image/svg+xml";
	contentTypes[".pdf"] = "application/pdf";
	contentTypes[".zip"] = "application/zip";
	contentTypes[".tar"] = "application/x-tar";
	contentTypes[".gz"] = "application/gzip";
	contentTypes[".mp3"] = "audio/mpeg";
	contentTypes[".wav"] = "audio/wav";
	contentTypes[".mp4"] = "video/mp4";
	contentTypes[".avi"] = "video/x-msvideo";
	contentTypes[".mpeg"] = "video/mpeg";
	contentTypes[".webm"] = "video/webm";
	contentTypes[".txt"] = "text/plain";
	contentTypes[".csv"] = "text/csv";
	contentTypes[".xml"] = "text/xml";
	contentTypes[".json"] = "application/json";
	contentTypes[".xhtml"] = "application/xhtml+xml";

	if (contentTypes.find(fileExtension) != contentTypes.end())
		return (contentTypes.at(fileExtension));
	else
		return ("application/octet-stream");// par defaut selon rfc 2046
}

std::string	getFileExtension(const std::string &path)
{
	size_t	pos = path.find_last_of(".");
	if (pos != std::string::npos)
		return (path.substr(pos));
	return ("");
}

std::map<std::string, std::string>	getDirContent(const std::string &path)
{
	// Retourne une map avec tout le contennu du dossier (fichier ou dossier) sauf les fichiers cachés
	std::map<std::string, std::string>	dirContent;
	DIR*	dir = opendir(path.c_str());
	if (dir)
	{
		struct dirent*	entry;
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_name[0] != '.')
			{
				std::string	fileName = entry->d_name;
				std::string	filePath = path + "/" + fileName;
				struct stat	fileStat;
				if (stat(filePath.c_str(), &fileStat) == 0)
				{
					if (S_ISDIR(fileStat.st_mode))
						dirContent[fileName] = "dir";
					else if (S_ISREG(fileStat.st_mode))
						dirContent[fileName] = "file";
				}
			}
		}
		closedir(dir);
	}
	return (dirContent);
}

std::string	addFaviconToResponse(const std::string& response)
{
	std::string 			iconLink = "<link rel=\"icon\" href=\"" FAVICON "\" type=\"image/x-icon\"/>\n";
	std::string::size_type	headEnd = response.find("</head>");

	if (headEnd != std::string::npos)
	{
		// Insérer l'icône juste avant la fermeture de la balise </head>
		return (response.substr(0, headEnd) + iconLink + response.substr(headEnd));
	}
	else
	{
		// Si pas de balise </head> trouvée, ajouter l'icône au début du document
		return (iconLink + response);
	}
}

std::string	getFileExtensionFromContentType(const std::string &contentType)
{
    if (contentType == "image/jpeg") return ".jpg";
    if (contentType == "image/png") return ".png";
    if (contentType == "image/gif") return ".gif";
    if (contentType == "text/html") return ".html";
    if (contentType == "text/plain") return ".txt";
    if (contentType == "application/pdf") return ".pdf";
	if (contentType == "application/zip") return ".zip";
	if (contentType == "application/x-tar") return ".tar";
	if (contentType == "application/gzip") return ".gz";
	if (contentType == "audio/mpeg") return ".mp3";
	if (contentType == "audio/wav") return ".wav";
	if (contentType == "video/mp4") return ".mp4";
	if (contentType == "video/x-msvideo") return ".avi";
	if (contentType == "video/mpeg") return ".mpeg";
	if (contentType == "video/webm") return ".webm";
	if (contentType == "text/csv") return ".csv";
	if (contentType == "text/xml") return ".xml";
	if (contentType == "application/json") return ".json";
	if (contentType == "application/xhtml+xml") return ".xhtml";
	return "";
}

std::string	generateRandomFilename()
{
	std::string	chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string	filename = "user_";
	for (int i = 0; i < 10; i++)
		filename += chars[rand() % chars.size()];
	return (filename);
}

std::string	convertBodyToFileContent(const std::string &body)
{
	std::string fileContent;
    std::string delimiter;
    std::string endDelimiter;

    // Find the boundary delimiter
    size_t boundaryPos = body.find("boundary=");
    if (boundaryPos == std::string::npos) {
        return body; // No boundary found, return the entire body as file content
    }
    boundaryPos += 9; // Skip "boundary="
    size_t boundaryEndPos = body.find("\r\n", boundaryPos);
    if (boundaryEndPos == std::string::npos) {
        return body; // Invalid boundary format, return the entire body as file content
    }
    delimiter = "--" + body.substr(boundaryPos, boundaryEndPos - boundaryPos);
    endDelimiter = delimiter + "--";

    // Find the start of the file content
    size_t fileStartPos = body.find(delimiter, boundaryEndPos);
    if (fileStartPos == std::string::npos) {
        return body; // No file content found, return the entire body as file content
    }
    fileStartPos += delimiter.size();
    fileStartPos = body.find("\r\n\r\n", fileStartPos);
    if (fileStartPos == std::string::npos) {
        return body; // Invalid format, return the entire body as file content
    }
    fileStartPos += 4; // Skip the header terminator

    // Find the end of the file content
    size_t fileEndPos = body.find("\r\n" + delimiter, fileStartPos);
    if (fileEndPos == std::string::npos) {
        fileEndPos = body.find("\r\n" + endDelimiter, fileStartPos);
        if (fileEndPos == std::string::npos) {
            return body; // Invalid format, return the entire body as file content
        }
    }

    // Extract the file content
    fileContent = body.substr(fileStartPos, fileEndPos - fileStartPos);
    return fileContent;
}

std::vector<std::string>	split(const std::string &str, const std::string &delim)
{
	std::vector<std::string>	tokens;
	size_t						start = 0;
	size_t						end = str.find(delim);

	while (end != std::string::npos)
	{
		tokens.push_back(str.substr(start, end - start));
		start = end + delim.size();
		end = str.find(delim, start);
	}
	tokens.push_back(str.substr(start, end));
	return (tokens);
}
