#--------------------------------------------------------------------------#
# There is usually no need to change something here unless you want to force
# a specific compiler or specific compile flags.
#--------------------------------------------------------------------------#

CC	    =	g++ -std=c++11
CFLAGS	=	-Wall -DNDEBUG -O3

DEPS	=	../candy-kingdom/build/libcandylib.a
LIBS	=	-L../candy-kingdom/build/ -lcandylib -lz

all: ex3

clean:
	rm -f ex3 *.o

#--------------------------------------------------------------------------#
# Link
#--------------------------------------------------------------------------#

ex3: ex3.o $(DEPS)
	$(CC) -o $@ ex3.o $(LIBS)

#--------------------------------------------------------------------------#
# Compile
#--------------------------------------------------------------------------#

ex3.o: ex3.cpp ipasir.h makefile
	$(CC) $(CFLAGS) -c ex3.cpp
