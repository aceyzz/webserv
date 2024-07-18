# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cedmulle <42-xvi@protonmail.com>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/02 10:29:14 by cedmulle          #+#    #+#              #
#    Updated: 2024/07/18 08:38:22 by cedmulle         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#								VARIABLES									   #
# **************************************************************************** #
NAME		= webserv

SRC_DIR		= src/
OBJ_DIR		= obj/
INC_DIR		= inc/

SRC			= $(SRC_DIR)CgiHandler.cpp \
			  $(SRC_DIR)Config.cpp \
			  $(SRC_DIR)Logger.cpp \
			  $(SRC_DIR)main.cpp \
			  $(SRC_DIR)ParseConfig.cpp \
			  $(SRC_DIR)Request.cpp \
			  $(SRC_DIR)Response.cpp \
			  $(SRC_DIR)Route.cpp \
			  $(SRC_DIR)Socket.cpp \
			  $(SRC_DIR)utils.cpp \
			  $(SRC_DIR)Webserv.cpp

OBJ			= $(addprefix $(OBJ_DIR), $(notdir $(SRC:.cpp=.o)))

INC			= $(INC_DIR)Cgi.hpp \
				$(INC_DIR)CgiHandler.hpp \
				$(INC_DIR)colors.hpp \
				$(INC_DIR)Config.hpp \
				$(INC_DIR)lib.hpp \
				$(INC_DIR)Logger.hpp \
				$(INC_DIR)ParseConfig.hpp \
				$(INC_DIR)Request.hpp \
				$(INC_DIR)Response.hpp \
				$(INC_DIR)Route.hpp \
				$(INC_DIR)Socket.hpp \
				$(INC_DIR)utils.hpp \
				$(INC_DIR)Webserv.hpp

RM			= rm -rf
CC			= c++
CFLAGS		= -Wall -Werror -Wextra -std=c++98 -I$(INC_DIR)

RED			= \033[1;31m
GRN			= \033[1;32m
YEL			= \033[1;33m
BLU			= \033[1;34m
MAG			= \033[1;35m
CYN			= \033[1;36m
GRY			= \033[1;90m
CLR			= \033[0K
RST			= \033[0m

# **************************************************************************** #
#							   MANDATORY RULES								   #
# **************************************************************************** #
all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(CYN)Compiled $(YEL)$(NAME)$(RST)\n\n"
	@echo "$(YEL)Usage: ./$(NAME) <config_file>$(RST)"
	@echo "$(GRY)or leave it empty to use the default config file$(RST)\n"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(INC)
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(GRN)Compiled $(YEL)$<$(RST)"

clean: stop_webserver
	@$(RM) $(OBJ_DIR)
	@echo "$(RED)Removed $(YEL)objects$(RST)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Removed $(YEL)$(NAME)$(RST)"

re: fclean all

.PHONY: all clean fclean re

# **************************************************************************** #
#							      FUNCTIONS									   #
# **************************************************************************** #
stop_webserver:
	@PID_LIST=`pgrep -f './webserv'` ; \
	for pid in $$PID_LIST ; do \
		kill -9 $$pid ; \
	done
	@echo "$(RED)Stopped $(YEL)webserv$(RST)"

kill:
	@read -p "Donner les numeros de ports des process a kill (séparé par un espace): " ports; \
	for port in $$ports; do \
		echo "$(GRY)Searching and terminating processes on port $$port...$(RST)"; \
		lsof -i :$$port -t | xargs -r kill; \
	done; \
	echo "$(GRN)Processes terminated.$(RST)"

restart: stop_webserver kill all

cleanlog:
	@rm -rf log/*.log
	@echo "$(RED)Removed $(YEL)logs$(RST)"

leaks: all
	@./tools/run_leaks.sh
