INCFLAGS=-I.. -I../include


CFLAGS=-w -O3 $(INCFLAGS)

PROG=libmap.a

SRC=readmap.c map_deal.c map_util.c map_warppoint.c
OBJ=$(SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
CC=gcc
RM=rm -f
AR=ar cr
MV=mv
RANLIB=ranlib
SED=sed
SHELL=/bin/sh
endif

all: $(PROG)

$(PROG): $(OBJ)
	$(RM) $(PROG)
	$(AR) $(PROG) $(OBJ)
	$(RANLIB) $(PROG)

depend:
	$(MV) Makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> makefile 

clean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

distclean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~
	$(MV) makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(RM) makefile.bak

# DO NOT DELETE THIS LINE
readmap.o: readmap.c /usr/include/stdio.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/string.h /usr/include/dirent.h \
 /usr/include/bits/dirent.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/errno.h /usr/include/bits/errno.h \
 /usr/include/linux/errno.h /usr/include/asm/errno.h \
 /usr/include/unistd.h /usr/include/bits/posix_opt.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 /usr/include/netinet/in.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix2_lim.h \
 /usr/include/stdint.h /usr/include/bits/wordsize.h \
 /usr/include/bits/socket.h /usr/include/bits/sockaddr.h \
 /usr/include/asm/socket.h /usr/include/asm/sockios.h \
 /usr/include/bits/in.h /usr/include/bits/byteswap.h \
 /usr/include/sys/stat.h /usr/include/bits/stat.h ../include/common.h \
 ../include/object.h ../include/readmap.h ../include/util.h \
 /usr/include/sys/time.h /usr/include/bits/time.h ../include/buf.h \
 ../include/char.h ../include/char_base.h ../include/skill.h \
 ../include/title.h ../include/addressbook.h ../include/net.h \
 /usr/include/signal.h /usr/include/bits/signum.h \
 /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
 /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
 /usr/include/bits/sigstack.h ../include/link.h ../include/char_data.h \
 ../include/item.h ../include/lssproto_serv.h \
 ../include/lssproto_util.h /usr/include/strings.h \
 ../include/configfile.h
map_deal.o: map_deal.c ../include/common.h /usr/include/stdio.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 ../include/readmap.h ../include/util.h /usr/include/sys/time.h \
 /usr/include/time.h /usr/include/bits/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/stdlib.h \
 /usr/include/sys/types.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../include/char.h ../include/char_base.h \
 ../include/skill.h ../include/title.h ../include/addressbook.h \
 ../include/net.h /usr/include/netinet/in.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/stdint.h \
 /usr/include/bits/wordsize.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/signal.h \
 /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
 /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
 /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
 ../include/link.h ../include/char_data.h ../include/item.h \
 ../include/map_util.h ../include/lssproto_serv.h \
 ../include/lssproto_util.h /usr/include/string.h \
 /usr/include/strings.h /usr/include/unistd.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
 /usr/include/getopt.h ../include/anim_tbl.h
map_util.o: map_util.c ../include/common.h /usr/include/stdio.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 ../include/char.h ../include/char_base.h ../include/skill.h \
 ../include/util.h /usr/include/sys/time.h /usr/include/time.h \
 /usr/include/bits/time.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../include/title.h ../include/addressbook.h \
 ../include/net.h /usr/include/netinet/in.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/stdint.h \
 /usr/include/bits/wordsize.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/signal.h \
 /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
 /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
 /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
 ../include/link.h ../include/char_data.h ../include/item.h \
 ../include/readmap.h
map_warppoint.o: map_warppoint.c ../include/common.h /usr/include/stdio.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 ../include/char.h ../include/char_base.h ../include/skill.h \
 ../include/util.h /usr/include/sys/time.h /usr/include/time.h \
 /usr/include/bits/time.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../include/title.h ../include/addressbook.h \
 ../include/net.h /usr/include/netinet/in.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/stdint.h \
 /usr/include/bits/wordsize.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/signal.h \
 /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
 /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
 /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
 ../include/link.h ../include/char_data.h ../include/item.h \
 ../include/readmap.h ../include/map_warppoint.h
