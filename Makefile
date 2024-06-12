# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cedmulle <42-xvi@protonmail.com>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/02 10:29:14 by cedmulle          #+#    #+#              #
#    Updated: 2024/06/12 13:45:37 by cedmulle         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#								VARIABLES									   #
# **************************************************************************** #
NAME		= webserv
SRC_DIR		= src/
OBJ_DIR		= obj/
INC_DIR		= inc/
SRC			= $(wildcard $(SRC_DIR)*.cpp)
OBJ			= $(addprefix $(OBJ_DIR), $(notdir $(SRC:.cpp=.o)))
INC			= $(wildcard $(SRC_DIR)*.hpp)
RM			= rm -rf
CC			= c++
CFLAGS		= -std=c++98 -I$(INC_DIR)

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
#									RULES									   #
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

clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(RED)Removed $(YEL)objects$(RST)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Removed $(YEL)$(NAME)$(RST)"

re: fclean all

.PHONY: all clean fclean re
