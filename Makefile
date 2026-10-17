OS              := $(shell uname)
NAME            = minishell
CC              = cc
SRCDIR          = src
SRC             = shell/main.c \
                  shell/signals.c \
                  shell/terminal.c \
                  lexer/lexer.c \
                  lexer/tokenizer.c \
                  lexer/operators.c \
                  lexer/quotes.c \
                  lexer/token_utils.c \
                  expansion/expansion.c \
                  expansion/expansion_utils.c \
                  parser/parser.c \
                  parser/node_utils.c \
                  parser/node_free.c \
                  exec/exec.c \
                  exec/cmd_path.c \
                  exec/pipe.c \
                  exec/stack.c \
                  redirection/redirection.c \
                  redirection/dispatch.c \
                  builtins/builtin.c \
                  builtins/dispatch.c \
                  env/env.c \
                  env/env_utils.c \
                  env/env_copy.c \
                  history/history.c \
                  history/history_file.c \
                  error/error.c \
                  error/error_handling.c \
                  error/error_handling_utils.c \
                  common/array_utils.c
OBJDIR          = obj
OBJS            = $(SRC:%.c=$(OBJDIR)/%.o)
HDRS            = $(shell find $(SRCDIR) -name '*.h')
LIBFTPATH       = libft
LIBFT           = $(LIBFTPATH)/libft.a
CFLAGS          = -g -Wall -Werror -Wextra
INCLUDES        = -I$(SRCDIR) -I$(LIBFTPATH)

# Conditional Readline paths for macOS
ifeq ($(OS),Darwin)
    READLINE_PATH = $(shell brew --prefix readline)
    INC_DIR       = -I$(READLINE_PATH)/include
    LIB_DIR       = -L$(READLINE_PATH)/lib
else
    INC_DIR       =
    LIB_DIR       =
endif

# Generic rule for .o files
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HDRS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) $(INC_DIR) -c $< -o $@

all: $(NAME)

$(LIBFT):
	@make -C $(LIBFTPATH) all

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(OBJS) $(LIBFT) -o $(NAME) $(LIB_DIR) -lreadline

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)
	rm -rf $(NAME).dSYM
	rm -rf $(LIBFTPATH)/*.o
	rm -rf $(LIBFTPATH)/*.a

re: fclean all

run: all
	./$(NAME)

