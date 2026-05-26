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

INC     	:=	inc

OBJS_PATH	:=	obj
OBJS		:=	$(addprefix $(OBJS_PATH)/,$(FILES:.c=.o))
DEPS		:=	$(OBJS:.o=.d)

CC			:=	gcc
CFLAGS		:=	-Wall -Wextra -Werror -fPIC

LIBFT_PATH	:=	libft
LIBFT_LIB	:=	$(LIBFT_PATH)/libft.a
LIBFT_INC	:=	$(LIBFT_PATH)/inc

INCS        :=	-I$(INC) -I$(LIBFT_INC)
LDFLAGS     :=	-L$(LIBFT_PATH) -lft

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

$(OBJS_PATH):
	mkdir $@

.PHONY: clean fclean re

clean:
	rm -r $(OBJS_PATH)

fclean: clean
	rm -f $(NAME) $(SHORTNAME)

re: fclean all

cleanlib:
	make fclean -C $(LIBFT_PATH)