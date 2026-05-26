ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME		=	libft_malloc_$(HOSTTYPE).so
SHORTNAME	=	libft_malloc.so

FILES		:=	\
				debug.c \
				free.c \
				global.c \
				malloc.c \
				realloc.c \
				utils.c

SRCS_PATH	:=	src
SRCS		:=	$(addprefix $(SRCS_PATH)/,$(FILES))

INC     	:=	includes

OBJS_PATH	:=	obj
OBJS		:=	$(addprefix $(OBJS_PATH)/,$(FILES:.c=.o))
DEPS		:=	$(OBJS:.o=.d)

CC			:=	gcc
CFLAGS		:=	-Wall -Wextra -Werror -fPIC

LIBFT_PATH	:=	libft
LIBFT_LIB	:=	$(LIBFT_PATH)/libft.a
LIBFT_INC	:=	$(LIBFT_PATH)/inc

INCS        :=	-I$(INC) -I$(LIBFT_INC) -I.
LDFLAGS     :=	-L$(LIBFT_PATH) -lft

TESTS_PATH	:=	tests
TESTS		:=	\
				main.c \
				test0.c \
				test1.c \
				test2.c \
				test3.c \
				test4.c \
				test5.c \

TESTS_EXEC_PATH	:=	tests_exec
TESTS_EXEC		:=	$(addprefix $(TESTS_EXEC_PATH)/,$(TESTS:.c=.out))

.PHONY: all clean fclean re cleanlib

all: $(NAME)

$(LIBFT_LIB):
	make -C $(LIBFT_PATH)

$(NAME): $(OBJS) $(LIBFT_LIB)
	$(CC) -shared -o $@ $^ $(LDFLAGS)
	rm -f $(SHORTNAME)
	ln -s $@ $(SHORTNAME)

-include $(DEPS)

$(OBJS_PATH)/%.o: $(SRCS_PATH)/%.c | $(OBJS_PATH)
	$(CC) $(CFLAGS) $(INCS) -MMD -MP -c $< -o $@
$(TESTS_EXEC_PATH)/%.out: $(TESTS_PATH)/%.c | $(TESTS_EXEC_PATH)
	$(CC) $(INCS) -L. -lft_malloc -o $@ $<

$(OBJS_PATH):
	mkdir $@
$(TESTS_EXEC_PATH):
	mkdir $@

.PHONY: clean fclean re tests

clean:
	rm -r $(OBJS_PATH)

fclean: clean
	rm -f $(NAME) $(SHORTNAME)

re: fclean all

cleanlib:
	make fclean -C $(LIBFT_PATH)

tests: $(TESTS_EXEC)