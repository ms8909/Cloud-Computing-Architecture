CC=gcc -Wall -ansi
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src

que_1:run_que_1
que_2:run_que_2
que_3:run_que_3
que_4:run_que_4



#First Question-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
${OBJ}/q1.o: ${SRC}/q1.c
	${CC} -c -o ${OBJ}/q1.o ${SRC}/q1.c
${BIN}/q1:${OBJ}/q1.o
	${CC} -o ${BIN}/q1 ${OBJ}/q1.o
#Second Question-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

${OBJ}/q2.o:${SRC}/q2.c
	${CC} -c -o ${OBJ}/q2.o ${SRC}/q2.c	
${BIN}/q2:${OBJ}/q2.o
	${CC} -o ${BIN}/q2 ${OBJ}/q2.o
#Third Question-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

${OBJ}/q3.o: ${SRC}/q3.c
	${CC} -c -o ${OBJ}/q3.o ${SRC}/q3.c
${BIN}/q3:${OBJ}/q3.o
	${CC} -o ${BIN}/q3 ${OBJ}/q3.o
#Fourth Question-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

${OBJ}/q4.o: ${SRC}/q4.c
	${CC} -c -o ${OBJ}/q4.o ${SRC}/q4.c
${BIN}/q4:${OBJ}/q4.o
	${CC} -o ${BIN}/q4 ${OBJ}/q4.o


#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
run_que_1:${BIN}/q1 
	${BIN}/q1 6
run_que_2:${BIN}/q2
	${BIN}/q2 6
run_que_3:${BIN}/q3
	${BIN}/q3
run_que_4:${BIN}/q4
	${BIN}/q4

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*
