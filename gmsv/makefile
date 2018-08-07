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

PROG=gmsv

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
main.o: main.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/time.h /usr/include/bits/time.h /usr/include/string.h \
  /usr/include/signal.h /usr/include/bits/sigset.h \
  /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
  /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
  /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/bits/sigthread.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h include/init.h include/common.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/net.h \
  /usr/include/sys/time.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/netinet/in.h \
  /usr/include/stdint.h /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/sys/types.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h include/link.h \
  include/handletime.h include/char.h include/char_base.h include/skill.h \
  include/util.h /usr/include/stdlib.h /usr/include/alloca.h \
  include/title.h include/addressbook.h include/char_data.h \
  include/item.h include/map_warppoint.h include/npcgen.h include/log.h \
  include/configfile.h include/battle.h include/petmail.h include/autil.h \
  include/family.h include/defend.h include/trade.h include/chatroom.h \
  genver.h
init.o: init.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/unistd.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/typesizes.h /usr/include/bits/confname.h \
  /usr/include/getopt.h /usr/include/stdio.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/strings.h include/configfile.h include/common.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/util.h \
  /usr/include/sys/time.h /usr/include/time.h /usr/include/bits/time.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/stdlib.h \
  /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h include/net.h /usr/include/netinet/in.h \
  /usr/include/stdint.h /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h \
  /usr/include/signal.h /usr/include/bits/signum.h \
  /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
  /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h \
  include/link.h include/msignal.h include/buf.h include/object.h \
  include/char.h include/char_base.h include/skill.h include/title.h \
  include/addressbook.h include/char_data.h include/item.h \
  include/readmap.h include/function.h include/saacproto_cli.h \
  include/saacproto_util.h /usr/include/string.h include/lssproto_serv.h \
  include/lssproto_util.h include/readnpc.h include/log.h \
  include/handletime.h include/encount.h include/enemy.h include/battle.h \
  include/magic_base.h include/pet_skill.h include/item_gen.h \
  include/petmail.h include/npc_quiz.h include/profession_skill.h \
  include/init.h
configfile.o: configfile.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/string.h /usr/include/strings.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h include/util.h /usr/include/sys/time.h \
  /usr/include/time.h /usr/include/bits/time.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h include/common.h /usr/include/errno.h \
  /usr/include/bits/errno.h /usr/include/linux/errno.h \
  /usr/include/asm/errno.h include/lssproto_util.h include/configfile.h \
  include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h \
  /usr/include/signal.h /usr/include/bits/signum.h \
  /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
  /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h \
  include/link.h include/npcutil.h include/char.h include/char_base.h \
  include/skill.h include/title.h include/addressbook.h \
  include/char_data.h include/item.h include/lssproto_serv.h \
  include/npc_raceman.h
util.o: util.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/sys/types.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/typesizes.h /usr/include/time.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/sys/socket.h /usr/include/sys/uio.h \
  /usr/include/bits/uio.h /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/sys/time.h /usr/include/sys/stat.h \
  /usr/include/bits/stat.h /usr/include/netinet/in.h \
  /usr/include/stdint.h /usr/include/bits/wchar.h /usr/include/bits/in.h \
  /usr/include/bits/byteswap.h /usr/include/arpa/inet.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/string.h /usr/include/stdlib.h /usr/include/alloca.h \
  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
  /usr/include/bits/confname.h /usr/include/getopt.h \
  /usr/include/dirent.h /usr/include/bits/dirent.h /usr/include/netdb.h \
  /usr/include/rpc/netdb.h /usr/include/bits/netdb.h /usr/include/errno.h \
  /usr/include/bits/errno.h /usr/include/linux/errno.h \
  /usr/include/asm/errno.h /usr/include/ctype.h include/common.h \
  include/util.h include/configfile.h
net.o: net.c include/version.h include/correct_bug.h include/version_pk.h \
  /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h \
  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
  /usr/include/bits/confname.h /usr/include/getopt.h /usr/include/netdb.h \
  /usr/include/netinet/in.h /usr/include/stdint.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h \
  /usr/include/rpc/netdb.h /usr/include/bits/netdb.h \
  /usr/include/sys/socket.h /usr/include/sys/uio.h \
  /usr/include/bits/uio.h /usr/include/sys/time.h \
  /usr/include/arpa/inet.h /usr/include/netinet/tcp.h \
  /usr/include/sys/timeb.h include/net.h include/common.h \
  /usr/include/signal.h /usr/include/bits/signum.h \
  /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
  /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h \
  include/link.h include/buf.h include/msignal.h include/configfile.h \
  include/util.h include/saacproto_cli.h include/saacproto_util.h \
  /usr/include/strings.h include/lssproto_serv.h include/lssproto_util.h \
  include/char.h include/char_base.h include/skill.h include/title.h \
  include/addressbook.h include/char_data.h include/item.h \
  include/handletime.h include/log.h include/object.h \
  include/item_event.h include/enemy.h include/battle.h \
  include/pet_event.h include/char_talk.h include/petmail.h \
  include/chatmagic.h
