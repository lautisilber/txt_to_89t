#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "bad_hashtable.h"
#include "utf8_reader.h"
#include "file_helpers.h"

#define MIN(a, b) (a > b ? b : a)

static BadHashTable_t unicode_ti_table;

static void fill_unicode_ti_table() {
    bad_ht_initialize(&unicode_ti_table);

    bad_ht_insert(&unicode_ti_table, "\u00BB", 22); // »
    bad_ht_insert(&unicode_ti_table, "\u00BC", 28); // ¼
    bad_ht_insert(&unicode_ti_table, "\u00BD", 29); // ½
    bad_ht_insert(&unicode_ti_table, "\u00BE", 30); // ¾
    bad_ht_insert(&unicode_ti_table, "\u00BF", 31); // ¿
    bad_ht_insert(&unicode_ti_table, "\u20AC", 128); // €
    // bad_ht_insert(&unicode_ti_table, "\u0081", 129); //
    bad_ht_insert(&unicode_ti_table, "\u201A", 130); // ‚
    bad_ht_insert(&unicode_ti_table, "\u0192", 131); // ƒ
    bad_ht_insert(&unicode_ti_table, "\u201E", 132); // „
    bad_ht_insert(&unicode_ti_table, "\u2026", 133); // …
    bad_ht_insert(&unicode_ti_table, "\u2020", 134); // †
    bad_ht_insert(&unicode_ti_table, "\u2021", 135); // ‡
    bad_ht_insert(&unicode_ti_table, "\u02C6", 136); // ˆ
    bad_ht_insert(&unicode_ti_table, "\u2030", 137); // ‰
    bad_ht_insert(&unicode_ti_table, "\u0160", 138); // Š
    bad_ht_insert(&unicode_ti_table, "\u2039", 139); // ‹
    bad_ht_insert(&unicode_ti_table, "\u0152", 140); // Œ
    // bad_ht_insert(&unicode_ti_table, "\u008D", 141); //
    bad_ht_insert(&unicode_ti_table, "\u017D", 142); // Ž
    // bad_ht_insert(&unicode_ti_table, "\u008F", 143); //
    // bad_ht_insert(&unicode_ti_table, "\u0090", 144); //
    bad_ht_insert(&unicode_ti_table, "\u2018", 145); // ‘
    bad_ht_insert(&unicode_ti_table, "\u2019", 146); // ’
    bad_ht_insert(&unicode_ti_table, "\u201C", 147); // “
    bad_ht_insert(&unicode_ti_table, "\u201D", 148); // ”
    bad_ht_insert(&unicode_ti_table, "\u2013", 149); // –
    bad_ht_insert(&unicode_ti_table, "\u2014", 150); // —
    bad_ht_insert(&unicode_ti_table, "\u00C6", 151); // Æ
    bad_ht_insert(&unicode_ti_table, "\u02DC", 152); // ˜
    bad_ht_insert(&unicode_ti_table, "\u2122", 153); // ™
    bad_ht_insert(&unicode_ti_table, "\u0161", 154); // š
    bad_ht_insert(&unicode_ti_table, "\u203A", 155); // ›
    bad_ht_insert(&unicode_ti_table, "\u0153", 156); // œ
    // bad_ht_insert(&unicode_ti_table, "\u009D", 157); //
    bad_ht_insert(&unicode_ti_table, "\u017E", 158); // ž
    bad_ht_insert(&unicode_ti_table, "\u0178", 159); // Ÿ
    bad_ht_insert(&unicode_ti_table, "\u00A1", 160); // ¡
    bad_ht_insert(&unicode_ti_table, "\u00A2", 161); // ¢
    bad_ht_insert(&unicode_ti_table, "\u00A3", 162); // £
    bad_ht_insert(&unicode_ti_table, "\u00A4", 163); // ¤
    bad_ht_insert(&unicode_ti_table, "\u00A5", 164); // ¥
    bad_ht_insert(&unicode_ti_table, "\u00A6", 165); // ¦
    // bad_ht_insert(&unicode_ti_table, "\u007C", 166); // |
    bad_ht_insert(&unicode_ti_table, "\u00A7", 168); // §
    bad_ht_insert(&unicode_ti_table, "\u00AA", 170); // ª
    bad_ht_insert(&unicode_ti_table, "\u00D7", 171); // ×
    bad_ht_insert(&unicode_ti_table, "\u00A9", 172); // ©
    bad_ht_insert(&unicode_ti_table, "\u00A8", 174); // ¨
    bad_ht_insert(&unicode_ti_table, "\u00AB", 175); // «
    bad_ht_insert(&unicode_ti_table, "\u00AC", 176); // ¬
    bad_ht_insert(&unicode_ti_table, "\u00AD", 177); // ­
    bad_ht_insert(&unicode_ti_table, "\u00AE", 178); // ®
    bad_ht_insert(&unicode_ti_table, "\u00AF", 179); // ¯
    bad_ht_insert(&unicode_ti_table, "\u00B0", 180); // °
    bad_ht_insert(&unicode_ti_table, "\u00B1", 181); // ±
    bad_ht_insert(&unicode_ti_table, "\u00B2", 183); // ²
    bad_ht_insert(&unicode_ti_table, "\u00B3", 184); // ³
    bad_ht_insert(&unicode_ti_table, "\u00B4", 185); // ´
    bad_ht_insert(&unicode_ti_table, "\u00B5", 186); // µ
    bad_ht_insert(&unicode_ti_table, "\u00DE", 187); // Þ
    bad_ht_insert(&unicode_ti_table, "\u00B6", 188); // ¶
    bad_ht_insert(&unicode_ti_table, "\u00B7", 189); // ·
    bad_ht_insert(&unicode_ti_table, "\u00B8", 190); // ¸
    bad_ht_insert(&unicode_ti_table, "\u00B9", 191); // ¹
    bad_ht_insert(&unicode_ti_table, "\u00C0", 192); // À
    bad_ht_insert(&unicode_ti_table, "\u00C1", 193); // Á
    bad_ht_insert(&unicode_ti_table, "\u00C2", 194); // Â
    bad_ht_insert(&unicode_ti_table, "\u00C3", 195); // Ã
    bad_ht_insert(&unicode_ti_table, "\u00C4", 196); // Ä
    bad_ht_insert(&unicode_ti_table, "\u00C5", 197); // Å
    bad_ht_insert(&unicode_ti_table, "\u00C7", 199); // Ç
    bad_ht_insert(&unicode_ti_table, "\u00C8", 200); // È
    bad_ht_insert(&unicode_ti_table, "\u00C9", 201); // É
    bad_ht_insert(&unicode_ti_table, "\u00CA", 202); // Ê
    bad_ht_insert(&unicode_ti_table, "\u00CB", 203); // Ë
    bad_ht_insert(&unicode_ti_table, "\u00CC", 205); // Ì
    bad_ht_insert(&unicode_ti_table, "\u00CD", 206); // Í
    bad_ht_insert(&unicode_ti_table, "\u00CF", 207); // Ï
    bad_ht_insert(&unicode_ti_table, "\u00D0", 18); // Ð
    bad_ht_insert(&unicode_ti_table, "\u00D1", 209); // Ñ
    bad_ht_insert(&unicode_ti_table, "\u00D2", 210); // Ò
    bad_ht_insert(&unicode_ti_table, "\u00D3", 211); // Ó
    bad_ht_insert(&unicode_ti_table, "\u00D4", 212); // Ô
    bad_ht_insert(&unicode_ti_table, "\u00D5", 213); // Õ
    bad_ht_insert(&unicode_ti_table, "\u00D6", 214); // Ö
    // bad_ht_insert(&unicode_ti_table, "\u0078", 215); // x
    bad_ht_insert(&unicode_ti_table, "\u00D8", 216); // Ø
    bad_ht_insert(&unicode_ti_table, "\u00D9", 217); // Ù
    bad_ht_insert(&unicode_ti_table, "\u00DA", 218); // Ú
    bad_ht_insert(&unicode_ti_table, "\u00DB", 219); // Û
    bad_ht_insert(&unicode_ti_table, "\u00DC", 220); // Ü
    bad_ht_insert(&unicode_ti_table, "\u00DD", 221); // Ý
    bad_ht_insert(&unicode_ti_table, "\u00DF", 223); // ß
    bad_ht_insert(&unicode_ti_table, "\u00E0", 224); // à
    bad_ht_insert(&unicode_ti_table, "\u00E1", 225); // á
    bad_ht_insert(&unicode_ti_table, "\u00E2", 226); // â
    bad_ht_insert(&unicode_ti_table, "\u00E3", 227); // ã
    bad_ht_insert(&unicode_ti_table, "\u00E4", 228); // ä
    bad_ht_insert(&unicode_ti_table, "\u00E5", 229); // å
    bad_ht_insert(&unicode_ti_table, "\u00E6", 230); // æ
    bad_ht_insert(&unicode_ti_table, "\u00E7", 231); // ç
    bad_ht_insert(&unicode_ti_table, "\u00E8", 232); // è
    bad_ht_insert(&unicode_ti_table, "\u00E9", 233); // é
    bad_ht_insert(&unicode_ti_table, "\u00EA", 234); // ê
    bad_ht_insert(&unicode_ti_table, "\u00EB", 235); // ë
    bad_ht_insert(&unicode_ti_table, "\u00EC", 236); // ì
    bad_ht_insert(&unicode_ti_table, "\u00ED", 237); // í
    bad_ht_insert(&unicode_ti_table, "\u00EE", 238); // î
    bad_ht_insert(&unicode_ti_table, "\u00EF", 239); // ï
    bad_ht_insert(&unicode_ti_table, "\u00F0", 240); // ð
    bad_ht_insert(&unicode_ti_table, "\u00F1", 241); // ñ
    bad_ht_insert(&unicode_ti_table, "\u00F2", 242); // ò
    bad_ht_insert(&unicode_ti_table, "\u00F3", 243); // ó
    bad_ht_insert(&unicode_ti_table, "\u00F4", 244); // ô
    bad_ht_insert(&unicode_ti_table, "\u00F5", 245); // õ
    bad_ht_insert(&unicode_ti_table, "\u00F6", 246); // ö
    bad_ht_insert(&unicode_ti_table, "\u00F7", 247); // ÷
    bad_ht_insert(&unicode_ti_table, "\u00F8", 248); // ø
    bad_ht_insert(&unicode_ti_table, "\u00F9", 249); // ù
    bad_ht_insert(&unicode_ti_table, "\u00FA", 250); // ú
    bad_ht_insert(&unicode_ti_table, "\u00FB", 251); // û
    bad_ht_insert(&unicode_ti_table, "\u00FC", 252); // ü
    bad_ht_insert(&unicode_ti_table, "\u00FD", 253); // ý
    bad_ht_insert(&unicode_ti_table, "\u00FE", 254); // þ
    bad_ht_insert(&unicode_ti_table, "\u00FF", 255); // ÿ
}

