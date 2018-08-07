
INCFLAGS=-I.. -I../include

CFLAGS=-w -O3 $(INCFLAGS)

PROG=libbattle.a

SRC=battle.c battle_event.c battle_command.c battle_ai.c battle_magic.c pet_skill.c battle_item.c profession_skill.c
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
	$(MV) makefile makefile.bak
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
battle.o: battle.c ../include/version.h ../include/correct_bug.h \
  ../include/version_pk.h /usr/include/string.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/stdlib.h /usr/include/sys/types.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/time.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h ../include/buf.h ../include/common.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h ../include/object.h \
  ../include/char.h ../include/char_base.h ../include/skill.h \
  ../include/util.h /usr/include/sys/time.h ../include/title.h \
  ../include/addressbook.h ../include/net.h /usr/include/netinet/in.h \
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
  ../include/link.h ../include/char_data.h ../include/item.h \
  ../include/battle.h ../include/battle_event.h \
  ../include/battle_command.h ../include/battle_ai.h \
  ../include/configfile.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h /usr/include/strings.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h ../include/encount.h ../include/enemy.h \
  ../include/handletime.h ../include/readmap.h ../include/pet_skill.h \
  ../include/npcutil.h ../include/magic.h ../include/npc_npcenemy.h \
  ../include/log.h ../include/pet_skillinfo.h ../include/anim_tbl.h \
  ../include/battle_magic.h ../include/profession_skill.h
battle_event.o: battle_event.c ../include/version.h \
  ../include/correct_bug.h ../include/version_pk.h /usr/include/string.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/math.h /usr/include/bits/huge_val.h \
  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
  ../include/char.h ../include/common.h /usr/include/stdio.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h \
  ../include/char_base.h ../include/skill.h ../include/util.h \
  /usr/include/sys/time.h /usr/include/time.h /usr/include/bits/time.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/stdlib.h \
  /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h ../include/title.h ../include/addressbook.h \
  ../include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
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
  ../include/link.h ../include/char_data.h ../include/item.h \
  ../include/char_event.h ../include/battle.h ../include/battle_event.h \
  ../include/configfile.h ../include/pet.h ../include/log.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  /usr/include/strings.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h ../include/anim_tbl.h ../include/battle_magic.h \
  ../include/enemy.h ../include/pet_skill.h ../include/item_event.h \
  ../include/profession_skill.h ../include/magic_base.h
battle_command.o: battle_command.c ../include/version.h \
  ../include/correct_bug.h ../include/version_pk.h /usr/include/string.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/stdlib.h /usr/include/sys/types.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/time.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h ../include/char.h ../include/common.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h \
  ../include/char_base.h ../include/skill.h ../include/util.h \
  /usr/include/sys/time.h ../include/title.h ../include/addressbook.h \
  ../include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
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
  ../include/link.h ../include/char_data.h ../include/item.h \
  ../include/battle.h ../include/pet_skill.h ../include/battle_event.h \
  ../include/battle_command.h ../include/lssproto_serv.h \
  ../include/lssproto_util.h /usr/include/strings.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h ../include/magic.h ../include/magic_base.h \
  ../include/handletime.h ../include/profession_skill.h
battle_ai.o: battle_ai.c ../include/version.h ../include/correct_bug.h \
  ../include/version_pk.h /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  ../include/common.h /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h ../include/char.h \
  ../include/char_base.h ../include/skill.h ../include/util.h \
  /usr/include/sys/time.h /usr/include/time.h /usr/include/bits/time.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/stdlib.h \
  /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h ../include/title.h ../include/addressbook.h \
  ../include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
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
  ../include/link.h ../include/char_data.h ../include/item.h \
  ../include/battle.h ../include/npcutil.h ../include/pet_skill.h
battle_magic.o: battle_magic.c ../include/version.h \
  ../include/correct_bug.h ../include/version_pk.h /usr/include/string.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  ../include/char.h ../include/common.h /usr/include/stdio.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h \
  ../include/char_base.h ../include/skill.h ../include/util.h \
  /usr/include/sys/time.h /usr/include/time.h /usr/include/bits/time.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/stdlib.h \
  /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h ../include/title.h ../include/addressbook.h \
  ../include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
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
  ../include/link.h ../include/char_data.h ../include/item.h \
  ../include/battle.h ../include/battle_event.h ../include/magic_base.h \
  ../include/battle_magic.h ../include/item_event.h ../include/anim_tbl.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  /usr/include/strings.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h ../include/profession_skill.h
pet_skill.o: pet_skill.c ../include/version.h ../include/correct_bug.h \
  ../include/version_pk.h /usr/include/stdio.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/string.h ../include/common.h /usr/include/errno.h \
  /usr/include/bits/errno.h /usr/include/linux/errno.h \
  /usr/include/asm/errno.h ../include/buf.h ../include/configfile.h \
  ../include/char_base.h ../include/skill.h ../include/util.h \
  /usr/include/sys/time.h /usr/include/time.h /usr/include/bits/time.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/stdlib.h \
  /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h ../include/title.h ../include/addressbook.h \
  ../include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
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
  ../include/link.h ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/pet_skill.h ../include/battle.h \
  ../include/battle_event.h ../include/battle_magic.h \
  ../include/item_gen.h ../include/char_talk.h
battle_item.o: battle_item.c ../include/version.h \
  ../include/correct_bug.h ../include/version_pk.h /usr/include/string.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  ../include/char.h ../include/common.h /usr/include/stdio.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h \
  ../include/char_base.h ../include/skill.h ../include/util.h \
  /usr/include/sys/time.h /usr/include/time.h /usr/include/bits/time.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/stdlib.h \
  /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h ../include/title.h ../include/addressbook.h \
  ../include/net.h /usr/include/netinet/in.h /usr/include/stdint.h \
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
  ../include/link.h ../include/char_data.h ../include/item.h \
  ../include/battle.h ../include/battle_event.h ../include/battle_item.h \
  ../include/battle_magic.h ../include/item_event.h ../include/log.h \
  ../include/anim_tbl.h ../include/npcutil.h ../include/magic_base.h \
  ../include/lssproto_serv.h ../include/lssproto_util.h \
  /usr/include/strings.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h
profession_skill.o: profession_skill.c ../include/version.h \
  ../include/correct_bug.h ../include/version_pk.h /usr/include/stdio.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/types.h /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h /usr/include/libio.h \
  /usr/include/_G_config.h /usr/include/wchar.h /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/include/string.h /usr/include/time.h /usr/include/bits/time.h \
  ../include/buf.h ../include/common.h /usr/include/errno.h \
  /usr/include/bits/errno.h /usr/include/linux/errno.h \
  /usr/include/asm/errno.h ../include/profession_skill.h \
  ../include/util.h /usr/include/sys/time.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h \
  /usr/include/alloca.h ../include/skill.h ../include/char_base.h \
  ../include/title.h ../include/addressbook.h ../include/net.h \
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
  ../include/link.h ../include/battle.h ../include/magic_base.h \
  ../include/configfile.h ../include/char.h ../include/char_data.h \
  ../include/item.h ../include/lssproto_serv.h ../include/lssproto_util.h \
  /usr/include/strings.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h
