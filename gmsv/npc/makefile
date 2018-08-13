INCFLAGS=-I. -I../include


CFLAGS=-w -O3 $(INCFLAGS)

PROG=libnpc.a

SRC=readnpc.c npcgen.c npctemplate.c npccreate.c npcutil.c \
npc_warp.c npc_townpeople.c \
npc_msg.c npc_healer.c npc_oldman.c \
npc_storyteller.c npc_dengon.c npc_doorman.c \
npc_npcenemy.c npc_itemchange.c \
npc_action.c npc_windowman.c npc_savepoint.c \
npc_windowhealer.c npc_itemshop.c npc_sysinfo.c \
npc_duelranking.c npc_petskillshop.c npc_petshop.c \
npc_signboard.c npc_warpman.c npc_exchangeman.c \
npc_timeman.c npc_bodylan.c npc_mic.c \
npc_luckyman.c npc_bus.c npc_charm.c npc_quiz.c \
npc_poolitemshop.c npc_checkman.c npc_janken.c \
npc_transmigration.c npc_makepair.c \
npc_familyman.c npc_fmdengon.c npc_fmwarpman.c \
npc_fmhealer.c npc_bankman.c npc_fmpkman.c\
npc_airplane.c npc_scheduleman.c npc_fmpkcallman.c\
npc_manorsman.c npc_riderman.c npc_fmletter.c npc_petmaker.c \
npc_gamblemaster.c npc_gambleroulette.c npc_petfusion.c\
npc_stoneserviceman.c npc_newnpcman.c npc_gamblebank.c \
npc_petracemaster.c npc_petracepet.c npc_bigsmallmaster.c \
npc_bigsmallpet.c npc_auctioneer.c npc_freepetskillshop.c \
npc_eventaction.c \
npc_mtradenpcman.c npc_blackmarket.c npc_transerman.c npc_pauctionman.c npc_alldoman.c\
npc_sellsthman.c \
npc_welfare.c npc_verywelfare.c npc_welfare2.c \
npc_raceman.c 

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
