#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

#include <csignal>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include "colors.hpp"
#include "utils.hpp"

#define DEBUG 0
#define MAX_EVENTS 1000
#define MAX_LISTEN 1000
#define MAX_URI_SIZE 2000
#define BUFFER_SIZE 1024
#define CHUNK_SIZE 4096
#define HTTP_VERSION "HTTP/1.1"
#define ISFILE 1
#define ISDIR 2
#define FAVICON "icon.ico"
#define TIME_TIMEOUT 10
