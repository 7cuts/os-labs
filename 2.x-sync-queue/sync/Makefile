TARGET_1 = queue-example
SRCS_1 = queue.c queue-example.c

TARGET_2 = queue-threads
SRCS_2 = queue.c queue-threads.c

TARGET_SPINLOCK = queue-threads-spinlock
SRCS_SPINLOCK = queue-spinlock.c queue-threads.c


CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET_1} ${TARGET_2} ${TARGET_SPINLOCK}

${TARGET_1}: queue.h ${SRCS_1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_1} ${LIBS} -o ${TARGET_1}

${TARGET_2}: queue.h ${SRCS_2}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_2} ${LIBS} -o ${TARGET_2}

${TARGET_SPINLOCK}: queue.h ${SRCS_SPINLOCK}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_SPINLOCK} ${LIBS} -o ${TARGET_SPINLOCK}

clean:
	${RM} -f *.o ${TARGET_1} ${TARGET_2} ${TARGET_SPINLOCK}
