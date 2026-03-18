#ifndef FILE_HELPERS_H
#define FILE_HELPERS_H

// Source - https://stackoverflow.com/a/230068
// Posted by Graeme Perrow, modified by community. See post 'Timeline' for change history
// Retrieved 2026-03-16, License - CC BY-SA 4.0

#include <stddef.h>
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

static bool is_little_endian() {
    union {
        uint32_t i;
        uint8_t c[4];
    } e = { 0x01000000 };

    return e.c[0];
}

// converts to big-endian
static uint32_t bad_htonl(uint32_t val) {
    if (is_little_endian()) {
        return ((val & 0x000000FFU) << 24) |
               ((val & 0x0000FF00U) << 8)  |
               ((val & 0x00FF0000U) >> 8)  |
               ((val & 0xFF000000U) >> 24);

    } else {
        return val; // Already big-endian
    }
}

// converts to big-endian
static uint32_t bad_htons(uint16_t val) {
    if (is_little_endian()) {
        return (val << 8) | (val >> 8);
    } else {
        return val; // Already big-endian (network order)
    }
}

static void check_write_file_error(FILE *fptr) {
    if (feof(fptr) || ferror(fptr)) {
        perror("Couldn't write to file");
        fclose(fptr);
        exit(1);
    }
}

typedef enum {
    BIGENDIAN, LITTLEENDIAN
} Endianness;

static size_t write_endian(FILE *fptr, void *data, size_t bytes, Endianness endianness) {
    uint8_t *data_bytes = (uint8_t*)data;
    switch (endianness) {
    case BIGENDIAN:
        for (size_t i = bytes; i > 0; --i) {
            fwrite(data_bytes + (i-1), sizeof(uint8_t), 1, fptr);
            check_write_file_error(fptr);
        }
        break;
    case LITTLEENDIAN:
        for (size_t i = 0; i < bytes; ++i) {
            fwrite(data_bytes + i, sizeof(uint8_t), 1, fptr);
            check_write_file_error(fptr);
        }
        break;
    default:
        fprintf(stderr, "Unreachable\n");
        exit(1);
    };
    return bytes;
}

#endif /* FILE_HELPERS_H */
