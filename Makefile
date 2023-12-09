TARGET = freq_analyzer
CC = gcc
COMMON_FLAGS=-g -fstack-protector-all
HASHTABLE_DIR = hashtable

all: build prepare_bc hash_table build/byterun.o build/frequency.o
	$(CC) $(COMMON_FLAGS) -o build/$(TARGET) build/byterun.o build/frequency.o $(HASHTABLE_DIR)/build/hashtable.o $(HASHTABLE_DIR)/build/hashtable_itr.o $(HASHTABLE_DIR)/build/hashtable_utility.o

hash_table:
	make -C $(HASHTABLE_DIR) all

build/frequency.o: frequency.c byterun.h
	$(CC) $(COMMON_FLAGS) -c frequency.c -o build/frequency.o

build/byterun.o: byterun.c byterun.h
	$(CC) $(COMMON_FLAGS) -c byterun.c -o build/byterun.o

prepare_bc:
	lamac -b Sort.lama

build:
	mkdir build
	touch build/.gitignore

clean:
	make -C $(HASHTABLE_DIR) clean
	$(RM) *.a *.o *.bc *~ -r build