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
