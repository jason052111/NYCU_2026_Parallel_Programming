CC = gcc
CFLAGS = -O2
PROF_FLAGS = -pg
PERF_FLAGS = -g

all:
	$(CC) $(CFLAGS) pi.c -o pi.out

profile:
	$(CC) -O0 $(PROF_FLAGS) pi.c -o pi_prof.out

perf:
	$(CC) $(CFLAGS) $(PERF_FLAGS) pi.c -o pi.out

clean:
	rm -f pi.out pi_prof.out gmon.out perf.data perf.data.old profiling_result perf_result