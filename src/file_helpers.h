#ifndef FILE_HELPERS_H
#define FILE_HELPERS_H

// Source - https://stackoverflow.com/a/230068
// Posted by Graeme Perrow, modified by community. See post 'Timeline' for change history
// Retrieved 2026-03-16, License - CC BY-SA 4.0

#ifdef WIN32
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static FILE *open_file(const char *fname, const char *mode) {
    FILE *fptr = fopen(fname, mode);
    if (fptr == NULL) {
        fprintf(stderr, "Coudln't open file %s for writing: %s\n", fname, strerror(errno));
        exit(1);
    }
    return fptr;
}

static void close_file(FILE *fptr) {
    if (fclose(fptr)) {
        perror("Coudln't close file");
        exit(1);
    }
}

static bool file_exists(const char *fname) {
    return access(fname, F_OK) == 0;
}

static void delete_file_if_exists(const char *fname) {
    if (!file_exists(fname)) { return; }
    if (remove(fname) != 0) {
        fprintf(stderr, "Couldn't delete file %s: %s", fname, strerror(errno));
    }
}


#endif /* FILE_HELPERS_H */
