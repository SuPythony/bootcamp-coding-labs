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

Before:

![time before](images/time_before.png)
![perf stat before](images/perf_stat_before.png)
![flamegraph before](images/flamegraph_before.svg)
![valgrind before](images/valgrind_before.png)
![callgrind before](images/callgrind_before.png)

After:

![time after](images/time_after.png)
![perf stat after](images/perf_stat_after.png)
![flamegraph after](images/flamegraph_after.svg)
![valgrind after](images/valgrind_after.png)
![callgrind after](images/callgrind_after.png)

## 3. Correctness Evidence

Include:

- `make test`
- Final normal run output
- Checksum comparison before and after optimization

## 4. Conceptual Questions

Answer Q1.1 through Q6.1 from the README.
