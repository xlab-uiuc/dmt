#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define TEA_MODE(vmid, is_free, nested_lvl) (0ULL | (((uint64_t)(vmid)) << 32) | ((is_free) ? 0x100 : 0) | ((nested_lvl) & 0xff))
#define TEA_VMID(mode) ((mode) >> 32)
#define TEA_IS_FREE(mode) ((mode) & 0x100)
#define TEA_NESTED_LVL(mode) ((mode) & 0xff)

static inline unsigned long sys_alloc_tea(unsigned long gva, unsigned long gpa, unsigned long pgcnt, unsigned long mode)
{
	return syscall(450, gva, gpa, pgcnt, mode);
}

static inline long kvm_hypercall0(unsigned int nr)
{
	long ret;
	asm volatile("vmcall"
		     : "=a"(ret)
		     : "a"(nr)
		     : "memory");
	return ret;
}

int main(int argc, char *argv[])
{
	uint64_t result;
	int i, j;
	clock_t t, ttl = 0;
	double time_taken;

	if (argc < 3) {
		printf("dmt-hc [TEA size in MB] [nested level]\n");
		return 0;
	}

	int mbs = strtol(argv[1], NULL, 0);
	int nested = strtol(argv[2], NULL, 0);

	// tweak these configurations for different settigns
	int iter = 20;
	int pages = 256 * mbs; // 200 MB

	ttl = 0;
	for (i = 0; i < iter; i++) {
		t = clock();

		for (j = 0; j < 1000; j++) {
			kvm_hypercall0(99);
		}

		t = clock() - t;
		ttl += t;
	}

	time_taken = ((double)ttl) / CLOCKS_PER_SEC / iter * 1000;
	printf("1000 hypercall took %f ms to execute\n", time_taken);

	ttl = 0;
	for (i = 0; i < iter; i++) {
		t = clock();

		result = sys_alloc_tea(1, 0x1145141919000ULL, pages, TEA_MODE(0, 0, nested));
		if (result)
			printf("damn1\n");

		t = clock() - t;
		ttl += t;

		result = sys_alloc_tea(1, 0x1145141919000ULL, pages, TEA_MODE(0, 1, nested));
		if (result)
			printf("damn2\n");
	}

	time_taken = ((double)ttl) / CLOCKS_PER_SEC / iter * 1000;
	printf("fun() took %f ms to execute\n", time_taken);

	return 0;
}
