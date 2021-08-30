CFLAGS = -Wall -g
CC     = gcc $(CFLAGS)

all : bl_server bl_client

bl_server : bl_server.o server_funcs.o util.o
	    $(CC) -o bl_server bl_server.o server_funcs.o util.o -lpthread
	    @echo bl_server is ready

bl_client: bl_client.o server_funcs.o simpio.o util.o
	   $(CC) -o bl_client bl_client.o server_funcs.o util.o simpio.o -lpthread
	   @echo bl_client is ready

bl_server.o : bl_server.c blather.h
	      $(CC) -c $<

bl_client.o : bl_client.c blather.h
	      $(CC) -c $<

server_funcs.o : server_funcs.c blather.h
	         $(CC) -c $<

util.o : util.c blather.h
	 $(CC) -c $<

simpio.o : simpio.c blather.h
	   $(CC) -c $<

clean:
	rm -f bl_client bl_server *.o *.fifo


include test_Makefile
