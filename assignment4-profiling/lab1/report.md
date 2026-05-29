# Intro Profiling Lab Report

## 1. Optimizations Made

1. Remove memory leaks - delete the distance and visited arrays in shortest_bfs_path before returning.
2. Remove noinline attributes and make to_index, in_bounds, is_open inline.
3. Process in row-major order in compute_congestion_pressure to take advantage of cache-locality.
4. Inline next_pressure_value.
5. Hoist distance, visited and frontier arrays in shortest_path_bfs to top - prevents repeatedly allocating and deallocating memory on heap for the three arrays in each call.
6. Replace fill of visited with counter - Instead of 1 representing visited, maintain a counter variable for the current bfs call number and check visited based on that. Removes the need to fill the visited array (vis) on each call.
7. Use suffix sums to make threshold loop in summarize_heatmap O(n) instead of O(n^2).
8. Make transition arrays in shortest_path_bfs (drow and dcol) static so that they're only constructed once.
9. Remove branching in run_all_requests.

## 2. Methodology Walkthrough

I mainly used perf and time. I tried to optimize the hot-paths, which turned out to be compute_congestions_pressure and shortest_path_bfs. After each change I timed it's performance (using time and the in-program chrono clock) to compare to the previous version.

### Before:

time:

![time before](images/time_before.png)

perf stat:

![perf stat before](images/perf_stat_before.png)

flamegraph:

![flamegraph before](images/flamegraph_before.svg)

valgrind:

![valgrind before](images/valgrind_before.png)

callgrind:

![callgrind before](images/callgrind_before.png)

### After:

time:

![time after](images/time_after.png)

perf stat:

![perf stat after](images/perf_stat_after.png)

flamegraph:

![flamegraph after](images/flamegraph_after.svg)

valgrind:

![valgrind after](images/valgrind_after.png)

callgrind:

![callgrind after](images/callgrind_after.png)

## 3. Correctness Evidence

make test:

![make test](images/make_test.png)

Output after optimization:

![output](images/output.png)

Output before optimization:

![output old](images/output_old.png)

Checksums are same.

## 4. Conceptual Questions

Answer Q1.1 through Q6.1 from the README.

Q1.1: user and sys measure the actual CPU time, ie the time the CPU spent computing something. real measures the total time the process takes (wall-clock time), which includes the CPU time along with time spent for other things not handled by the CPU such as waiting for user input or a network request.

Q2.1: perf uses hardware counters to keep track of the event counts. It uses multiplexing to count more events than hardware counters available. Afterwards, it applies formulae in software to calculate the derived matrix.

Q2.2: The percentage represents event multiplexing. It means that this event was only monitored for that percent of time. To get the value over for the entire run, perf stat scales the raw count to get an estimated value. This is required as there may not be enough hardware counters on the CPU as the number of events perf has to calculate.

Q2.3: As explained in Q2.2, it is extrapolated from the raw count using the multiplexing percentage, and is thus an approximation, not an exact value.

Q3.1: Every function call has an associated stack frame keeping track of the return address, arguments and local variables. The frame pointer stores the base address of this stack frame (and does not change during the execution of the function). perf -g follows a process called stack walking. As it samples the CPU state based on the frequency, it takes the current frame pointer, reads the return address, goes to the frame pointer of the parent function and repeats until it reaches the outermost function. This way it is able to trace the function call chain and reconstruct the call stack.

Q3.2: Inclusive cost is the total time spent inside a function including the time spent in the child functions called by it. Self cost is only the time spent in the function excluding the time spent in the child calls.

Q4.1: gprof instruments the compiled binary with instructions to record the data. It maintains function call counts and whenever a function is called the specific call count is incremented. It inspects the call stack to get the stack trace and figure out which function called the current function. Because of all this overhead, running a gprof instrumented binary is slower than running the normal binary.

Q4.2: perf gives us just approximation of the event counts and not the exact value. FlameGraphs are made from perf data so they are also based on approximations. We use gprof to get the exact counts.

Q5.1: Valgrind Memcheck is like an emulator that runs the already compiled binary directly. AddressSanitizer is built into the compiler and instruments the binary with specific tracking instructions. AddressSanitizer is much faster than Valgrind. Valgrind should be used when we want to run it directly without recompiling the binary. Also it can be used to detect uninitialized memory usage, which AddressSanitizer does not do. AddressSanitizer should generally be preferred for speed and requires recompiling. It is useful for detecting buffer overflows (stack/global), which is not possible through Valgrind.

Q6.1: Yes, perf record and gprof had differences in their measured counts (specifically the percentage of the time that a function ran). It is mainly due to the difference in measurement method. Perf samples the cpu hardware counters regularly and then extrapolates to get an approximate value. On the other hand, gprof instruments the binary to keep track, adding overhead which might skew the results. perf is a more accurate tool to use.
