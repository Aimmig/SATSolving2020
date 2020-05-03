#--------------------------------------------------------------------------#
# There is usually no need to change something here unless you want to force
# a specific compiler or specific compile flags.
#--------------------------------------------------------------------------#

CC	    =	g++ -std=c++17
CFLAGS	=	-Wall -DNDEBUG -O3

DEPS	=	../../candy-kingdom/build/libcandylib.a
LIBS	=	-L../../candy-kingdom/build/ -lcandylib -lz
NAME    =	ex2

all: $(NAME)

clean:
	rm -f $(NAME) *.o

#--------------------------------------------------------------------------#
# Link
#--------------------------------------------------------------------------#

$(NAME): $(NAME).o $(DEPS)
	$(CC) -o $@ $(NAME).o $(LIBS)

#--------------------------------------------------------------------------#
# Compile
#--------------------------------------------------------------------------#

$(NAME).o: $(NAME).cpp ipasir.h makefile
	$(CC) $(CFLAGS) -c $(NAME).cpp
