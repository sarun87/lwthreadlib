#######################################################
###f MAKEFILE FOR mythreadlib and tester code #########
#### Author: Arun Sriraman; Shyam Prasad; Chinmay Sane#
#############################################33########

CC=gcc
LIBCFLAGS=-g -c
CFLAGS=-g
LIBSOURCES=futex.c mythread.c
LIBNAME=libmythread.a
OBJECTS=$(LIBSOURCES:.c=.o)
TEST=myThreadTester.c
INCLUDEPATH=./
LINKFLAG=-L
INCLUDEFLAG=-I

all: compile link test
compile: $(SOURCES)
	@$(CC) $(LIBCFLAGS) $(LIBSOURCES)
link: $(OBJECTS)
	@ar -cvq $(LIBNAME) $(OBJECTS) 
test: $(LIBNAME)
	@$(CC) $(CFLAGS) $(TEST) -lmythread -o runMe $(LINKFLAG)$(INCLUDEPATH) $(INCLUDEFLAG)$(INCLUDEPATH)
clean: 
	@rm -rf *.o
	@rm -rf libmythread.a
	@rm runMe
lib: compile link