// SupportedFormat
typedef enum {
    TI92P, TI89, TI92
} SupportedFormat;
static const char *_supported_format_strs[] = {
    "**TI92P*", "**TI89**", "**TI92**"
};
static const char *supported_format_to_str(SupportedFormat sf) {
    switch (sf) {
    case TI92P: return _supported_format_strs[0];
    case TI89: return _supported_format_strs[1];
    case TI92: return _supported_format_strs[2];
    default:
        fprintf(stderr, "unreachable on %s:%d\n",__FILE__, __LINE__);
        exit(1); // unreachable
    }
}

// DataType
typedef enum {
    TEXT = 0x0B,
    NOTEFOLIO = 0x1C,
    IMAGE = 0x10
} DataType;

// StoreType
typedef enum {
    RAM = 0,
    RAM_LOCKED = 1,
    ARCHIVE = 2
} StoreType;

// TIWriterConfig
#define FOLDER_NAME_LENGTH 8
#define VARIABLE_NAME_LENGTH 8
#define COMMENT_LENGTH 40
typedef struct {
    SupportedFormat format;
    StoreType store_type;
    const char *folder_name;
    const char *variable_name;
    const char *comment;
} TIWriterConfig;

static size_t write_ubyte(FILE *fptr, uint8_t i) {
    fwrite(&i, 1, sizeof(uint8_t), fptr);
    check_write_file_error(fptr);
    return sizeof(uint8_t);
}

