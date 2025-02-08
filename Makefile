NAME = ircserv

CCPP = c++

OBJSDIR = obj/

I_DIR = includes/

INCLUDE = -I $(I_DIR)

SRCS = $(shell find src -name *.cpp)

OBJS = $(addprefix $(OBJSDIR), $(SRCS:.cpp=.o))

FLAGS = -Wall -Werror -Wextra -std=c++98

$(NAME): $(OBJS)
	@$(CCPP) $(FLAGS) $(OBJS) -o $(NAME)
	@echo "✅ Compiled"

$(OBJSDIR)%.o: %.cpp
	@mkdir -p $(@D)
	@$(CCPP) $(INCLUDE) $(FLAGS) $< -c -o $@

all: $(NAME)

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)
	@rm -Rf $(OBJSDIR)
	@echo "🧹 Cleaned"

re: fclean all

.PHONY: clean all re fclean
