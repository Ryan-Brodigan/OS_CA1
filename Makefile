CC = gcc
CFLAGS = -c -Wall
BINARIES = dist
LIBRARIES = lib
OBJ_FILES = ${BINARIES}/main.o\
            ${BINARIES}/simulate.o

link: ${OBJ_FILES}
	${CC} $? -o ${BINARIES}/main

${BINARIES}/main.o: main.c
	${CC} ${CFLAGS} $? -o ${BINARIES}/main.o

${BINARIES}/simulate.o: ${LIBRARIES}/simulate.c
	${CC} ${CFLAGS} $? -o ${BINARIES}/simulate.o

clean:
	rm -rf ./${BINARIES} && mkdir ./${BINARIES}