buf.o: buf.c include/version.h include/correct_bug.h include/version_pk.h \
  /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  include/buf.h include/common.h /usr/include/errno.h \
  /usr/include/bits/errno.h /usr/include/linux/errno.h \
  /usr/include/asm/errno.h include/handletime.h /usr/include/sys/time.h
msignal.o: msignal.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/signal.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/include/bits/sigset.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/typesizes.h /usr/include/bits/signum.h \
  /usr/include/time.h /usr/include/bits/siginfo.h \
  /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
  /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/bits/sigthread.h /usr/include/errno.h \
  /usr/include/bits/errno.h /usr/include/linux/errno.h \
  /usr/include/asm/errno.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h /usr/include/stdio.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  include/buf.h include/common.h include/net.h /usr/include/sys/time.h \
  /usr/include/bits/time.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/netinet/in.h \
  /usr/include/stdint.h /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/sys/types.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h include/link.h \
  include/char.h include/char_base.h include/skill.h include/util.h \
  /usr/include/stdlib.h /usr/include/alloca.h include/title.h \
  include/addressbook.h include/char_data.h include/item.h \
  include/object.h include/configfile.h include/lssproto_serv.h \
  include/lssproto_util.h /usr/include/string.h /usr/include/strings.h \
  include/saacproto_cli.h include/saacproto_util.h include/log.h \
  include/petmail.h
link.o: link.c /usr/include/stdlib.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h include/version.h \
  include/correct_bug.h include/version_pk.h include/link.h \
  include/common.h /usr/include/stdio.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/buf.h
mclient.o: mclient.c /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/string.h /usr/include/strings.h /usr/include/sys/types.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/sys/socket.h \
  /usr/include/sys/uio.h /usr/include/bits/uio.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/sys/time.h /usr/include/sys/stat.h \
  /usr/include/bits/stat.h /usr/include/netinet/in.h \
  /usr/include/stdint.h /usr/include/bits/in.h \
  /usr/include/bits/byteswap.h /usr/include/arpa/inet.h \
  /usr/include/stdlib.h /usr/include/alloca.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
  /usr/include/netdb.h /usr/include/rpc/netdb.h /usr/include/bits/netdb.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/autil.h \
  include/version.h include/correct_bug.h include/version_pk.h \
  include/common.h include/util.h include/net.h /usr/include/signal.h \
  /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
  /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
  /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
  /usr/include/bits/sigthread.h include/link.h include/char.h \
  include/char_base.h include/skill.h include/title.h \
  include/addressbook.h include/char_data.h include/item.h \
  include/mclient.h include/log.h include/handletime.h \
  include/chatmagic.h
callfromac.o: callfromac.c /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  include/version.h include/correct_bug.h include/version_pk.h \
  include/common.h /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h \
  include/saacproto_cli.h include/saacproto_util.h /usr/include/stdlib.h \
  /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h /usr/include/string.h /usr/include/strings.h \
  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
  /usr/include/bits/confname.h /usr/include/getopt.h \
  include/lssproto_serv.h include/lssproto_util.h include/net.h \
  /usr/include/sys/time.h /usr/include/netinet/in.h /usr/include/stdint.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h \
  /usr/include/signal.h /usr/include/bits/signum.h \
  /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
  /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h \
  include/link.h include/char.h include/char_base.h include/skill.h \
  include/util.h include/title.h include/addressbook.h \
  include/char_data.h include/item.h include/object.h include/readmap.h \
  include/msignal.h include/map_deal.h include/npc_duelranking.h \
  include/npc_warpman.h include/handletime.h include/npc_scheduleman.h \
  include/configfile.h include/chatroom.h include/npc_eventaction.h \
  include/family.h include/defend.h include/log.h include/buf.h \
  include/npc_alldoman.h include/npc_raceman.h
