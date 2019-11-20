BIN_DIR = bin
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

COMPILER_FLAGS = -O3 -ggdb -Wall -Wextra -pedantic
LINKER_FLAGS =

SOURCE_FILES = $(wildcard ${SRC_DIR}/*.c)
OBJ_FILES = $(patsubst ${SRC_DIR}/%.c,${BUILD_DIR}/%.o,${SOURCE_FILES})

all: ${BIN_DIR}/hasm.exe

run: all
	cmd /c start cmd /c "${BIN_DIR}\hasm.exe ${ARGV} && pause"

debug: all
	gdb ${BIN_DIR}/hasm.exe ${ARGV}

clean:
	del /q /f /s ${BIN_DIR}\*
	del /q /f /s ${BUILD_DIR}\*

${BIN_DIR}/hasm.exe: ${OBJ_FILES}
	gcc ${LINKER_FLAGS} -o $@ $^

${BUILD_DIR}/%.o: ${SRC_DIR}/%.c ${INCLUDE_DIR}/%.h
	gcc ${COMPILER_FLAGS} -I ${INCLUDE_DIR} -c -o $@ $<

${BUILD_DIR}/main.o: ${SRC_DIR}/main.c
	gcc ${COMPILER_FLAGS} -I ${INCLUDE_DIR} -c -o $@ $<

.PHONY: run debug clean
