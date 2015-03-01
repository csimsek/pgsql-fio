#include "fio.h"

int mkdir_recursive(const char *dir, mode_t mode) {
    int result;
    char tmp[256];
    char *p = NULL;
    size_t len;
    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
	if ( len < 1 ) {
		return 0;
	}
	
    if(tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }
    for(p = tmp + 1; *p; p++) {
        if(*p == '/') {
            *p = 0;
            mkdir(tmp, mode);
            chmod(tmp, mode);
            *p = '/';
        }
    }
    result = mkdir(tmp, mode);
    chmod(tmp, mode);
    return result;
}

long getfilesize(FILE *fd) {
    long result;
    long oldpos = ftell(fd);
    fseek(fd, 0, SEEK_END);
    result = ftell(fd);
    fseek(fd, oldpos, SEEK_SET);
    return result;
}