callfromcli.o: callfromcli.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/time.h /usr/include/bits/time.h /usr/include/stdlib.h \
  /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h include/common.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/util.h \
  /usr/include/sys/time.h include/lssproto_serv.h include/lssproto_util.h \
  /usr/include/string.h /usr/include/strings.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h include/saacproto_cli.h include/saacproto_util.h \
  include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h \
  /usr/include/signal.h /usr/include/bits/signum.h \
  /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
  /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h \
  include/link.h include/char.h include/char_base.h include/skill.h \
  include/title.h include/addressbook.h include/char_data.h \
  include/item.h include/object.h include/readmap.h include/handletime.h \
  include/configfile.h include/event.h include/pet.h include/battle.h \
  include/battle_command.h include/magic.h include/petmail.h \
  include/item_gen.h include/pet_skill.h include/log.h include/map_deal.h \
  include/trade.h include/family.h include/item_event.h \
  include/profession_skill.h include/chatroom.h
object.o: object.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/sys/stat.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/typesizes.h /usr/include/time.h \
  /usr/include/bits/stat.h /usr/include/stdio.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/string.h /usr/include/stdlib.h /usr/include/sys/types.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/dirent.h \
  /usr/include/bits/dirent.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  include/common.h /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/object.h \
  include/char_base.h include/skill.h include/util.h \
  /usr/include/sys/time.h include/title.h include/addressbook.h \
  include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix2_lim.h \
  /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
  /usr/include/asm/sockios.h /usr/include/bits/in.h \
  /usr/include/bits/byteswap.h /usr/include/signal.h \
  /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
  /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
  /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
  /usr/include/bits/sigthread.h include/link.h include/char.h \
  include/char_data.h include/item.h include/buf.h include/readmap.h \
  include/pet.h
handletime.o: handletime.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/time.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/time.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/sys/time.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h include/common.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h \
  include/handletime.h
function.o: function.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/string.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  include/common.h /usr/include/stdio.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
  /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/util.h \
  /usr/include/sys/time.h /usr/include/time.h /usr/include/bits/time.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/stdlib.h \
  /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h include/char_event.h include/char_base.h \
  include/skill.h include/title.h include/addressbook.h include/net.h \
  /usr/include/netinet/in.h /usr/include/stdint.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h \
  /usr/include/signal.h /usr/include/bits/signum.h \
  /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
  /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h \
  include/link.h include/char.h include/char_data.h include/item.h \
  include/item_event.h include/magic.h include/pet_event.h \
  include/npc_townpeople.h include/npc_Dengon.h include/npc_door.h \
  include/npc_healer.h include/npc_oldman.h include/npc_warp.h \
  include/npc_storyteller.h include/npc_msg.h include/npc_npcenemy.h \
  include/npc_action.h include/npc_windowman.h include/npc_savepoint.h \
  include/npc_windowhealer.h include/npc_itemshop.h include/npc_sysinfo.h \
  include/npc_duelranking.h include/npc_petskillshop.h \
  include/npc_petshop.h include/npc_signboard.h include/npc_warpman.h \
  include/npc_exchangeman.h include/petmail.h include/npc_timeman.h \
  include/npc_bodylan.h include/npc_mic.h include/npc_luckyman.h \
  include/npc_bus.h include/npc_charm.h include/npc_poolitemshop.h \
  include/npc_quiz.h include/npc_checkman.h include/npc_janken.h \
  include/npc_transmigration.h include/battle_event.h include/enemy.h \
  include/npc_familyman.h include/npc_bankman.h include/npc_fmdengon.h \
  include/npc_fmhealer.h include/npc_petmaker.h include/npc_fmwarpman.h \
  include/npc_fmpkman.h include/npc_fmpkcallman.h include/npc_airplane.h \
  include/npc_scheduleman.h include/npc_manorsman.h \
  include/npc_riderman.h include/npc_fmletter.h \
  include/npc_stoneserviceman.h include/npc_gamblebank.h \
  include/npc_newnpcman.h include/npc_gambleroulette.h \
  include/npc_gamblemaster.h include/npc_transerman.h \
  include/npc_freepetskillshop.h include/npc_petracemaster.h \
  include/npc_petracepet.h include/npc_itemchange.h \
  include/npc_makepair.h include/npc_petfusion.h include/npc_alldoman.h \
  include/npc_welfare.h include/npc_raceman.h
log.o: log.c include/version.h include/correct_bug.h include/version_pk.h \
  /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/string.h /usr/include/time.h /usr/include/bits/time.h \
  include/common.h /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/util.h \
  /usr/include/sys/time.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h include/log.h include/handletime.h include/net.h \
  /usr/include/netinet/in.h /usr/include/stdint.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h \
  /usr/include/signal.h /usr/include/bits/signum.h \
  /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
  /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h \
  include/link.h include/char_base.h include/skill.h include/title.h \
  include/addressbook.h