static size_t write_ushort(FILE *fptr, uint16_t i) {
    // fwrite(&i, 1, sizeof(uint16_t), fptr);
    // check_write_file_error(fptr);
    // return sizeof(uint16_t);
    return write_endian(fptr, &i, sizeof(uint16_t), BIGENDIAN);
}

static size_t write_uint(FILE *fptr, uint32_t i) {
    // fwrite(&i, 1, sizeof(uint32_t), fptr);
    // check_write_file_error(fptr);
    // return sizeof(uint32_t);
    return write_endian(fptr, &i, sizeof(uint32_t), BIGENDIAN);
}

static size_t write_chararr_fixed_size(FILE *fptr, const char *str, size_t size, size_t fixed_size) {
    fwrite(str, MIN(size, fixed_size), sizeof(char), fptr);
    check_write_file_error(fptr);
    for (size_t i = size; i < fixed_size; ++i) {
        fputc(0x0, fptr);
        check_write_file_error(fptr);
    }
    return sizeof(char) * fixed_size;
}

static size_t write_chararr(FILE *fptr, const char *str, size_t size) {
    return write_chararr_fixed_size(fptr, str, size, size);
}

static size_t write_str_fixed_size(FILE *fptr, const char *str, size_t fixed_size) {
    const size_t size = strlen(str);
    return write_chararr_fixed_size(fptr, str, size, fixed_size);
}

