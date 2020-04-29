# reference https://mropengate.blogspot.com/2018/01/makefile.html

main: main.o process.o scheduler.o
	gcc -o main main.o process.o scheduler.o

main.o: main.c process.h scheduler.h
	gcc	-c main.c

scheduler.o: scheduler.c process.h scheduler.h
	gcc -c scheduler.c

process.o: process.c process.h
	gcc -c process.c

clean: 
	rm -rf *.o