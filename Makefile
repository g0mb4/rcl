PROJECT = rcl
OUTPUT  = rcli

PREFIX = c:\SysGCC\raspberry\bin\arm-linux-gnueabihf-

SSH_USER = gmb
SSH_PWD  = 123456
SSH_HOST = 192.168.1.21
SCP      = "C:\Program Files (x86)\WinSCP\WinSCP.com"

CC   = $(PREFIX)gcc
CXX  = $(PREFIX)g++
SIZE = $(PREFIX)size

FLEX  = c:\flexbison\bin\win_flex.exe
BISON = c:\flexbison\bin\win_bison.exe

CSRC   = $(wildcard *.c)
COBJ   = $(patsubst %.c, %.o, $(CSRC))
CDEP   = $(patsubst %.c, %.d, $(CSRC))
CFLAGS = -Wall
CLIBS  = -lpthread

all: rcl

rcl: bison flex $(CDEP) $(COBJ)
	$(CC) -o $(OUTPUT) $(COBJ) $(CLIBS)
	$(SIZE) $(OUTPUT)

bison: $(PROJECT).y
	$(BISON) -d $(PROJECT).y
	$(CC) $(CFLAGS) -c $(PROJECT).tab.c

flex: $(PROJECT).l $(PROJECT).tab.h
	$(FLEX) $(PROJECT).l
	$(CC) $(CFLAGS) -c lex.yy.c

$(COBJ): %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $(<:.d=.c)

$(CDEP): %.d: %.c
	$(CC) -MM $(CFLAGS) -o $@ $<

debug: CFLAGS += -g -D _DEBUG -pg
debug: rcl

upload: $(OUTPUT)
	$(SCP) /ini=nul /command "open scp://$(SSH_USER):$(SSH_PWD)@$(SSH_HOST)/ -hostkey=*" "cd /home/$(SSH_USER)" "put $(OUTPUT)" "chmod 744 $(OUTPUT)" "exit"

clean:
	rm -f *.yy.c || true
	rm -f *.tab.c *.tab.h || true
	rm -f *.d *.o || true
	rm -f $(OUTPUT) || true
