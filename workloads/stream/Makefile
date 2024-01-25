CC = gcc
CFLAGS = -O2 -fopenmp -mno-avx

FC = gfortran
FFLAGS = -O2 -fopenmp -mno-avx

all: stream_f.exe stream_c.exe stream stream_numa

stream_f.exe: stream.f mysecond.o
	$(CC) $(CFLAGS) -c mysecond.c
	$(FC) $(FFLAGS) -c stream.f
	$(FC) $(FFLAGS) stream.o mysecond.o -o stream_f.exe

stream_c.exe: stream.c
	$(CC) $(CFLAGS) stream.c -o stream_c.exe -lnuma

stream_numa : stream.c
	$(CC) $(CFLAGS) stream.c -o stream_numa -DSTREAM_ARRAY_SIZE=1342177280 -lnuma

stream : stream.c
	$(CC) $(CFLAGS) stream.c -o stream -D__RUN_FOREVER__ -DSTREAM_ARRAY_SIZE=1000000000


clean:
	rm -f stream_f.exe stream_c.exe *.o stream
	rm -f *.a

# an example of a more complex build line for the Intel icc compiler
stream.icc: stream.c
	icc -O3 -xCORE-AVX2 -ffreestanding -qopenmp -DSTREAM_ARRAY_SIZE=80000000 -DNTIMES=20 stream.c -o stream.omp.AVX2.80M.20x.icc
