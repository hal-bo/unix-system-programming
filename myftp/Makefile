# 61911650 高野遥斗

TARGET1 = myftpd
SRCS1 = myftp_server.c myftp_share.c myftp_server_util.c
OBJS1 = ${SRCS1:.c=.o}

TARGET2 = myftpc
SRCS2 = myftp_client.c myftp_share.c myftp_client_util.c string_utils.c
OBJS2 = ${SRCS2:.c=.o}

HEADERS = .
RM = rm -f
CC = gcc
CFLAG = #-Wall -Wextra -Werror

all: ${TARGET1} ${TARGET2}

${TARGET1}: ${OBJS1}
	${CC} ${CFLAG} -I ${HEADERS} -o ${TARGET1} ${OBJS1} 
${TARGET2}: ${OBJS2}
	${CC} ${CFLAG} -I ${HEADERS} -o ${TARGET2} ${OBJS2}

.c.o:
	${CC} ${CFLAG} -I ${HEADERS} -c $< -o ${<:.c=.o}

clean:
	${RM} ${OBJS1} ${OBJS2}

clean_target:
	${RM} ${TARGET1} ${TARGET2}

fclean: clean
	${RM} ${TARGET1} ${OBJS1} ${TARGET2} ${OBJS2}

re: fclean all

.PHONY: all fclean clean re
