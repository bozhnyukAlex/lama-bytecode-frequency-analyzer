COMMON_FLAGS=-g -O -fstack-protector-all


all: build hashtable.o hashtable_itr.o hashtable_utility.o

hashtable.o: hashtable.c
	gcc $(COMMON_FLAGS) -c hashtable.c -o build/hashtable.o

hashtable_itr.o: hashtable_itr.c
	gcc $(COMMON_FLAGS) -c hashtable_itr.c -o build/hashtable_itr.o

hashtable_utility.o: hashtable_utility.c
	gcc $(COMMON_FLAGS) -c hashtable_utility.c -o build/hashtable_utility.o

build:
	mkdir build
	touch build/.gitignore

clean:
	rm -r build