static size_t write_str(FILE *fptr, const char *str) {
    return write_chararr(fptr, str, strlen(str));
}

static void write_header(FILE *fptr, const TIWriterConfig *config, size_t content_file_size, DataType data_type) {
    size_t file_size = 0;

    file_size += write_str(fptr, supported_format_to_str(config->format));
    file_size += write_ushort(fptr, 0x1 << 8);
    file_size += write_str_fixed_size(fptr, config->folder_name, FOLDER_NAME_LENGTH);
    file_size += write_str_fixed_size(fptr, config->comment, COMMENT_LENGTH);
    file_size += write_ushort(fptr, 0x1 << 8); // number of vars = 1
    file_size += write_uint(fptr, 0x52 << 24); // data adress
    // file_size += write_uint(fptr, 0x52 << 8); // data adress
    file_size += write_str_fixed_size(fptr, config->variable_name, VARIABLE_NAME_LENGTH);
    file_size += write_ubyte(fptr, data_type);
    file_size += write_ubyte(fptr, config->store_type);
    // file_size += write_ushort(fptr, 0xFF); // ?
    file_size += write_ushort(fptr, 0xFF << 8); // ?

    file_size += content_file_size + 8;

    write_ubyte(fptr, file_size & 0xFF);
    write_ubyte(fptr, (file_size >> 8) & 0xFF);
    write_ubyte(fptr, (file_size >> 24) & 0xFF);
    write_ubyte(fptr, (file_size >> 16) & 0xFF);
    write_ushort(fptr, 0xA55A); // 2 bytes separator
    write_ushort(fptr, 0x0); // 2 bytes separator
}

static void write_checksum(FILE *fptr, size_t checksum) {
    write_ubyte(fptr, checksum & 0xFF); // hi a low bytes of crc sum
    write_ubyte(fptr, checksum >> 8);
}

