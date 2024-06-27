#pragma once

#include "Webserv.hpp"

std::string							strToHtml(std::string str);
std::string							fileToStr(const std::string &path);
std::string							getContentType(const std::string &fileExtension);
std::string							getFileExtension(const std::string &path);
std::map<std::string, std::string>	getDirContent(const std::string &path);
std::string							addFaviconToResponse(const std::string& response);
std::string							getFileExtensionFromContentType(const std::string &contentType);
std::string							generateRandomFilename();
std::string							convertBodyToFileContent(const std::string &body);
