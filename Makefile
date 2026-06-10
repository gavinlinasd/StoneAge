# StoneAge server suite: gmsv (game server) + saac (account server)

SUBDIRS := gmsv saac

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	$(MAKE) -C gmsv clean
	$(MAKE) -C saac clean

.PHONY: all clean $(SUBDIRS)
