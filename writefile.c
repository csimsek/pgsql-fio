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
    text *vfilename;
    bytea *vcontent;
    bool mkdir1;
    char *filename;
    size_t contentsize;
    FILE *fd;
    char *buffer;
    vfilename = PG_GETARG_TEXT_P(0);
    vcontent = PG_GETARG_BYTEA_P(1);
    mkdir1 = PG_GETARG_BOOL(2);
    filename = text_to_cstring(vfilename);
    contentsize = VARSIZE(vcontent) - VARHDRSZ;
    if (mkdir1) {
        char* filename2 = strdup(filename);
        mkdir_recursive(dirname(filename2), 0777);
    }
    if ((fd = fopen(filename, "w+")) == NULL) {
        elog(ERROR, "cannot open file: %s", filename);
        return 0;
    }
    buffer = text_to_cstring(vcontent);
    fwrite(buffer, 1, contentsize, fd);
    fclose(fd);
    return contentsize;
}
