NAME		=   ft_traceroute
NAME_BONUS  =   ft_traceroute_bonus
CC			=	gcc
CFLAGS		=	-Wall -Wextra -Werror -D_DEFAULT_SOURCE
INCLUDE		=	-Iinclude

SRC_DIR		=	src
OBJ_DIR		=	obj
INC_DIR		=	include

SRCS		=	$(wildcard ${SRC_DIR}/*.c)
OBJS		=	$(SRCS:${SRC_DIR}/%.c=${OBJ_DIR}/%.o)
LDFLAGS		= -lm

.PHONY		:	all clean fclean re run bonus

all			:	${NAME}

${NAME}		:	${OBJ_DIR} ${OBJS}
				${CC} ${CFLAGS} ${OBJS} -o ${NAME} ${LDFLAGS}

bonus       :   ${NAME_BONUS}

${NAME_BONUS}		:	${OBJ_DIR} ${OBJS}
				${CC} ${CFLAGS} ${OBJS} -o ${NAME_BONUS} ${LDFLAGS}

${OBJ_DIR}	:
				mkdir -p ${OBJ_DIR}

${OBJ_DIR}/%.o	:	${SRC_DIR}/%.c
					${CC} ${CFLAGS} ${INCLUDE} -c $< -o $@

clean		:
				rm -rf ${OBJ_DIR}

fclean		:	clean
				rm -f ${NAME}

re			:	fclean all