// TITextWriter
typedef struct {
    TIWriterConfig config;
    const uint32_t *text_content;
    size_t text_content_size;
} TITextWriter;

static void ti_text_write(FILE *fptr, const TITextWriter *t) {
    // get content size
    unsigned int content_size = t->text_content_size;

    // count newlines
    for (size_t i = 0; i < t->text_content_size; ++i) {
        if (t->text_content[i] == 0x0A) { // newline
            ++content_size;
            printf("R\n");
        }
    }
    printf("tc %lu, c: %u\n", t->text_content_size, content_size);

    write_header(fptr, &t->config, content_size + 7, TEXT);

    write_uint(fptr, content_size);
    write_ubyte(fptr, 0x0);
    write_ubyte(fptr, 0x1); // start of text
    write_ubyte(fptr, 0x20); // TODO: idk if this is needed

    unsigned int sum = (content_size & 0xFF) + ((content_size >> 8) & 0xFF);

    for (size_t i = 0; i < t->text_content_size; ++i) {
        uint32_t c = t->text_content[i];
        printf("%u (%c)\n", c, c);
        if (c <= 0x7F) { // if it's an ascii character
            if (c == 0x0A) { // new line
                const char c2 = 0x0D; // carriage return
                write_ubyte(fptr, c2);
                sum += c2;
                c = 0x20; // space
            } else if (c == 0x0D) { // carriage return -> skip
                continue;
            }
        } else {
            uint8_t value;
            bool special_char = bad_ht_get(&unicode_ti_table, c, &value);
            if (special_char) {
                c = value;
            } else {
                fprintf(stderr, "Invalid code %u\n", c);
                exit(1);
            }
        }
        sum += c;
        write_ubyte(fptr, c);
    }
    write_ushort(fptr, 0xE0); // end of file

    size_t checksum = sum + 0xE0 + 0x1;
    write_checksum(fptr, checksum);
}

void print_usage() {

}

// argument 1 -> .txt source
// argument 2 -> .89t output
// argument 3 -> variable
// argument 4 -> folder

#ifdef DEBUG
#define FNAME_TXT "test/test.txt"
#define FNAME_89T "examples/test_a.89t"
#define VARIABLE "test"
#define FOLDER "main"
#endif

int main(int argc, char *argv[]) {
    #ifndef DEBUG
    if (argc == 1) {
        print_usage();
    } else if (argc < 5) {
        printf("Error: bad arguments");
        print_usage();
    }

    const char *fname_txt = argv[1];
    const char *fname_89t = argv[2];
    const char *variable = argv[3];
    const char *folder = argv[4];
    #else
    const char *fname_txt = FNAME_TXT;
    const char *fname_89t = FNAME_89T;
    const char *variable = VARIABLE;
    const char *folder = FOLDER;
    #endif

    const size_t vlen = strlen(variable);
    const size_t flen = strlen(folder);
    if (vlen == 0 || vlen > VARIABLE_NAME_LENGTH) {
        fprintf(stderr, "Variable has to be at least one char long and up to %u chars long\n", VARIABLE_NAME_LENGTH);
        exit(1);
    }
    if (flen == 0 || flen > FOLDER_NAME_LENGTH) {
        fprintf(stderr, "Folder name has to be at least one char long and up to %u chars long\n", FOLDER_NAME_LENGTH);
        exit(1);
    }

    uint32_t *text_content = NULL;
    const char *a = fname_89t;
    size_t text_content_size = read_utf8(fname_txt, &text_content);

    TITextWriter config = {
        .config = {
            .comment = "comment",
            .folder_name = folder,
            .variable_name = variable,
            .format = TI92P,
            .store_type = ARCHIVE,
        },
        .text_content = text_content,
        .text_content_size = text_content_size,
    };

    // delete old output file
    delete_file_if_exists(fname_89t);
    FILE *out_file_ptr = open_file(fname_89t, "wb");

    ti_text_write(out_file_ptr, &config);

    close_file(out_file_ptr);

    free(text_content);

    puts("done");

    return 0;
}
