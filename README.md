# PostgreSQL "basic" File I/O Functions

## Quick Start

This extension for some basic file system functions.

After checkout the code run that command:

`make install`

*Please note: Do not forget edit `Makefile` for `PG_CONFIG` entry.*

And then go to `psql` console and install the extension for your db

`$ psql dbname`

`dbname# CREATE EXTENSION fio;`

After creating extension, you can use functions with `fio_` prefix.

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

## Functions

##### `fio_chmod(pathname varchar, mode varchar);`
- **pathname**: the path's name you want to change mode
- **mode**: string for mode (ex: '0777')

##### `fio_mkdir(pathname varchar, mode varchar, recursive boolean default false)`
- **pathname**: the path's name you want to create
- **mode**: string for mode (ex: '0777')
- **recursive**: create directory recursion

##### `fio_readdir(pathname varchar, pattern varchar default '*')`
- **pathname**: the path's name you want to list of files/directories
- **pattern**: shell like pattern you want to filter (ex: '*.jpg')

##### `fio_readfile(filename varchar)`
- **filename**: The file's name you want to read. Return type is `bytea`.

##### `fio_writefile(filename varchar, content bytea, mkdir boolean default false, overwrite boolean DEFAULT false)`
- **filename**: The file's name you want to create/write.
- **content**: What content you want to write the file.
- **mkdir**: If `true`, it creates directory with given `filename`. Creates directory with recursion option.
- **overwrite**: If `true` overwrite file if already exists. If `false` and file exists it fails.


##### `fio_removefile(filename varchar)`
- **filename**: The file's name you want to remove.

##### `fio_renamefile(filename varchar, newfilename varchar)`
- **filename**: The file's name you want to rename.
- **newfilename**: The new file's name

