NAME		= webserv
SRCS		= srcs/main.cpp
OBJS		= $(SRCS:.cpp=.o)
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
RM			= rm -f

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Building $(NAME)..."
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "Build complete!"

clean:
	@echo "Cleaning object files..."
	@$(RM) $(OBJS)
	@echo "Clean done."

fclean: clean
	@echo "Removing binary..."
	@$(RM) $(NAME)
	@echo "Full clean done."

re: fclean all

leak: all
	valgrind --leak-check=full ./$(NAME)

.PHONY: all clean fclean re leak
