#--------------------------------------------------------------------------#
# There is usually no need to change something here unless you want to force
# a specific compiler or specific compile flags.
#--------------------------------------------------------------------------#

CC	    =	g++ -std=c++11
CFLAGS	=	-Wall -DNDEBUG -O3

DEPS	=	../candy-kingdom/build/libcandylib.a
LIBS	=	-L../candy-kingdom/build/ -lcandylib -lz

all: example

clean:
	rm -f example *.o

#--------------------------------------------------------------------------#
# Link
#--------------------------------------------------------------------------#

example: example.o $(DEPS)
	$(CC) -o $@ example.o $(LIBS)

#--------------------------------------------------------------------------#
# Compile
#--------------------------------------------------------------------------#

example.o: example.cpp ipasir.h makefile
	$(CC) $(CFLAGS) -c example.cpp
