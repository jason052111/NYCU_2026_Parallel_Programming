# Assignment 0: Monte Carlo Pi Estimation and Profiling

This lab is my first assignment for the Parallel Programming course. In this assignment, I implemented a serial Monte Carlo program to estimate pi, then used profiling tools to understand where the program spends most of its execution time.

## What I Implemented

The program estimates pi by randomly generating points inside a square from `(-1, -1)` to `(1, 1)`. The square has area `4`, and the inscribed circle has radius `1`, so its area is `pi`.

If the random points are uniformly distributed, then:

```text
points inside circle / total points ~= pi / 4
```

Therefore:

```text
pi ~= 4 * points inside circle / total points
```

For each random point `(x, y)`, I check whether it is inside the unit circle:

```c
x * x + y * y <= 1.0
```

The main computation is placed in `toss_darts()`, so profiling tools can clearly show where the program spends time.

## Files

```text
pi.c       Monte Carlo pi estimation program
Makefile   Build commands for normal, gprof, and perf versions
```

## Build and Run

Build the normal version:

```bash
make
```

Run the program:

```bash
./pi.out
```

Example output:

```text
3.141192
```

Clean generated files:

```bash
make clean
```

## Makefile Targets

```bash
make
```

Builds the normal executable:

```text
pi.out
```

```bash
make profile
```

Builds a `gprof` version with `-pg`:

```text
pi_prof.out
```

```bash
make perf
```

Builds a `perf` version with debug information:

```text
gcc -O2 -g pi.c -o pi.out
```

## Profiling With `time`

I first used `time` to measure the total execution time:

```bash
time ./pi.out
```

Example result:

```text
real    0m0.206s
user    0m0.205s
sys     0m0.000s
```

From this result, I learned that most of the time is spent in user-space CPU computation. The `sys` time is almost zero because this program does very little system-level work such as file I/O.

## Profiling With `gprof`

To use `gprof`, I compiled the program with `-pg`:

```bash
make profile
```

Then I ran the profiling executable:

```bash
./pi_prof.out
```

This generated:

```text
gmon.out
```

Then I used:

```bash
gprof -b ./pi_prof.out gmon.out
```

The flat profile showed:

```text
100.00%  toss_darts
```

From this result, I learned that `toss_darts()` is the hot code of this program. This makes sense because it contains the main loop that repeatedly generates random points and checks whether each point is inside the circle.

## Profiling With `perf`

For `perf`, I compiled the program with debug information:

```bash
make perf
```

Then I recorded CPU profiling data:

```bash
~/WSL2-Linux-Kernel/tools/perf/perf record -e cpu-cycles ./pi.out
```

Then I viewed the report:

```bash
~/WSL2-Linux-Kernel/tools/perf/perf report --stdio
```

Example result:

```text
Overhead  Command  Shared Object  Symbol
87.56%    pi.out   libc.so.6      [.] __random
5.05%     pi.out   libc.so.6      [.] __random_r
3.45%     pi.out   pi.out         [.] toss_darts
2.46%     pi.out   pi.out         [.] rand@plt
1.48%     pi.out   libc.so.6      [.] rand
```

From this result, I learned that although `gprof` shows `toss_darts()` as the hot function, `perf` gives a more detailed view. Most of the sampled execution time is inside `__random`, which is called by `rand()`. This means the main bottleneck of this Monte Carlo implementation is random number generation.

## WSL2 `perf` Setup Note

I used WSL2, and `perf` was not available by default. The system first reported:

```text
perf not found for kernel 6.6.87.2-microsoft
```

The required WSL2-specific `linux-tools` packages were also not available from `apt`, so I built `perf` from the Microsoft WSL2 Linux kernel source:

```bash
git clone --depth=1 --branch linux-msft-wsl-6.6.87.2 https://github.com/microsoft/WSL2-Linux-Kernel.git
cd ~/WSL2-Linux-Kernel/tools/perf
make -j$(nproc)
```

During the build, I installed missing dependencies such as `libtraceevent-dev`. I also rebuilt without Python support when the generated `perf` binary tried to load a missing Conda Python library:

```bash
make clean
make -j$(nproc) NO_LIBPYTHON=1
```

After that, `perf` worked:

```bash
./perf --version
```

Output:

```text
perf version 6.6.87.2.g427645e3db3a
```

## What I Learned

Through this assignment, I learned how Monte Carlo simulation can estimate pi using random sampling. I also learned the basic workflow of performance profiling:

- `time` measures the total execution time of the whole program.
- `gprof` shows which functions take most of the execution time.
- `perf` can inspect lower-level performance behavior and library calls.

The most important profiling lesson is that optimization should be guided by measurement. In this program, the main loop is the hot code, and within that loop, random number generation is the major cost.
