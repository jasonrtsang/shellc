all: my_shell

my_shell: myshell.o myjobs.o
	gcc -o my_shell myshell.o myjobs.o

myshell.o: myshell.h myshell.c
	gcc -c myshell.c

myjobs.o: myjobs.h myjobs.c
	gcc -c myjobs.c

clean:
	rm -f my_shell *.o

run: my_shell
	./my_shell