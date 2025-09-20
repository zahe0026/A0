#include <stdio.h>     // fprintf, stdout, stderr, fopen, fread, fclose, ferror
#include <stdlib.h>    // exit, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>    // strerror
#include <errno.h>     // errno
#include <sys/stat.h>  // struct stat, stat()

// Fejlhåndteringsfunktion fra opgavebeskrivelsen
int print_error(char *path, int errnum) {
    return fprintf(stdout, "%s: cannot determine (%s)\n",
                   path, strerror(errnum));
}

// Filtype enum
typedef enum {
    FILE_DATA = 0,
    FILE_EMPTY,
    FILE_ASCII,
    FILE_ISO_8859_1,
    FILE_UTF8,
} file_type;

// Tilsvarende strenge til output
static const char * const FILE_TYPE_STRINGS[] = {
    "data",
    "empty",
    "ASCII text",
    "ISO-8859-1 text",
    "UTF-8 Unicode text",  // Rettet stavefejl
};

// Tjek om byte er tilladt ASCII jf. opgavespecifikationen
static int is_allowed_ascii(unsigned char b) {
    if (b >= 0x07 && b <= 0x0D) return 1;  // Kontroltegn
    if (b == 0x1B) return 1;                // ESC
    if (b >= 0x20 && b <= 0x7E) return 1;  // Printable ASCII
    return 0;
}

static file_type detect_type(const char *path, const struct stat *st) {
    // Tom fil kontrol
    if (st->st_size == 0) {
        return FILE_EMPTY;
    }

    FILE *f = fopen(path, "rb");
    if (!f) {
        return FILE_DATA;
    }

    // Flag til at spore gyldighed af hver encoding
    int ascii_ok = 1;
    int iso_ok = 1;
    int utf8_ok = 1;
    int utf8_need = 0;  // Rettet stavefejl: var uft8_need
    int saw_utf8_multibyte = 0;

    unsigned char buf[4096];
    size_t n;

    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        for (size_t i = 0; i < n; ++i) {
            unsigned char b = buf[i];

            // ASCII kontrol
            if (ascii_ok && !is_allowed_ascii(b)) {
                ascii_ok = 0;
            }

            // ISO-8859-1 kontrol
            if (iso_ok) {
                if (is_allowed_ascii(b)) {
                    // Gyldig ASCII delmængde
                } else if (b >= 160) {  // ← Kun én }
                    // Gyldige ISO-8859-1 udvidede tegn
                } else {
                    // Bytes 128-159 er ikke del af ISO-8859-1
                    iso_ok = 0;
                }
            }

            // UTF-8 kontrol
            if (utf8_ok) {
                if (utf8_need > 0) {
                    // Forventer fortsættelsesbyte (10xxxxxx)
                    if ((b & 0xC0) == 0x80) {
                        utf8_need--;  // KRITISK RETTELSE: dekrementer tæller
                    } else {
                        utf8_ok = 0;
                    }
                } else {
                    // Forventer nyt tegn start
                    if ((b & 0x80) == 0x00) {
                        // 1-byte (ASCII)
                        if (!is_allowed_ascii(b)) utf8_ok = 0;
                    } else if ((b & 0xE0) == 0xC0) {
                        // 2-byte sekvens (110xxxxx)
                        utf8_need = 1;
                        saw_utf8_multibyte = 1;
                    } else if ((b & 0xF0) == 0xE0) {
                        // 3-byte sekvens (1110xxxx)
                        utf8_need = 2;
                        saw_utf8_multibyte = 1;
                    } else if ((b & 0xF8) == 0xF0) {
                        // 4-byte sekvens (11110xxx)
                        utf8_need = 3;
                        saw_utf8_multibyte = 1;
                    } else {
                        // Ugyldig UTF-8 start byte
                        utf8_ok = 0;
                    }
                }
            }
        }
    }

    int ioerr = ferror(f);
    fclose(f);
    if (ioerr) return FILE_DATA;

    // Prioritet: ASCII > UTF-8 > ISO-8859-1 > data
    if (ascii_ok) return FILE_ASCII;

    // UTF-8 skal have komplette sekvenser og indeholde multibyte tegn
    if (utf8_ok && utf8_need == 0 && saw_utf8_multibyte) return FILE_UTF8;

    if (iso_ok) return FILE_ISO_8859_1;

    return FILE_DATA;
}

int main(int argc, char *argv[]) {
    // Tjek antal argumenter
    if (argc != 2) {
        fprintf(stderr, "Usage: file path\n");
        return EXIT_FAILURE;
    }

    char *path = argv[1];
    struct stat st;

    // Hent filstatistik
    if (stat(path, &st) != 0) {
        print_error(path, errno);
        return EXIT_SUCCESS;  // Jf. opgaven: exit 0 selv ved I/O fejl
    }

    file_type t = detect_type(path, &st);
    printf("%s: %s\n", path, FILE_TYPE_STRINGS[t]);
    return EXIT_SUCCESS;
}