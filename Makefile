GCC = gcc
BUILD_DIR = build
GL_LIBS = -lGL -lglut -lGLEW -lGLU
MATH = -lm


all: out1

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

out1: ${BUILD_DIR} 1.c
	${GCC} 1.c ${GL_LIBS} -o ${BUILD_DIR}/out1


clean:
	rm build/*