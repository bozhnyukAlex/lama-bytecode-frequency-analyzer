#include <stdlib.h>
#include "byterun.h"
#include "hashtable/hashtable.h"
#include "hashtable/hashtable_utility.h"
#include "hashtable/hashtable_itr.h"
#include "string.h"
#include "stdio.h"
#include "malloc.h"

#define DEFINE_HASHTABLE_CHANGE(fnname, keytype, valuetype) \
int fnname (struct hashtable *h, keytype *k, valuetype *v) \
{ \
    return hashtable_change(h,k,v); \
}

typedef struct {
    const char *instruction_pointer;
    int length;
} Instruction;

typedef struct {
    Instruction *instruction;
    int count;
} InstructionEntry;

int entries_cmp(const InstructionEntry *a, const InstructionEntry *b) {
    return b->count - a->count;
}

unsigned int hash_instruction(const Instruction *instr) {
    size_t res = 0;
    for (int i = 0; i < instr->length; i++) {
        res = (res << 8) + instr->instruction_pointer[i];
    }
    return res;
}

DEFINE_HASHTABLE_SEARCH(count_instruction, Instruction, int)
DEFINE_HASHTABLE_INSERT(count_instruction_insert, Instruction, int)
DEFINE_HASHTABLE_CHANGE(count_instruction_change, Instruction, int)

int instr_cmp(Instruction *a, Instruction *b) {
    if (a->length != b->length) {
        return 0;
    }
    int res = memcmp(a->instruction_pointer, b->instruction_pointer, a->length);
    if (res == 0) {
        return 1;
    }
    return 0;
}

void eval_frequency(FILE *f, ByteFile *bf) {
    struct hashtable *ht = create_hashtable(10000, (unsigned int (*)(void *)) hash_instruction, (int (*)(void *, void *)) instr_cmp);
    const char *ip = bf->code_ptr;
    int entries_count = 0;
    while (ip < bf->code_ptr + bf->bytecode_size) {
        const char *new_ip = decode_instruction(0, bf, ip);
        Instruction *instr = malloc(sizeof(Instruction));
        if (instr == NULL) {
            failure("Could not init instruction");
            return;
        }
        instr->instruction_pointer = ip;
        instr->length = new_ip - ip;

        ip = new_ip;
        int *count = count_instruction(ht, instr);
        if (count == NULL) {
            entries_count++;
            int *cnt_new = malloc(sizeof(int));
            cnt_new[0] = 1;
            count_instruction_insert(ht, instr, cnt_new);
        } else {
            int *cnt_new = malloc(sizeof(int));
            cnt_new[0] = *count + 1;
            count_instruction_change(ht, instr, cnt_new);
        }
    }

    InstructionEntry *entries = malloc(entries_count * sizeof(InstructionEntry));
    int entry_pos = 0;

    struct hashtable_itr *ht_iter = hashtable_iterator(ht);
    do {
        Instruction *key = hashtable_iterator_key(ht_iter);
        int *val = hashtable_iterator_value(ht_iter);

        InstructionEntry entry = { .instruction = key, .count = *val };
        entries[entry_pos++] = entry;

    } while (hashtable_iterator_advance(ht_iter));

    qsort(entries, entries_count, sizeof(InstructionEntry), (__compar_fn_t) entries_cmp);
    for (int i = 0; i < entries_count; i++) {
        fprintf(f, "%d occurrences: \"", entries[i].count);
        decode_instruction(f, bf, entries[i].instruction->instruction_pointer);
        fprintf(f, "\"\n");
    }

    hashtable_destroy(ht, 1);
}


int main(int argc, char *argv[]) {
    ByteFile *bf = read_file(argv[1]);
    eval_frequency(stdout, bf);
    free(bf);
    return 0;
}
