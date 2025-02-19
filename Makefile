NAME = ircserv
CCPP = c++
FLAGS = -Wall -Werror -Wextra -std=c++98 -g

OBJSDIR = obj/
I_DIR = src/includes/
INCLUDE = -I $(I_DIR)

SRCS = $(shell find src -name "*.cpp" ! -name "ClientServ.cpp")
OBJS = $(addprefix $(OBJSDIR), $(SRCS:.cpp=.o))

CLIENT_SRC   = $(shell find src -name "ClientServ.cpp")
CLIENT_OBJ   = $(addprefix $(OBJSDIR), $(CLIENT_SRC:.cpp=.o))

$(NAME): $(OBJS)
	@$(CCPP) $(FLAGS) $(OBJS) -o $(NAME)
	@echo "✅ Compiled"

$(OBJSDIR)%.o: %.cpp
	@mkdir -p $(@D)
	@$(CCPP) $(INCLUDE) $(FLAGS) $< -c -o $@

client: $(CLIENT_OBJ)
	@$(CCPP) $(FLAGS) $(CLIENT_OBJ) -o "Client"
	@echo "✅ Compiled client"

all: $(NAME)

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)
	@rm -f "Client"
	@rm -Rf $(OBJSDIR)
	@echo "🧹 Cleaned"

re: fclean all

.PHONY: clean all re fclean
