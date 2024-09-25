NAME		= philosophers

CC			= cc

FLAGS		= -Wall -Wextra -Werror

RM			= rm -rf

HEADER		= philosophers.h

# MAIN
DIR_MAIN	= ./
SRC_MAIN	= philosophers.c
PATH_MAIN	= $(addprefix $(DIR_MAIN), $(SRC_MAIN))

# UTILS
SRC_UTILS	= util_1.c
DIR_UTILS	= utils/
PATH_UTILS	= $(addprefix $(DIR_UTILS), $(SRC_UTILS))

# OBJECTS
DIR_OBJ		= objects/
OBJ_MAIN	= $(addprefix $(DIR_OBJ), $(notdir $(SRC_MAIN:.c=.o)))
OBJ_UTILS	= $(addprefix $(DIR_OBJ), $(notdir $(SRC_UTILS:.c=.o)))
SRC_OBJ		= $(OBJ_MAIN) $(OBJ_UTILS)

#Compile MAIN
$(DIR_OBJ)%.o: $(DIR_MAIN)%.c $(HEADER) Makefile
	@mkdir -p $(DIR_OBJ)
	@$(CC) $(FLAGS) -c $< -o $@

#Compile Utils
$(DIR_OBJ)%.o: $(DIR_UTILS)%.c $(HEADER) Makefile
	@mkdir -p $(DIR_OBJ)
	@$(CC) $(FLAGS) -c $< -o $@

#Linking
$(NAME): $(SRC_OBJ)
	@$(CC) $(SRC_OBJ) -o $(NAME)


all: $(NAME)

clean:
	@$(RM) $(DIR_OBJ)/*.o

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
