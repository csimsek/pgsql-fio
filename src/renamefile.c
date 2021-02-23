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
