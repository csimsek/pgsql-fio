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

struct dircontext {
    DIR *dir;
}; 

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
        return 0;
    }
    if (PG_ARGISNULL(1)) {
        elog(ERROR, "pattern must be specified");
        return 0;
    }
    pathname = text_to_cstring(PG_GETARG_TEXT_P(0));
    pattern = text_to_cstring(PG_GETARG_TEXT_P(1));
    if (SRF_IS_FIRSTCALL()) {
        MemoryContext oldcontext;
        TupleDesc tupdesc;
        dirctx = (struct dircontext *) palloc(sizeof(struct dircontext *));
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
            elog(ERROR, "cannot open dir: %s (%s)", pathname, strerror(errno));
            return 0;
        }
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
    dir = dirctx->dir;
    closedir(dir);
    pfree(dirctx);
    SRF_RETURN_DONE(funcctx);
}
