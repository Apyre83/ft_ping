UP = "\033[A"
CLEAR = "\033[K"

MY_SOURCES =	src/ft_ping.c		\
				src/ft_utils.c		\
				src/ft_init.c		\
				src/ft_parsing.c	\
				src/ft_print.c

MY_OBJECTS = $(MY_SOURCES:.c=.o)

INCLUDES = include/

NAME = ft_ping

FLAGS = -g -Wall -Wextra -Werror -I $(INCLUDES)

all: $(NAME)

.c.o:
	@gcc $(FLAGS) -c $< -o $(<:.c=.o)
	@echo $(UP)"\033[0;93mCompiling $(NAME) : $@ \033[0;39m                       "

$(NAME): $(LIB) $(MY_OBJECTS)
	@gcc $(FLAGS) $(MY_OBJECTS) $(LIBFT) -o $(NAME)
	@echo "\033[0;92m$(NAME) compiled\033[0;39m                                "

clean:
	@rm -f $(MY_OBJECTS)
	@echo "\033[0;92m.o files cleaned.\033[0;39m"

fclean: clean
	@rm -f $(NAME) $(NAME_LINUX)
	@echo "\033[0;92m$(NAME) cleaned\033[0;39m"

re: fclean all

.PHONY: all clean fclean re
