launcher.exe: launcher.c resources.o
	gcc -mwindows -o launcher.exe -Wall launcher.c resources.o

resources.o: resources.rc
	windres -o resources.o resources.rc

clean:
	rm launcher.exe
	rm resources.o