npcserver.o: npcserver.c /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/strings.h /usr/include/sys/types.h /usr/include/time.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/sys/socket.h \
  /usr/include/sys/uio.h /usr/include/bits/uio.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/sys/time.h /usr/include/sys/stat.h \
  /usr/include/bits/stat.h /usr/include/netinet/in.h \
  /usr/include/stdint.h /usr/include/bits/in.h \
  /usr/include/bits/byteswap.h /usr/include/arpa/inet.h \
  /usr/include/stdlib.h /usr/include/alloca.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
  /usr/include/netdb.h /usr/include/rpc/netdb.h /usr/include/bits/netdb.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/autil.h \
  include/version.h include/correct_bug.h include/version_pk.h \
  include/common.h include/util.h include/net.h /usr/include/signal.h \
  /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
  /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
  /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
  /usr/include/bits/sigthread.h include/link.h include/char.h \
  include/char_base.h include/skill.h include/title.h \
  include/addressbook.h include/char_data.h include/item.h include/log.h \
  include/handletime.h include/npcserver.h include/lssproto_serv.h \
  include/lssproto_util.h /usr/include/string.h
autil.o: autil.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h include/autil.h \
  include/common.h /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/char.h \
  include/char_base.h include/skill.h include/util.h \
  /usr/include/sys/time.h include/title.h include/addressbook.h \
  include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h \
  /usr/include/signal.h /usr/include/bits/signum.h \
  /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
  /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h \
  include/link.h include/char_data.h include/item.h \
  include/lssproto_util.h /usr/include/string.h /usr/include/strings.h \
  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
  /usr/include/bits/confname.h /usr/include/getopt.h
npcshandle.o: npcshandle.c /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/strings.h /usr/include/sys/types.h /usr/include/time.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/sys/socket.h \
  /usr/include/sys/uio.h /usr/include/bits/uio.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/sys/time.h /usr/include/sys/stat.h \
  /usr/include/bits/stat.h /usr/include/netinet/in.h \
  /usr/include/stdint.h /usr/include/bits/in.h \
  /usr/include/bits/byteswap.h /usr/include/arpa/inet.h \
  /usr/include/stdlib.h /usr/include/alloca.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
  /usr/include/netdb.h /usr/include/rpc/netdb.h /usr/include/bits/netdb.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/autil.h \
  include/version.h include/correct_bug.h include/version_pk.h \
  include/common.h include/util.h include/net.h /usr/include/signal.h \
  /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
  /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
  /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
  /usr/include/bits/sigthread.h include/link.h include/object.h \
  include/char.h include/char_base.h include/skill.h include/title.h \
  include/addressbook.h include/char_data.h include/item.h include/log.h \
  include/handletime.h include/npcserver.h include/lssproto_serv.h \
  include/lssproto_util.h /usr/include/string.h include/npcshandle.h \
  include/npcutil.h include/npc_eventaction.h
saacproto_cli.o: saacproto_cli.c include/version.h include/correct_bug.h \
  include/version_pk.h include/saacproto_cli.h include/saacproto_util.h \
  /usr/include/stdlib.h /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/strings.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h /usr/include/stdio.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  include/common.h /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h include/net.h \
  /usr/include/sys/time.h /usr/include/netinet/in.h /usr/include/stdint.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h \
  /usr/include/signal.h /usr/include/bits/signum.h \
  /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
  /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h \
  include/link.h include/chatmagic.h
saacproto_util.o: saacproto_util.c include/version.h \
  include/correct_bug.h include/version_pk.h /usr/include/stdio.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
  /usr/include/bits/confname.h /usr/include/getopt.h \
  /usr/include/strings.h include/saacproto_util.h
lssproto_serv.o: lssproto_serv.c include/version.h include/correct_bug.h \
  include/version_pk.h include/autil.h include/common.h \
  /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h \
  include/lssproto_serv.h include/lssproto_util.h /usr/include/stdlib.h \
  /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h /usr/include/string.h /usr/include/strings.h \
  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
  /usr/include/bits/confname.h /usr/include/getopt.h include/log.h \
  include/char.h include/char_base.h include/skill.h include/util.h \
  /usr/include/sys/time.h include/title.h include/addressbook.h \
  include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
  /usr/include/bits/socket.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h /usr/include/asm/sockios.h \
  /usr/include/bits/in.h /usr/include/bits/byteswap.h \
  /usr/include/signal.h /usr/include/bits/signum.h \
  /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
  /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h \
  include/link.h include/char_data.h include/item.h
lssproto_util.o: lssproto_util.c include/version.h include/correct_bug.h \
  include/version_pk.h /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
  /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
  /usr/include/bits/confname.h /usr/include/getopt.h \
  /usr/include/strings.h include/lssproto_util.h include/common.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h
