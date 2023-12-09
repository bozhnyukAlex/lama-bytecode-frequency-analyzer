#ifndef LAMA_BYTECODE_INTERPRETER_BYTERUN_H
#define LAMA_BYTECODE_INTERPRETER_BYTERUN_H

#include <stdio.h>

typedef struct {
    char *string_ptr;              /* A pointer to the beginning of the string table */
    int *public_ptr;              /* A pointer to the beginning of publics table    */
    char *code_ptr;                /* A pointer to the bytecode itself               */
    int *global_ptr;              /* A pointer to the global area                   */
    int stringtab_size;          /* The size (in bytes) of the string table        */
    int global_area_size;        /* The size (in words) of global area             */
    int public_symbols_number;   /* The number of public symbols                   */
    char buffer[0];
    int bytecode_size;
} ByteFile;

typedef enum {
    I_BINOP = 0, // binary operator
    I_LD = 2, // load a variable to the stack
    I_LDA = 3, // load a variable address to the stack
    I_ST = 4, // store a value into a variable
    I_PATT = 6, // checks various patterns
} OpCodeComplex;

typedef enum {
    I_CONST = 0x10, // put a constant on the stack
    I_STRING = 0x11, // put a string on the stack
    I_SEXP = 0x12, // create an S-expression
    I_STI = 0x13, // store a value into a reference
    I_STA = 0x14, // store a value into array/sexp/string
    I_JMP = 0x15, // unconditional jump
    I_END = 0x16, // end procedure definition
    I_RET = 0x17, // returns from a function
    I_DROP = 0x18, // drops the top element off
    I_DUP = 0x19, // duplicates the top element
    I_SWAP = 0x1a, // swaps two top elements
    I_ELEM = 0x1b, // takes an element of array/string/sexp
    I_CJMP_Z = 0x50, // conditional jump
    I_CJMP_NZ = 0x51, // conditional jump
    I_BEGIN = 0x52, // begins procedure definition (with no closure)
    I_CBEGIN = 0x53, // begins procedure definition (with a closure)
    I_CLOSURE = 0x54, // create a closure
    I_CALLC = 0x55, // calls a closure
    I_CALL = 0x56, // calls a function/procedure
    I_TAG = 0x57, // checks the tag and arity of S-expression
    I_ARRAY = 0x58, // checks the tag and size of array
    I_FAIL = 0x59, // match failure (location, leave a value)
    I_LINE = 0x5a,// line info
    I_CALL_READ = 0x70,
    I_CALL_WRITE = 0x71,
    I_CALL_LENGTH = 0x72,
    I_CALL_STRING = 0x73,
    I_CALL_ARRAY = 0x74,
} OpCode;

ByteFile *read_file(char *fname);
const char *decode_instruction(FILE *f, ByteFile *bf, const char *ip);
void failure (char *s, ...);
#endif //LAMA_BYTECODE_INTERPRETER_BYTERUN_H
