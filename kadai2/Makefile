NAME = bufcache
SRCS = main.c getargs.c buffer.c
OBJS = ${SRCS:.c=.o}
HEADERS = .
RM = rm -f
CC = gcc
CFLAG = -Wall -Wextra -Werror

.c.o:
	${CC} ${CFLAG} -I ${HEADERS} -c $< -o ${<:.c=.o}

all: ${NAME}

$(NAME): $(OBJS)
	${CC} ${CFLAG} -I ${HEADERS} -o ${NAME} ${OBJS}

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all fclean clean re
