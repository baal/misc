CC=gcc
CFLAGS=-mwindows -DWINVER=0x0501 -DUNICODE -D_UNICODE

devnotify.exe: devnotify.c
  ${CC} -o devnotify.exe ${CFLAGS} devnotify.c

clean:
	rm devnotify.exe
