#include <stdio.h>     // printf, fprintf
#include <stdlib.h>    // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>    // strerror
#include <errno.h>     // errno
#include <sys/stat.h>  // struct stat, stat()


// Den givet hjælpefunktion fra opgavebeskrivelsen 
int print_error(char *path, int errnum) {
    return fprintf(stdout, "%s: cannot determine (%s)\n",
                   path, strerror(errnum));
}



// Filtyper samt deres udskrifter
typedef enum {
    FILE_DATA = 0,
    FILE_EMPTY,
    FILE_ASCII,
    FILE_ISO_8859_1,
    FILE_UTF8,
} file_type;



static const char * const FILE_TYPE_STRINGS[] = {
    "data",
    "empty",
    "ASCII text",
    "ISO-8859-1 text",
    "UFT-8 Unicode text",
};


// De tilladte ASCII-bytes fra opgavebeskrivelsen
static int is_allowed_ascii(unsigned char b) {
    if (b >= 0x07 && b <= 0x0D) return 1;
    if (b == 0x1B) return 1;
    if (b >= 0x20 && b <= 0x7E) return 1;
    return 0;
}


static file_type detect_type(const char *path, const struct stat *st) {
    if (st->st_size == 0) {
        return FILE_EMPTY;
    }

    FILE *f = fopen(path, "rb");
    if (!f) {

        return FILE_DATA;
    }

// Vi opdaterer flagene i én læsning.
    int ascii_ok = 1;
    int iso_ok   = 1;
    int utf8_ok  = 1;
    int uft8_need = 1;
    int saw_utf8_multibyte = 0;
   
    unsigned char buf[4096];
    size_t n;

    while ((n = fread(buf, 1, sizeof buf, f)) > 0) {
        for (size_t i = 0; i < n; ++i) {
            unsigned char b = buf[i];

            
            if (ascii_ok && !is_allowed_ascii(b)) ascii_ok = 0;

            
            if (iso_ok) {
                if (is_allowed_ascii(b)) {
                    
                } else if (b >= 160) {
                   
                } else {
                    
                    iso_ok = 0;
                }
            }

            
            if (utf8_ok) {
                if (uft8_need > 0) {
                    if ((b & 0xC0) == 0x80) {
                    } else {
                        utf8_ok = 0;
                    }
                } else {
                    if ((b & 0x80) == 0x00) {
                        
                        if (!is_allowed_ascii(b)) utf8_ok = 0;
                    } else if ((b & 0xE0) == 0xC0) {
                         uft8_need = 1;      
                        saw_utf8_multibyte = 1;
                    } else if ((b & 0xF0) == 0xE0) {
                          uft8_need = 2;     
                        saw_utf8_multibyte = 1;
                    } else if ((b & 0xF8) == 0xF0) {
                        uft8_need = 3;      
                        saw_utf8_multibyte = 1;
                    } else {
                       
                        utf8_ok = 0;
                    }
                }
            }
        }
    }

    int ioerr = ferror(f);
    fclose(f);
    if (ioerr) return FILE_DATA;

    if (ascii_ok) return FILE_ASCII;

    
    if (utf8_ok && uft8_need == 0 && saw_utf8_multibyte) return FILE_UTF8;

    if (iso_ok) return FILE_ISO_8859_1;

    return FILE_DATA;
}


int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        fprintf(stderr, "Usage: file path\n");
        return EXIT_FAILURE;   
    }

    char *path = argv[1];
    struct stat st;

    
    if (stat(path, &st) != 0) {
        print_error(path, errno);
        return EXIT_SUCCESS; 
    }  

    file_type t = detect_type (path, &st);
    printf("%s: %s\n", path, FILE_TYPE_STRINGS[t]);
    return EXIT_SUCCESS; 
}





   







