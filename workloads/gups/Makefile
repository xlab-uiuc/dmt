
all: libgupsmp.a libgups.a

libgupsmp.a: gupsmp.o
	ar rcs $@ $^

libgups.a: gups.o
	ar rcs $@ $^	

libgupstoy.a: gupstoy.o
	ar rcs $@ $^	

gupsmp.o : gups.c
	gcc -c -O3 -mno-avx -fopenmp -o $@ $< 

gups.o : gups.c
	gcc -c -O3 -mno-avx -o $@ $< 

gupstoy.o : gupstoy.c
	gcc -c -O3 -mno-avx -o $@ $< 

clean:
	rm -f *.o *.a

