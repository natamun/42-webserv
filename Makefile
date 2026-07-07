NAME		= webserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

HEADER_DIR 	= includes
SRCS_DIR	= srcs
OBJ_DIR		= obj

SRCS_FILES	= main.cpp \
			  WebServer.cpp \
			  Client.cpp \
			  ConfigParser.cpp \
			  HttpRequest.cpp \
        HttpResponse.cpp
SRCS		= $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))
OBJ			= $(addprefix $(OBJ_DIR)/, $(SRCS_FILES:.cpp=.o))

HANDLER_FILES	= GetHandler.cpp \
				  PostHandler.cpp \
				  DeleteHandler.cpp

SRCS		= $(addprefix $(SRCS_DIR)/, $(SRCS_FILES)) \
			  $(addprefix $(SRCS_DIR)/handlers/, $(HANDLER_FILES))

OBJ			= $(addprefix $(OBJ_DIR)/, $(SRCS_FILES:.cpp=.o)) \
			  $(addprefix $(OBJ_DIR)/handlers/, $(HANDLER_FILES:.cpp=.o))

TEST_NAME		= webserv_test
SRC_NO_MAIN		= $(filter-out $(SRCS_DIR)/main.cpp, $(SRCS))

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
	@mkdir -p $(@D)
	@echo "$(B)🔨 Compiling: $< $(RES)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "$(Y)Cleaning objects...$(RES)"
	@rm -rf $(OBJ_DIR)
	@echo "$(G)Objects cleaned.$(RES)"

fclean: clean
	@echo "$(Y)Removing $(NAME)...$(RES)"
	@rm -f $(NAME) $(TEST_NAME)
	@echo "$(G)Cleanup complete.$(RES)"

debug: CXXFLAGS += -g
debug: re

re: fclean all

# Usage: make test FILE=test_name
test:
	@echo "$(Y)--- Compiling test: $(FILE) ---$(RES)"
	@$(CXX) $(CXXFLAGS) $(SRC_NO_MAIN) $(FILE) -o $(TEST_NAME)
	@echo "$(G)✅ $(TEST_NAME) created!$(RES)"
	@./$(TEST_NAME)

.PHONY: all clean fclean re debug test
