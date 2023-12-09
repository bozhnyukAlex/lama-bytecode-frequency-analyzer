
#include "stdio.h"
# include <stdlib.h>
# include <string.h>
# include <stdarg.h>
#include <stdint.h>
#include "errno.h"
#include "byterun.h"


void flog(FILE *f, const char *pat, ...) {
    if (f == NULL) {
        return;
    }

    va_list args;
    va_start(args, pat);
    vfprintf(f, pat, args);
}

static void vfailure(char *s, va_list args) {
    fprintf(stderr, "*** FAILURE: ");
    vfprintf(stderr, s, args);   // vprintf (char *, va_list) <-> printf (char *, ...)
    exit(255);
}

void failure(char *s, ...) {
    va_list args;

    va_start(args, s);
    vfailure(s, args);
}

ByteFile *read_file(char *fname) {
    FILE *f = fopen(fname, "rb");
    long size;
    ByteFile *file;

    if (f == 0) {
        failure("%s\n", strerror(errno));
    }

    if (fseek(f, 0, SEEK_END) == -1) {
        failure("%s\n", strerror(errno));
    }

    file = (ByteFile *) malloc(sizeof(int32_t) * 5 + (size = ftell(f)));

    if (file == 0) {
        failure("unable to allocate memory.\n");
    }

    rewind(f);

    if (size != fread(&file->stringtab_size, 1, size, f)) {
        failure("%s\n", strerror(errno));
    }

    fclose(f);

    file->string_ptr = &file->buffer[file->public_symbols_number * 2 * sizeof(int)];
    file->public_ptr = (int32_t *) file->buffer;
    file->code_ptr = &file->string_ptr[file->stringtab_size];
    file->global_ptr = (int32_t *) malloc(file->global_area_size * sizeof(int));
    file->bytecode_size = (char *) &file->stringtab_size + size - file->code_ptr;

    return file;
}

/* Gets a string from a string table by an index */
char *get_string_from_table(ByteFile *f, int pos) {
    return f->string_ptr + pos;
}

const char *decode_instruction(FILE *f, ByteFile *bf, const char *ip) {
#define READ_BYTE  (*ip++)
#define READ_INT (ip += sizeof(int), *(int*)(ip - sizeof(int)))
#define READ_STRING get_string_from_table(bf, READ_INT)
#define FAIL  failure ("ERROR: invalid opcode %d-%d\n", first_bits, second_bits)
    static const char *const ops[] = {"+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "!!"};
    static const char *const pats[] = {"=str", "#string", "#array", "#sexp", "#ref", "#val", "#fun"};
    static const char *const lds[] = {"LD", "LDA", "ST"};

    char instruction = READ_BYTE;
    char first_bits = (char) ((instruction & 0xF0) >> 4);
    char second_bits = (char) (instruction & 0x0F);

    if (first_bits == 15) {
        flog(f, "STOP");
        return ip;
    }

    if (first_bits == I_BINOP) {
        flog(f, "BINOP\t%s", ops[second_bits]);
        return ip;
    }

    if (first_bits == I_LD || first_bits == I_LDA || first_bits == I_ST) {
        flog(f, "%s\t", lds[first_bits - 2]);
        switch (second_bits) {
            case 0:
                flog(f, "G(%d)", READ_INT);
                break;
            case 1:
                flog(f, "L(%d)", READ_INT);
                break;
            case 2:
                flog(f, "A(%d)", READ_INT);
                break;
            case 3:
                flog(f, "C(%d)", READ_INT);
                break;
            default:
                FAIL;
        }
        return ip;
    }

    if (first_bits == I_PATT) {
        flog(f, "PATT\t%s", pats[second_bits]);
        flog(f, "\n");
        return ip;
    }

    switch (instruction) {
        case I_CONST: {
            flog(f, "CONST\t%d", READ_INT);
            break;
        }
        case I_STRING: {
            flog(f, "STRING\t%s", READ_STRING);
            break;
        }
        case I_SEXP: {
            flog(f, "SEXP\t%s ", READ_STRING);
            flog(f, "%d", READ_INT);
            break;
        }
        case I_STI: {
            flog(f, "STI");
            break;
        }
        case I_STA: {
            flog(f, "STA");
            break;
        }
        case I_JMP: {
            flog(f, "JMP\t0x%.8x", READ_INT);
            break;
        }
        case I_CJMP_Z: {
            flog(f, "CJMPz\t0x%.8x", READ_INT);
            break;
        }
        case I_CJMP_NZ: {
            flog(f, "CJMPnz\t0x%.8x", READ_INT);
            break;
        }
        case I_CLOSURE: {
            flog(f, "CLOSURE\t0x%.8x", READ_INT);
            int n = READ_INT;
            for (int i = 0; i < n; i++) {
                switch (READ_BYTE) {
                    case 0:
                        flog(f, "G(%d)", READ_INT);
                        break;
                    case 1:
                        flog(f, "L(%d)", READ_INT);
                        break;
                    case 2:
                        flog(f, "A(%d)", READ_INT);
                        break;
                    case 3:
                        flog(f, "C(%d)", READ_INT);
                        break;
                    default:
                        FAIL;
                }
            }
            break;
        }
        case I_ELEM: {
            flog(f, "ELEM");
            break;
        }
        case I_BEGIN: {
            flog(f, "BEGIN\t%d ", READ_INT);
            flog(f, "%d", READ_INT);
            break;
        }
        case I_CBEGIN: {
            flog(f, "CBEGIN\t%d ", READ_INT);
            flog(f, "%d", READ_INT);
            break;
        }
        case I_CALL: {
            flog(f, "CALL\t0x%.8x ", READ_INT);
            flog(f, "%d", READ_INT);
            break;
        }
        case I_CALLC: {
            flog(f, "CALLC\t%d", READ_INT);
            break;
        }
        case I_CALL_READ: {
            flog(f, "CALL\tLread");
            break;
        }
        case I_CALL_WRITE: {
            flog(f, "CALL\tLwrite");
            break;
        };
        case I_CALL_STRING: {
            flog(f, "CALL\tLstring");
            break;
        }
        case I_CALL_LENGTH: {
            flog(f, "CALL\tLlength");
            break;
        }
        case I_CALL_ARRAY: {
            flog(f, "CALL\tBarray\t%d", READ_INT);
            break;
        }
        case I_END: {
            flog(f, "END");
            break;
        }
        case I_DROP: {
            flog(f, "DROP");
            break;
        }
        case I_DUP: {
            flog(f, "DUP");
            break;
        }
        case I_TAG: {
            flog(f, "TAG\t%s ", READ_STRING);
            flog(f, "%d", READ_INT);
            break;
        }
        case I_SWAP: {
            flog(f, "SWAP");
            break;
        }
        case I_ARRAY: {
            flog(f, "ARRAY\t%d", READ_INT);
            break;
        }
        case I_FAIL: {
            flog(f, "FAIL\t%d", READ_INT);
            flog(f, "%d", READ_INT);
            break;
        }
        case I_LINE: {
            flog(f, "LINE\t%d", READ_INT);
            break;
        }
        case I_RET: {
            flog(f, "RET");
            break;
        }
        default: {
            FAIL;
        }
    }

    return ip;
}
