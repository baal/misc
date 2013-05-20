devdump.exe: devdump.c
	cl /D WIN32 /D NDEBUG /D _CONSOLE /D UNICODE /D _UNICODE devdump.c

clean:
	del devdump.obj
	del devdump.exe
