PROJECT=rcl

PREFIX=c:\SysGCC\raspberry\bin\arm-linux-gnueabihf-

SSH_USER=gmb
SSH_PWD=123456
SSH_HOST=192.168.0.21
SCP="C:\Program Files (x86)\WinSCP\WinSCP.com"

CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
SIZE=$(PREFIX)size

FLEX=c:\flexbison\bin\win_flex.exe
BISON=c:\flexbison\bin\win_bison.exe

CFLAGS=-Wall
DEBUGFLAGS=-g -D _DEBUG -pg

bison: $(PROJECT).y
	$(BISON) -d $(PROJECT).y

flex: $(PROJECT).l $(PROJECT).tab.h
	$(FLEX) $(PROJECT).l

cfiles:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(PROJECT).tab.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c lex.yy.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(PROJECT).c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(PROJECT)_env.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $(PROJECT)_fcns.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c str_buf.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c main.c

rcl: bison flex cfiles
	$(CC) -o $(PROJECT) main.o $(PROJECT).tab.o lex.yy.o $(PROJECT).o $(PROJECT)_fcns.o $(PROJECT)_env.o str_buf.o

release: rcl
	$(SIZE) $(PROJECT)

debug: rcl
	$(SIZE) $(PROJECT)
	$(SCP) /ini=nul /command "open scp://$(SSH_USER):$(SSH_PWD)@$(SSH_HOST)/ -hostkey=*" "cd /home/$(SSH_USER)" "put $(PROJECT)" "chmod 744 $(PROJECT)" "exit"

clean:
	rm -f lex.yy.c *.tab.c *.tab.h *.o $(PROJECT)
