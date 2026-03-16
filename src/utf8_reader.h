// simple utf-8 reader

#ifndef UTF8_READER_H
#define UTF8_READER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "file_helpers.h"

#define GET_PTR_TO_UINT32_SHIFTED_N_BYTES(i, shift) (((unsigned char*)(&c) + shift))

static bool read_byte(FILE *fptr, unsigned char *c, bool allow_eof) {
    *c = 0;
    int ci = fgetc(fptr);
    if (ci == EOF) {
        if (allow_eof && feof(fptr)) {
            return false;
        }
        fclose(fptr);
        perror("Couldn't read character in file");
        exit(1);
    }
    *c = (unsigned char)ci;
    return true;
}

static void exit_with_bad_utf8_format(uint8_t bytes_in_unicode_char, uint8_t bad_byte) {
    fprintf(stderr, "Error while parsing utf8 char with %u bytes (bad byte %u)\n", bytes_in_unicode_char, bad_byte);
    exit(1);
}

static uint8_t _get_byte_width(unsigned char c) {
    if ((c & 0x80) == 0) { return 1; }
    else if ((c & 0xF0) == 0xE0) { return 2; }
    else if ((c & 0xF8) == 0xF0) { return 3; }
    else if ((c & 0xF8) == 0xF0) { return 4; }
    else { return 0; }
}

static bool read_utf8_char(FILE *fptr, uint32_t *unicode_char, uint8_t *bytes_in_char) {
    // true means a character has been retrieved. false mean EOF
    /*
     * https://www.rfc-editor.org/rfc/rfc3629
     *
     * Char. number range  |        UTF-8 octet sequence
           (hexadecimal)    |              (binary)
        --------------------+---------------------------------------------
        0000 0000-0000 007F | 0xxxxxxx
        0000 0080-0000 07FF | 110xxxxx 10xxxxxx
        0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
        0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     */

    unsigned char b0;
    if (!read_byte(fptr, &b0, true)) {
        return false;
    }

    uint8_t n_bytes_in_char = _get_byte_width(b0);

    if (n_bytes_in_char == 1) {
        *unicode_char = b0;
    } else if (n_bytes_in_char == 2) {
        unsigned char b1;
        read_byte(fptr, &b1, false);
        if ((b1 & 0xC0) != 0x80) { exit_with_bad_utf8_format(2, 1); }

        *unicode_char =
            ((uint32_t)(b0 & 0x1F)) << 6 |
            ((uint32_t)(b1 & 0x3F));
    } else if (n_bytes_in_char == 3) {
        unsigned char b1, b2;
        read_byte(fptr, &b1, false);
        read_byte(fptr, &b2, false);
        if ((b1 & 0xC0) != 0x80) { exit_with_bad_utf8_format(3, 1); }
        if ((b2 & 0xC0) != 0x80) { exit_with_bad_utf8_format(3, 2); }

        *unicode_char =
            ((uint32_t)(b0 & 0x0F)) << 12 |
            ((uint32_t)(b1 & 0x3F)) << 6 |
            ((uint32_t)(b2 & 0x3F));
    } else if (n_bytes_in_char == 4) {
        unsigned char b1, b2, b3;
        read_byte(fptr, &b1, false);
        read_byte(fptr, &b2, false);
        read_byte(fptr, &b3, false);
        if ((b1 & 0xC0) != 0x80) { exit_with_bad_utf8_format(4, 1); }
        if ((b2 & 0xC0) != 0x80) { exit_with_bad_utf8_format(4, 2); }
        if ((b3 & 0xC0) != 0x80) { exit_with_bad_utf8_format(4, 3); }

        *unicode_char =
            ((uint32_t)(b0 & 0x07)) << 18 |
            ((uint32_t)(b1 & 0x3F)) << 12 |
            ((uint32_t)(b2 & 0x3F)) << 6 |
            ((uint32_t)(b3 & 0x3F));
    } else {
        exit_with_bad_utf8_format(1, 0);
    }

    if (bytes_in_char != NULL) {
        *bytes_in_char = n_bytes_in_char;
    }

    return true;
}

static size_t read_utf8(const char *fname, uint32_t *buffer) {
    fname = NULL;
    long size;
    uint32_t c;
    size_t i = 0;

    FILE *fptr = open_file(fname, "r");

    // Seek to the end of the file
    if (fseek(fptr, 0, SEEK_END) != 0) {
        perror("Error seeking to end of file");
        goto error;
    }

    // Get the current position, which is the size
    size = ftell(fptr);
    if (size == -1) {
        perror("Error getting file position");
        goto error;
    }

    if (fclose(fptr)) {
        perror("Coudln't close file");
        goto error;
    }

    buffer = (uint32_t*)malloc(sizeof(uint32_t) * size);
    while (read_utf8_char(fptr, &c, NULL)) {
        buffer[i++] = c;
    }

    error:
    fclose(fptr);
    exit(1);
}

#endif /* UTF8_READER_H */
