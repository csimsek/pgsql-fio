-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION fio" to load this file. \quit

CREATE OR REPLACE FUNCTION fio_writefile(filename varchar, content bytea, mkdir boolean default false, overwrite boolean default false)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'fio_writefile'
    LANGUAGE 'c';
CREATE OR REPLACE FUNCTION fio_readfile(filename varchar)
    RETURNS bytea
    AS 'MODULE_PATHNAME', 'fio_readfile'
    LANGUAGE 'c';
CREATE OR REPLACE FUNCTION fio_removefile(filename varchar)
    RETURNS void
    AS 'MODULE_PATHNAME', 'fio_removefile'
    LANGUAGE 'c';
CREATE OR REPLACE FUNCTION fio_renamefile(filename varchar, newfilename varchar)
    RETURNS void
    AS 'MODULE_PATHNAME', 'fio_renamefile'
    LANGUAGE 'c';
CREATE TYPE readdirresult AS (
    dirname varchar
);
CREATE OR REPLACE FUNCTION fio_readdir(pathname varchar, pattern varchar default '*')
    RETURNS SETOF readdirresult
    AS 'MODULE_PATHNAME', 'fio_readdir'
    LANGUAGE 'c';
CREATE OR REPLACE FUNCTION fio_mkdir(pathname varchar, mode varchar, recursive boolean default false)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'fio_mkdir'
    LANGUAGE 'c';
CREATE OR REPLACE FUNCTION fio_chmod(pathname varchar, mode varchar)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'fio_chmod'
    LANGUAGE 'c';
