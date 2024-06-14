# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aauthier <aauthier@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/13 18:25:12 by jo-tan            #+#    #+#              #
#    Updated: 2024/06/14 04:36:04 by aauthier         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= minishell

SE_FILES 	= src/exec/exec_builtin.c \
				src/exec/exec_outils.c \
				src/exec/ft_cd.c \
				src/exec/ft_echo.c \
				src/exec/ft_env.c \
				src/exec/ft_exit.c \
				src/exec/ft_export.c \
				src/exec/ft_pwd.c \
				src/exec/ft_unset.c \
				src/exec/main.c \
				src/exec/main_outils.c \
				src/exec/signals.c \
				src/exec/ft_exec.c

SP_FILES	= src/pars/cmd_check_outils.c \
				src/pars/cmd_check.c \
				src/pars/envp_get_list.c \
				src/pars/envp_get_tab.c \
				src/pars/expand_tokenizer.c \
				src/pars/expand.c \
				src/pars/ft_split1.c \
				src/pars/get_files_outils.c \
				src/pars/get_files.c \
				src/pars/group_list.c \
				src/pars/group_outils.c \
				src/pars/outils_envp.c \
				src/pars/outils_expand.c \
				src/pars/outils_free.c \
				src/pars/outils_print.c \
				src/pars/parser_outils.c \
				src/pars/parser.c \
				src/pars/quotes_expand.c \
				src/pars/quotes_ok.c \
				src/pars/quotes_outils.c \
				src/pars/quotes_remove.c \
				src/pars/quotes_spaces.c \
				src/pars/syntax_pb.c \
				src/pars/token_list.c \
				src/pars/outils_libft.c
				
SE_DIR		= src/exec
SP_DIR		= src/pars

LIBFT		= libft/libft.a
LIBFT_DIR	= libft

# Objects
O_DIR		= obj
D_DIR       = $(O_DIR)

O_FILES		= $(patsubst $(SE_DIR)/%.c,$(O_DIR)/%.o,$(SE_FILES)) \
				$(patsubst $(SP_DIR)/%.c,$(O_DIR)/%.o,$(SP_FILES))

D_FILES		= $(patsubst $(SE_DIR)/%.c,$(O_DIR)/%.d,$(SE_FILES)) \
				$(patsubst $(SP_DIR)/%.c,$(O_DIR)/%.d,$(SP_FILES))

# Compilation
CC			= cc
CF          = -Wall -Wextra -Werror -I/opt/homebrew/opt/readline/include
INC         = -I inc/ -I $(LIBFT_DIR)
LDFLAGS     = -L/opt/homebrew/opt/readline/lib -lreadline -L $(LIBFT_DIR) -lft

# Cleaning
RM			= rm -rf

all:		$(LIBFT) $(NAME)

$(NAME):	$(O_FILES)
	@$(CC) -fsanitize=address -g -o $(NAME) $(O_FILES) $(LDFLAGS) -L $(LIBFT_DIR) -lft $(INC)
	@echo "minishell is up to date!"

-include $(D_FILES)

$(O_DIR)/%.o: $(SE_DIR)/%.c
	@echo "Compiling $<"
	@mkdir -p $(O_DIR)
	@$(CC) $(CF) $(INC) -c $< -o $@

$(O_DIR)/%.o: $(SP_DIR)/%.c
	@echo "Compiling $<"
	@mkdir -p $(O_DIR)
	@$(CC) $(CF) $(INC) -c $< -o $@

$(O_DIR):
	@mkdir -p $@

$(LIBFT):
	@make --no-print-directory -C $(LIBFT_DIR)
	@echo "libft is ready."

clean:
	@$(RM) $(O_DIR)
	@make --no-print-directory -C libft fclean
	@echo "Objects and dependend files removed."

fclean:	clean
	@$(RM) $(NAME)
	@make --no-print-directory -C libft fclean
	@echo "Binary files removed."

valgrind: all
		valgrind --suppressions=$(PWD)/ignore_readline --trace-children=yes \
		--leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes \
    	./minishell

re:		fclean all

.PHONY:	all clean fclean re
