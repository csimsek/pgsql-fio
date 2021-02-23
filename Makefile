MODULE_big = fio

OBJS = src/fio.o       \
       src/renamefile.o \
       src/removefile.o \
       src/writefile.o \
       src/readfile.o  \
       src/readdir.o   \
       src/mkdir.o     \
       src/chmod.o     \
       src/utils.o

EXTENSION = fio
DATA = fio--1.0.sql
REGRESS = fio
EXTRA-CLEAN =

#PG_CONFIG = /usr/pgsql-9.3/bin/pg_config
PG_CONFIG = pg_config

SHLIB_LINK := $(LIBS)

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
