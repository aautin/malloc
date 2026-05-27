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
CFLAGS		:=	-Wall -Wextra -Werror -fPIC -g

LIBFT_PATH	:=	libft
LIBFT_LIB	:=	$(LIBFT_PATH)/libft.a
LIBFT_INC	:=	$(LIBFT_PATH)/inc

INCS        :=	-I$(INC) -I$(LIBFT_INC) -I.
LDFLAGS     :=	-L$(LIBFT_PATH) -lft

TESTS_PATH	:=	tests
TESTS		:=	\
				test0.c \
				test1.c \
				test2.c \
				test3.c \
				test5.c

TESTS_NEED_LIB  :=  \
				main.c \
				multithread_allocations.c \
				test4.c \
				main_ex.c

TESTS_EXEC_PATH	:=	tests_exec
TESTS_EXEC		:=	$(addprefix $(TESTS_EXEC_PATH)/,$(TESTS:.c=.out))

TESTS_LIB_EXEC_PATH	:=	tests_lib_exec
TESTS_LIB_EXEC	:=	$(addprefix $(TESTS_LIB_EXEC_PATH)/,$(TESTS_NEED_LIB:.c=.out))

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
	$(CC) $(INCS) -o $@ $<
$(TESTS_LIB_EXEC_PATH)/%.out: $(TESTS_PATH)/%.c | $(TESTS_LIB_EXEC_PATH)
	$(CC) -L. -lft_malloc $(INCS) -o $@ $<

$(OBJS_PATH):
	mkdir $@
$(TESTS_EXEC_PATH):
	mkdir $@
$(TESTS_LIB_EXEC_PATH):
	mkdir $@

.PHONY: clean fclean re tests clean_tests re_tests

clean:
	rm -r $(OBJS_PATH)

fclean: clean
	rm -f $(NAME) $(SHORTNAME)

re: fclean all

cleanlib:
	make fclean -C $(LIBFT_PATH)

tests: $(TESTS_EXEC) $(TESTS_LIB_EXEC)

clean_tests:
	rm -rf $(TESTS_EXEC_PATH)
	rm -rf $(TESTS_LIB_EXEC_PATH)

re_tests: clean_tests tests