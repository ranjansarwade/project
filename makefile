project : main.c
	cc main.c  -Wall -lncurses -lmenu -o project
clean :
	rm -f *.o project
