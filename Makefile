MODULE_big = fio

OBJS = src/fio.o       \
       src/utils.o

EXTENSION = fio
DATA = fio--1.0.sql
EXTRA-CLEAN =

#PG_CONFIG = /usr/pgsql-9.3/bin/pg_config
PG_CONFIG = pg_config

SHLIB_LINK := $(LIBS)

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
