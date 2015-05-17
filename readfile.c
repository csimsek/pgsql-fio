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

#define BUFFER_SIZE 1024

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
    if (PG_ARGISNULL(0)) {
        elog(ERROR, "filename must be specified");
        return 0;
    }
    v_filename = PG_GETARG_TEXT_P(0);
    filename = text_to_cstring(v_filename);
    if ((fd = fopen(filename, "r+")) == NULL) {
        elog(ERROR, "cannot open file: %s", filename);
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
