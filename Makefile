all:	udpservselect01.c udpcli04.c OOXX2.c
	gcc -I./lib -g -O2 -D_REENTRANT -w udpservselect01.c -o udpservselect01 sigchldwaitpid.o ./lib/libunp.a -lpthread
	gcc -I./lib -g -O2 -D_REENTRANT -w udpcli04.c -o udpcli04 dgcliconnect.o  ./lib/libunp.a -lpthread


clean:
		rm t_serv01
