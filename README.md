# Sorting Algorithms Benchmark

This repository contains implementations of various sorting algorithms and their benchmarks. Each file demonstrates a different approach to sorting large datasets, with optimizations for performance and scalability.

## File Descriptions

### [mergesort.cpp](mergesort.cpp)
- **Description**: Implements a basic merge sort algorithm with an optimized merge function using a preallocated auxiliary array.
- **Key Features**:
  - Recursive merge sort implementation.
  - Preallocated auxiliary array for efficient merging.
- **Usage**: Benchmarks the performance of merge sort on arrays of varying sizes.

---

### [mergesortk.cpp](mergesortk.cpp)
- **Description**: Extends the basic merge sort by introducing a threshold `k` for switching to insertion sort for small subarrays.
- **Key Features**:
  - Combines merge sort with insertion sort for small subarrays.
  - Benchmarks the effect of different `k` values on performance.
- **Usage**: Finds the optimal `k` value for merge sort on arrays of varying sizes.

---

### [mergesortt.cpp](mergesortt.cpp)
- **Description**: Implements a parallel merge sort using threads, with an adjustable `MIN_THREAD_SIZE` to control when threading is used.
- **Key Features**:
  - Parallel merge sort using C++ threads.
  - Benchmarks the effect of different `MIN_THREAD_SIZE` values on performance.
- **Usage**: Determines the best `MIN_THREAD_SIZE` for parallel merge sort on arrays of varying sizes.

---

### [mergesorttk.cpp](mergesorttk.cpp)
- **Description**: Combines the optimizations from `mergesortk.cpp` and `mergesortt.cpp`, using both insertion sort for small subarrays and threading for large subarrays.
- **Key Features**:
  - Hybrid approach combining insertion sort and parallel merge sort.
  - Benchmarks the effect of both `k` and `MIN_THREAD_SIZE` on performance.
- **Usage**: Optimizes both `k` and `MIN_THREAD_SIZE` for hybrid merge sort on arrays of varying sizes.


---

### [ranksort.cpp](ranksort.cpp)
- **Description**: Implements a parallel rank sort algorithm using OpenMP and Intel TBB for parallelism.
- **Key Features**:
  - Parallel rank sort with SIMD optimizations.
  - Uses OpenMP tasks and Intel TBB for efficient parallelism.
  - Includes a custom merge function for cache-friendly merging.
- **Usage**: Benchmarks the performance of parallel rank sort on arrays of varying sizes.

---

### [sort.cpp](sort.cpp)
- **Description**: Benchmarks the performance of the standard C++ `std::sort` function on arrays of varying sizes.
- **Key Features**:
  - Uses high-quality random number generation for input data.
  - Measures average runtime over multiple runs.
- **Usage**: Provides a baseline for comparing custom sorting algorithms.

---

1. **Compile**:
   Use `g++` with appropriate flags for each file. For example:
   ```bash
   g++ -std=c++20 -O3 -march=native -flto -o mergesort mergesort.cpp
   g++ -std=c++20 -O3 -march=native -flto -o mergesort mergesortt.cpp
   g++ -std=c++20 -O3 -march=native -flto -o mergesort mergesortk.cpp
   g++ -std=c++20 -O3 -march=native -flto -o mergesort mergesorttk.cpp
   g++ -std=c++20 -O3 -march=native -flto -o mergesort sort.cpp
   g++-14 -std=c++20 -O3 -fopenmp -I/opt/homebrew/include -L/opt/homebrew/lib -ltbb ranksort.cpp -o ranksort
   ```
2. **Run**:
   Execute the compiled binary. For example:
   ```bash
   ./mergesort
   ./mergesortk
   ./mergesortt
   ./mergesorttk
   ./ranksort
   ./sort
   ```

---

**Benchmarking**

Each file benchmarks the performance of its respective sorting algorithm on arrays of sizes ranging from 1,000 to 100,000,000 elements. The results include average runtimes over multiple runs, with optimizations for specific parameters (e.g., k, MIN_THREAD_SIZE).

**Notes**

- Ensure that your system has sufficient memory to handle large arrays.
- For ranksort.cpp, make sure OpenMP and Intel TBB are installed and properly configured.
- The benchmarks exclude the first run to account for potential warm-up effects.

**Link to Raw Data Spreadsheet**: 
https://docs.google.com/spreadsheets/d/1_c6K8cKKW7dYrXtD5qOvt-yyaip5SkLGytWiwcro2X4/edit?usp=sharing 

https://docs.google.com/spreadsheets/d/1_c6K8cKKW7dYrXtD5qOvt-yyaip5SkLGytWiwcro2X4/edit?usp=sharing 
   
