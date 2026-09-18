# --- Colors ---
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BOLD = \033[1m
RESET = \033[0m
CLEAR = \033[2K\r

# --- Variables ---
NAME = a.out
CXX = c++
CPPFLAGS =
INCLUDES = -I./include
LFLAGS = -lncurses
SRC = ./src/main.cpp \
	  ./src/Game.cpp \
	  ./src/Player.cpp

OBJ = $(SRC:%.cpp=obj/%.o)

all: $(NAME)
	@printf "$(GREEN)$(BOLD)$(NAME) done!$(RESET)\n"

$(NAME): $(OBJ)
	@printf "$(CLEAR)$(YELLOW)linking $(NAME)...$(RESET)\n"
	@$(CXX) $(OBJ) $(LFLAGS) -o $(NAME)

obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	@printf "[$(GREEN)$(BOLD) OK $(RESET)$(BOLD)]$(RESET) compiling $(BOLD)$@...$(RESET)$(CLEAR)"
	@$(CXX) -c $(CPPFLAGS) $< $(INCLUDES) -o $@

clean:
	@printf "$(RED)$(BOLD)cleaning object files...\n"
	@rm -rf obj/

fclean: clean
	@printf "$(RED)$(BOLD)cleaning all...\n"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
