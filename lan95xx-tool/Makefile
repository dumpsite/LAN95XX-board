ifeq ($(OS),Windows_NT)
	USBFLAGS =
	USBLIBS = -lusb
	EXE_SUFFIX = .exe
	CFLAGS += $(USBFLAGS)
	LDFLAGS+= $(USBLIBS)
else
	USBFLAGS = `libusb-config --cflags`
	USBLIBS = `libusb-config --libs` #-framework CoreFoundation
	EXE_SUFFIX =
	CFLAGS += $(USBFLAGS) -fPIC
	LDFLAGS+= $(USBLIBS)
endif
########################################################################################

#CFLAGS+=-ggdb -g3 -O0 -DDEBUG=3 -DMYDEBUG
CFLAGS+=-ggdb -g3 -O0 -DDEBUG=3 -DMYDEBUG

all: lan95xxtool$(EXE_SUFFIX)
depclean: deepclean
deepclean: clean
	$(RM) *~
clean:
	$(RM) *.o
	$(RM) lan95xxtool$(EXE_SUFFIX)

usbhelper.o: usbhelper.c usbhelper.h Makefile
	$(CC) -c -o usbhelper.o usbhelper.c $(CFLAGS)

lan95xxtool.o: lan95xxtool.c usbhelper.h Makefile
	$(CC) -c -o lan95xxtool.o lan95xxtool.c $(CFLAGS)

lan95xxtool$(EXE_SUFFIX): lan95xxtool.o usbhelper.o usbhelper.h Makefile
	$(CC) -o lan95xxtool$(EXE_SUFFIX) lan95xxtool.o usbhelper.o $(CFLAGS) $(LDFLAGS)
