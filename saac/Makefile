# saac - StoneAge account/auth server (MySQL/MariaDB backed)
# Also builds the acwk housekeeping worker in wk/.

CC      ?= gcc
PROG    := saac
WKPROG  := wk/acwk

MYSQL_CFLAGS := $(shell mariadb_config --cflags 2>/dev/null || mysql_config --cflags 2>/dev/null)
MYSQL_LIBS   := $(shell mariadb_config --libs 2>/dev/null || mysql_config --libs 2>/dev/null)

INCFLAGS := -I.
OPTFLAGS := -O2 -g
SAFETY   := -fno-strict-aliasing -fwrapv
WARNINGS := -Wall -Werror=implicit-function-declaration -Werror=implicit-int \
            -Werror=return-type -Werror=pointer-to-int-cast -Werror=int-to-pointer-cast

CFLAGS  ?= -std=gnu17 $(OPTFLAGS) $(SAFETY) $(WARNINGS) $(INCFLAGS) $(MYSQL_CFLAGS)
LDFLAGS ?=
LDLIBS  := $(MYSQL_LIBS) -lz

SRC := main.c recv.c util.c char.c db.c mail.c acfamily.c defend.c lock.c \
       tcpip_util.c chatroom.c auction.c sasql.c deathcontend.c \
       saacproto_serv.c saacproto_util.c saacproto_work.c \
       saacproto_lserver.c saacproto_oac.c
OBJ := $(SRC:.c=.o)

WK_SRC := wk/wk.c wk/error.c wk/TCPIP.c wk/tools.c
WK_OBJ := $(WK_SRC:.c=.o)

all: $(PROG) $(WKPROG)

$(PROG): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

$(WKPROG): $(WK_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(WK_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

-include $(OBJ:.o=.d) $(WK_OBJ:.o=.d)

clean:
	$(RM) $(PROG) $(OBJ) $(WKPROG) $(WK_OBJ) $(OBJ:.o=.d) $(WK_OBJ:.o=.d)

.PHONY: all clean
