CC ?= cc
BASE = -std=c11 -Wall -Wextra

# clang と gcc でベクトル化無効化のオプション名が異なるため、コンパイラを判定する
ifneq (,$(findstring clang,$(shell $(CC) --version 2>&1)))
NOVEC = -fno-vectorize -fno-slp-vectorize -fno-unroll-loops
else
NOVEC = -fno-tree-vectorize -fno-tree-slp-vectorize -fno-unroll-loops
endif

all: bench_O2 bench_novec

# Default optimisation (auto-vectorisation/unrolling allowed)
bench_O2: cache_bench.c
	$(CC) $(BASE) -O2 -o $@ $<
# Confounder isolation: no vectorisation, no unrolling
bench_novec: cache_bench.c
	$(CC) $(BASE) -O2 $(NOVEC) -o $@ $<

asm:
	$(CC) $(BASE) -O2 -S -o asm_O2.s cache_bench.c
	$(CC) $(BASE) -O2 $(NOVEC) -S -o asm_novec.s cache_bench.c

run: all
	./bench_O2; ./bench_novec

clean:
	rm -f bench_O2 bench_novec asm_*.s
.PHONY: all asm run clean
