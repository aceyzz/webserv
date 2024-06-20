#!/bin/bash

run_leaks() {
	trap '' SIGINT SIGTERM
	leaks --atExit -- ./webserv config/config.conf
	trap - SIGINT SIGTERM
}

run_leaks
