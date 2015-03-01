
MODULE_big = fio
OBJS = fio.o writefile.o readfile.o readdir.o mkdir.o chmod.o utils.o
EXTENSION = fio
DATA = fio--1.0.sql
REGRESS = fio
EXTRA-CLEAN =

#PG_CONFIG = /usr/pgsql-9.3/bin/pg_config
PG_CONFIG = pg_config

SHLIB_LINK := $(LIBS)

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
