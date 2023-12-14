/**
Copyright (c) 2015, Cafer Şimşek
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of pgsql-fio nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "fio.h"

PG_MODULE_MAGIC;

void _PG_init(void);
void _PG_fini(void);

void _PG_init(void)
{
}

void _PG_fini(void)
{
}

PG_FUNCTION_INFO_V1(fio_removefile);
PG_FUNCTION_INFO_V1(fio_renamefile);
PG_FUNCTION_INFO_V1(fio_writefile);
PG_FUNCTION_INFO_V1(fio_readfile);
PG_FUNCTION_INFO_V1(fio_readdir);
PG_FUNCTION_INFO_V1(fio_mkdir);
PG_FUNCTION_INFO_V1(fio_chmod);

Datum fio_writefile(PG_FUNCTION_ARGS) {
    // writefile(filename varchar, content bytea, mkdir boolean default false);

    text *vfilename;
    bytea *vcontent;
    char *filename;
    size_t contentsize;
    FILE *fd;
    char *buffer;
    size_t writesize;
    char *open_flag;

    if (PG_ARGISNULL(0)) {
        elog(ERROR, "filename must be specified");
        return 0;
    }
    vfilename = PG_GETARG_TEXT_P(0);

    if (PG_ARGISNULL(1)) {
        elog(ERROR, "content must be specified");
        return 0;
    }
    vcontent = PG_GETARG_BYTEA_P(1);

    filename = text_to_cstring(vfilename);
    contentsize = VARSIZE(vcontent) - VARHDRSZ;

    if (!PG_ARGISNULL(2)) {
        if (PG_GETARG_BOOL(2)) { // if it is recursive
            char* filename2 = strdup(filename);
            mkdir_recursive(dirname(filename2), 0777);
            //TODO: Why is this 0777?
            free(filename2);
        }
    }

    open_flag = (char *) palloc(sizeof(char) * 3);
    strncpy(open_flag, "wx\0", 3);
    if (!PG_ARGISNULL(3)) {
        if (PG_GETARG_BOOL(3)) {
            strncpy(open_flag, "w\0", 2);
        }
    }

    if ((fd = fopen(filename, open_flag)) == NULL) {
        int err = errno;
        elog(ERROR, "cannot open file: %s (%s)", filename, strerror(err));
        return 0;
    }
    buffer = text_to_cstring(vcontent);
    writesize = fwrite(buffer, 1, contentsize, fd);
    fclose(fd);
    pfree(open_flag);
    pfree(filename);
    return writesize;
}

Datum fio_renamefile(PG_FUNCTION_ARGS) {
    text *vfilename;
    text *vfilename_new;
    char *filename;
    char *filename_new;

    if (PG_ARGISNULL(0)) {
        elog(ERROR, "filename must be specified");
        return 0;
    }
    vfilename = PG_GETARG_TEXT_P(0);

    if (PG_ARGISNULL(1)) {
        elog(ERROR, "new filename must be specified");
        return 0;
    }
    vfilename_new = PG_GETARG_TEXT_P(1);

    filename = text_to_cstring(vfilename);
    filename_new = text_to_cstring(vfilename_new);

    if (access(filename_new, F_OK) != 0) {
        if (rename(filename, filename_new) != 0) {
            int err = errno;
            elog(ERROR, "cannot rename file: %s (%s)", filename, strerror(err));
            return 0;
        }
    } else {
        int err = errno;
        elog(ERROR, "new file %s already exists (%s) ", filename, strerror(err));
        return 0;
    }
    pfree(filename);
    pfree(filename_new);
    PG_RETURN_NULL();
}

Datum fio_removefile(PG_FUNCTION_ARGS) {
    text *vfilename;
    char *filename;

    if (PG_ARGISNULL(0)) {
        elog(ERROR, "filename must be specified");
        return 0;
    }
    vfilename = PG_GETARG_TEXT_P(0);

    filename = text_to_cstring(vfilename);

    if (remove(filename) != 0) {
        int err = errno;
        elog(ERROR, "cannot remove file: %s (%s)", filename, strerror(err));
        return 0;
    }
    pfree(filename);
    PG_RETURN_NULL();
}

Datum fio_readfile(PG_FUNCTION_ARGS) {
    // readfile(filename varchar);

    text *v_filename;
    char *filename;
    char *content;
    int totalcount = 0;
    char buffer[BUFFER_SIZE];
    FILE *fd;
    bytea *result;
    long filesize;
    int bufferedbytecount;
    DIR* dir = NULL;
    if (PG_ARGISNULL(0)) {
        elog(ERROR, "filename must be specified");
        return 0;
    }
    v_filename = PG_GETARG_TEXT_P(0);
    filename = text_to_cstring(v_filename);
    dir = opendir(filename);
    if (dir != NULL) {
        closedir(dir);
        elog(ERROR, "cannot open file: %s (not regular file)", filename);
        return 0;
    }
    if ((fd = fopen(filename, "r")) == NULL) {
        int err = errno;
        elog(ERROR, "cannot open file: %s (%s)", filename, strerror(err));
        return 0;
    }

    filesize = get_file_size(fd);
    content = (char *) palloc(filesize);
    do {
        bufferedbytecount = fread(buffer, 1, BUFFER_SIZE, fd);
        memcpy(content + totalcount, buffer, bufferedbytecount);
        totalcount += bufferedbytecount;
    } while (bufferedbytecount == BUFFER_SIZE);
    fclose(fd);
    result = (bytea *) palloc(VARHDRSZ + filesize);
    memcpy(VARDATA(result), content, filesize);
    pfree(content);
    SET_VARSIZE(result, filesize + VARHDRSZ);
    PG_RETURN_BYTEA_P(result);
}

Datum fio_readdir(PG_FUNCTION_ARGS) {
    // readdir(pathname varchar, pattern varchar default '*');

    FuncCallContext *funcctx;
    DIR *dir = NULL;
    char *pathname;
    char *pattern;
    struct dirent *dp;
    struct dircontext *dirctx;
    if (PG_ARGISNULL(0)) {
        elog(ERROR, "path must be specified");
    }
    if (PG_ARGISNULL(1)) {
        elog(ERROR, "pattern must be specified");
    }
    pathname = text_to_cstring(PG_GETARG_TEXT_P(0));
    pattern = text_to_cstring(PG_GETARG_TEXT_P(1));
    if (SRF_IS_FIRSTCALL()) {
        MemoryContext oldcontext;
        TupleDesc tupdesc;
        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
        if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("function returning record called in context "
                            "that cannot accept type record")));
        funcctx->attinmeta = TupleDescGetAttInMetadata(tupdesc);
        dir = opendir(pathname);
        if (dir == NULL) {
            int err = errno;
            elog(ERROR, "cannot open dir: %s (%s)", pathname, strerror(err));
        }
        dirctx = (struct dircontext *) palloc(sizeof(struct dircontext *));
        dirctx->dir = dir;
        funcctx->user_fctx = (void *) dirctx;
        MemoryContextSwitchTo(oldcontext);
    }
    funcctx = SRF_PERCALL_SETUP();
    dirctx = (struct dircontext *) funcctx->user_fctx;
    dir = dirctx->dir;
    while ((dp = readdir(dir)) != NULL) {
        if (fnmatch(pattern, dp->d_name, FNM_NOESCAPE) == 0) {
            HeapTuple tuple;
            Datum result;
            char *value = (char *) palloc(dp->d_reclen * sizeof(char));
            memcpy(value, dp->d_name, dp->d_reclen);
            tuple = BuildTupleFromCStrings(funcctx->attinmeta, &value);
            result = HeapTupleGetDatum(tuple);
            pfree(value);
            SRF_RETURN_NEXT(funcctx, result);
        }
    }
    closedir(dir);
    pfree(dirctx);
    SRF_RETURN_DONE(funcctx);
}

Datum fio_mkdir(PG_FUNCTION_ARGS) {
    // mkdir(pathname varchar, mode varchar, recursive boolean default false);

    text *v_pathname;
    char *pathname;
    text *vmode;
    size_t mode_ch_size;
    char *ch_mode;
    char *ch_mode_copy;
    int mode;
    bool recursive = FALSE;
    if (PG_ARGISNULL(0)) {
        elog(ERROR, "path must be specified");
        return 0;
    }
    v_pathname = PG_GETARG_TEXT_P(0);
    pathname = text_to_cstring(v_pathname);
    if (strlen(pathname) == 0) {
        elog(ERROR, "path must be specified");
        return 0;
    }
    if (PG_ARGISNULL(1)) {
        elog(ERROR, "mode must be specified");
        return 0;
    }
    vmode = PG_GETARG_TEXT_P(1);
    mode_ch_size = VARSIZE(vmode) - VARHDRSZ;
    ch_mode = text_to_cstring(vmode);
    ch_mode_copy= text_to_cstring(PG_GETARG_TEXT_P(1));
    mode = (int) strtol(ch_mode, &ch_mode_copy, 8);
    if (strncmp(ch_mode, ch_mode_copy, mode_ch_size) == 0) {
        elog(ERROR, "mode must be specified");
        return 0;
    }
    if (!PG_ARGISNULL(2)) {
        recursive = PG_GETARG_BOOL(2);
    }
    if (recursive) {
        mkdir_recursive(pathname, mode);
    } else {
        mkdir(pathname, mode);
        chmod(pathname, mode);
    }
    return 0;
}

Datum fio_chmod(PG_FUNCTION_ARGS) {
    // chmod(pathname varchar, mode varchar);

    text *v_pathname;
    char *pathname;
    text *vmode;
    size_t mode_ch_size;
    char *ch_mode;
    char *ch_mode_copy;
    int mode;
    if (PG_ARGISNULL(0)) {
        elog(ERROR, "path must be specified");
        return 0;
    }
    v_pathname = PG_GETARG_TEXT_P(0);
    pathname = text_to_cstring(v_pathname);
    if (PG_ARGISNULL(1)) {
        elog(ERROR, "mode must be specified");
        return 0;
    }
    vmode = PG_GETARG_TEXT_P(1);
    mode_ch_size = VARSIZE(vmode) - VARHDRSZ;
    ch_mode = text_to_cstring(vmode);
    ch_mode_copy = text_to_cstring(PG_GETARG_TEXT_P(1));
    mode = (int) strtol(ch_mode, &ch_mode_copy, 8);
    if (strncmp(ch_mode, ch_mode_copy, mode_ch_size) == 0) {
        elog(ERROR, "mode must be specified");
        return 0;
    }
    return chmod(pathname, mode);
}
