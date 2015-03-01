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

Datum fio_mkdir(PG_FUNCTION_ARGS) {
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
