#!/bin/bash

run_leaks() {
	trap '' SIGINT SIGTERM
	leaks --atExit -- ./webserv ./config/config.conf 2>/dev/null
	trap - SIGINT SIGTERM
}

run_leaks
