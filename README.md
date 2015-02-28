# PostgreSQL "basic" File I/O Functions

This extension for some basic file system functions.

After checkout the code run that command:

`make install`

And then go to `psql` console and install the extension for your db

`$ psql dbname`

`dbname# CREATE EXTENSION fio;`

After creating extenison you can use functions with `fio_` prefix.

For example:

    dbname=# select fio_readdir('/usr/', '*');
      fio_readdir 
    -------------
     (include)
     (src)
     (lib64)
     (.)
     (tmp)
     (pgsql-9.3)
     (bin)
     (..)
     (libexec)
     (local)
     (lib)
     (share)
     (games)
     (pgsql-9.4)
     (etc)
     (java)
     (sbin)
    (17 rows)
    dbname=# 
