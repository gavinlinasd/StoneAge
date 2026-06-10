# gmsv - StoneAge game world server
#
# Non-recursive build replacing the 1998-era per-directory makefiles.
# Objects are built next to their sources; the binary lands at ./gmsv.

CC      ?= gcc
PROG    := gmsv

INCFLAGS := -I. -Iinclude
OPTFLAGS := -O2 -g
# This codebase predates strict aliasing and signed-overflow optimization;
# keep the compiler honest about 1990s C semantics.
SAFETY   := -fno-strict-aliasing -fwrapv
WARNINGS := -Wall -Werror=implicit-function-declaration -Werror=implicit-int \
            -Werror=return-type -Werror=pointer-to-int-cast -Werror=int-to-pointer-cast

CFLAGS  ?= -std=gnu17 $(OPTFLAGS) $(SAFETY) $(WARNINGS) $(INCFLAGS)
LDFLAGS ?=
LDLIBS  := -lm

ROOT_SRC := main.c init.c configfile.c util.c net.c buf.c msignal.c link.c \
            mclient.c callfromac.c callfromcli.c object.c handletime.c \
            function.c log.c npcserver.c autil.c npcshandle.c \
            saacproto_cli.c saacproto_util.c lssproto_serv.c lssproto_util.c

# npc sources never compiled by the original build (samples / dead code)
NPC_EXCLUDE := 11npc_action.c npc_fmchallenge.c npc_pkpetshop.c \
               npc_sample.c npc_simpleshop.c

CHAR_SRC   := $(wildcard char/*.c)
NPC_SRC    := $(filter-out $(addprefix npc/,$(NPC_EXCLUDE)),$(wildcard npc/*.c))
MAP_SRC    := $(wildcard map/*.c)
ITEM_SRC   := $(wildcard item/*.c)
BATTLE_SRC := $(wildcard battle/*.c)
MAGIC_SRC  := $(wildcard magic/*.c)

SRC := $(ROOT_SRC) $(CHAR_SRC) $(NPC_SRC) $(MAP_SRC) $(ITEM_SRC) \
       $(BATTLE_SRC) $(MAGIC_SRC)
OBJ := $(SRC:.c=.o)

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

# genver.h embeds include/version.h into a version string shown at startup
genver.h: include/version.h genver.sh
	$(SHELL) ./genver.sh

main.o: genver.h

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

-include $(OBJ:.o=.d)

clean:
	$(RM) $(PROG) $(OBJ) $(OBJ:.o=.d) genver.h

.PHONY: all clean
