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
#include <vector>

#include "colors.hpp"

# define DEBUG 1
# define MAX_EVENTS 100
# define MAX_URI_SIZE 4096
