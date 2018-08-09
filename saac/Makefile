#
# Edit by ringo 19980606
# 19990723
#
#
#

CC = gcc

#加入mysql资料库功能
CFLAGS=-w -O3 $(INCFLAGS)
MYSQL=-L /usr/lib/mysql/ -lmysqlclient -lz 

TOP=../..
PROG = saac
SHELL = /bin/sh
PERL=/usr/bin/perl

RPCGEN=${TOP}/bin/lsgen.perl

RPCHTML = ${TOP}/doc/saacproto.html
#RPCSRC = saacproto_serv.c saacproto_util.c saacproto_work.c saacproto_sup.c saacproto_lserver.c saacproto_oac.c deathcontend.c
RPCSRC = saacproto_serv.c saacproto_util.c saacproto_work.c saacproto_lserver.c saacproto_oac.c deathcontend.c
RPCINCLUDE = saacproto_serv.h saacproto_util.h saacproto_work.h saacproto_lserver.h saacproto_oac.h deathcontend.h
RPCNOUSE = saacproto_cli.c saacproto_cli.h

#加入mysql资料库功能
SRC = main.c recv.c util.c char.c db.c mail.c acfamily.c defend.c lock.c tcpip_util.c chatroom.c auction.c sasql.c $(RPCSRC)
#SRC = main.c recv.c util.c char.c db.c mail.c acfamily.c defend.c lock.c tcpip_util.c chatroom.c auction.c $(RPCSRC)

OBJ=$(SRC:.c=.o)

WKDIR=wk
WK=acwk




all : ${PROG}# ${WK}


${PROG} : $(RPCSRC) ${OBJ} 
	$(CC) $ -o $(PROG) $(OBJ) $(CFLAGS) $(MYSQL)

#${WK} :
#	make -C ${WKDIR} all

depend: $(RPCSRC) $(ANIMDATAFILE)
	@for dir in $(SUBDIR) ; do \
		$(MAKE) -C $$dir depend;\
	done
	$(MV) Makefile Makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> Makefile 

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


rpc : ${RPCHTML} ${RPCGEN}
	$(PERL) ${RPCGEN} ${RPCHTML} -h . -o .
	rm -f ${RPCNOUSE}

# DO NOT DELETE THIS LINE
