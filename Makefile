GCC = gcc
BUILD_DIR = build
GL_LIBS = -lGL -lglut -lGLEW -lGLU
MATH = -lm


all: out1 out2

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

out1: ${BUILD_DIR} 1.c
	${GCC} 1.c Utils.c ${GL_LIBS} ${MATH} -o ${BUILD_DIR}/out1

out2: ${BUILD_DIR} 2.c
	${GCC} 2.c Utils.c ${GL_LIBS} ${MATH} -o ${BUILD_DIR}/out2

clean:
	rm build/*