export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed


export SHELL=/bin/sh

# USE_MITO 反穴伙民旦伊永玉IO毛银丹方丹卞允月＄1¤7indy匹戊件由奶伙允月午五叄1¤7
# pthread 反骚日卅中及匹｝仇木毛反内仄化pthread 手反内允仇午

INCFLAGS=-Iinclude

CFLAGS=-w -O3 $(INCFLAGS)

PROG=../gmsv

#TOP=../..
TOP=..

PERL=/usr/bin/env perl

SIMPLEEX=$(TOP)/bin/simpleex.perl

#RPC
RPCGEN=~/bin/lsgen.perl
CLIRPCHTML=~/doc/saacproto.html
CLIRPCSRC=saacproto_cli.c saacproto_util.c
CLIRPCINCLUDE=include/saacproto_cli.h include/saacproto_util.h
CLIRPCNOUSE=saacproto_serv.c include/saacproto_serv.h
CLIRPCOPTION=-h include -o . -euc 

SERVRPCHTML=~/doc/lssproto.html
SERVRPCSRC=lssproto_serv.c lssproto_util.c
SERVRPCINCLUDE=include/lssproto_serv.h include/lssproto_util.h
SERVRPCNOUSE=lssproto_cli.c include/lssproto_cli.h
SERVRPCOPTION= -h include -o . -euc -e

RPCSRC=$(CLIRPCSRC) $(SERVRPCSRC)

#PETSKILL
PETSKILLGEN=~/bin/makedefs.perl
PETSKILLTXT=data/petskill.txt
PETSKILLOPTION= -name 8 -value 4 -o

#CHAR_ANIM
ANIMDATAGEN=~/bin/convertStringtographicnumber.perl
ANIMDATATXT=include/anim_tbl.h
ANIMDATAFILE=char/ls2data.h


SUBDIR=char npc map item battle magic
LIBS=char/libchar.a npc/libnpc.a map/libmap.a\
item/libitem.a \
magic/libmagic.a \
battle/libbattle.a \

SRC=main.c init.c configfile.c util.c net.c buf.c msignal.c link.c mclient.c \
callfromac.c callfromcli.c object.c handletime.c function.c log.c npcserver.c \
autil.c npcshandle.c\
$(CLIRPCSRC) $(SERVRPCSRC)

OBJ=$(SRC:.c=.o)


#LDFLAGS=-lm -lpthread
LDFLAGS=-lm


ifeq ($(shell uname),Linux)
	LDFLAGS+=
endif
ifeq ($(shell uname),SunOS)
	LDFLAGS+=-lnsl -lsocket
endif
ifeq ($(shell uname),FreeBSD)
	LDFLAGS+=
endif

all: $(PROG)

$(PROG): genver $(RPCSRC) $(ANIMDATAFILE) $(LIBS) $(OBJ) 
	$(CC) $(CFLAGS) -o $(PROG) $(OBJ) $(LIBS) $(LDFLAGS)

genver:
	$(SHELL) ./genver.sh

depend: $(RPCSRC) $(ANIMDATAFILE)
	@for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir depend;\
	done
	$(MV) makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> makefile 

clean:
	@for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir clean;\
	done
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

distclean:
	@for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir distclean;\
	done
	$(RM) $(CLIRPCSRC)
	$(RM) $(CLIRPCINCLUDE)
	$(RM) $(SERVRPCSRC)
	$(RM) $(SERVRPCINCLUDE)
	$(RM) $(ANIMDATAFILE)
	$(RM) include/*~

	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

	$(MV) Makefile Makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(RM) Makefile.bak

data:	$(RPCSRC) $(ANIMDATAFILE)

$(CLIRPCSRC): #$(RPCGEN) $(CLIRPCHTML) $(CLIRPCGEN)
#	$(PERL) $(RPCGEN)  $(CLIRPCOPTION) $(CLIRPCHTML)
#	$(RM) $(CLIRPCNOUSE)

$(SERVRPCSRC): #$(RPCGEN) $(SERVRPCHTML) $(SERVRPCGEN)
#	$(PERL) $(RPCGEN) $(SERVRPCOPTION) $(SERVRPCHTML)
#	$(RM) $(SERVRPCNOUSE)

$(ANIMDATAFILE): #$(ANIMDATAGEN) $(ANIMDATATXT)
#	$(PERL) $(ANIMDATAGEN) $(ANIMDATATXT) > $(ANIMDATAFILE)

$(LIBS):
	@target=`echo $@ | $(SED) s/\\\\/.\\*//`; \
	$(MAKE) -C $$target;


jpex :
	$(PERL) $(SIMPLEEX) *.c */*.c | nkf -s > jp_literal.sjis



# DO NOT DELETE THIS LINE
