#CONTIKI_PROJECT = research_protocol
#all: $(CONTIKI_PROJECT)

#CFLAGS += -Iinc

#PROJECTDIRS = src inc
#PROJECT_SOURCEFILES += protocol.c

#CONTIKI_WITH_RIME = 1

#CONTIKI = ../..
#include $(CONTIKI)/Makefile.include

CONTIKI_PROJECT = research_protocol
all: $(CONTIKI_PROJECT)

PROJECT_SOURCEFILES += protocol_stack.c
PROJECT_SOURCEFILES += protocol_messages.c
PROJECT_SOURCEFILES += routing.c
PROJECT_SOURCEFILES += sys.c
PROJECT_SOURCEFILES += heart_beat.c



CONTIKI_WITH_RIME = 1
CONTIKI_WITH_IPV6 = 0

CONTIKI = ../..
include $(CONTIKI)/Makefile.include