CFLAGS = -Wall -g
CC     = gcc $(CFLAGS)

commando : cmd.o commando.o cmdcol.o util.o 
	$(CC) -o commando cmd.o commando.o cmdcol.o util.o
	@echo commando is ready

commando.o : commando.c commando.h 
	$(CC) -c commando.c 

cmd.o : cmd.c commando.h 
	$(CC) -c $<

cmdcol.o : cmdcol.c commando.h
	$(CC) -c $<

util.o : util.c commando.h
	$(CC) -c $<
clean:
	rm -f commando *.o

include test_Makefile
