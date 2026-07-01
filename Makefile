NAME		= webserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

HEADER_DIR 	= headers
SRCS_DIR	= srcs
OBJ_DIR		= obj

SRCS_FILES	= srcs/main.cpp
SRCS		= $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))
OBJ			= $(addprefix $(OBJ_DIR)/, $(SRCS_FILES:.cpp=.o))
CXXFLAGS 	+= -I $(HEADER_DIR)

G = \033[0;32m
R = \033[0;31m
Y = \033[0;33m
B = \033[0;34m
RES = \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(Y)--- Finalizing Compilation ---$(RES)"
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(G)✅ $(NAME) created successfully!$(RES)"

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "$(B)🔨 Compiling: $< $(RES)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "$(Y)Cleaning objects...$(RES)"
	@rm -rf $(OBJ_DIR)
	@echo "$(G)Objects cleaned.$(RES)"

fclean: clean
	@echo "$(Y)Removing $(NAME)...$(RES)"
	@rm -f $(NAME)
	@echo "$(G)Cleanup complete.$(RES)"

debug: CXXFLAGS += -g
debug: re

re: fclean all

.PHONY: all clean fclean re debug
