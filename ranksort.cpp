// g++-14 -std=c++20 -O3 -fopenmp -I/opt/homebrew/include -L/opt/homebrew/lib -ltbb ranksort.cpp -o ranksort
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <omp.h>
#include <tbb/parallel_sort.h>
using namespace std;
using namespace std::chrono;

// Structure to hold elements with their original indices
struct Element {
    double value;
    int index;

    // Comparison operator for sorting
    bool operator<(const Element& other) const {
        return (value != other.value) ? (value < other.value) : (index < other.index);
    }
};

// Pre-declare for tasking
void parallel_rank_sort_impl(Element* start, Element* end, Element* elements_base, Element* temp_base, int depth);

constexpr int SEQUENTIAL_CUTOFF = 16384;  // Threshold for switching to sequential sort
constexpr int PARALLEL_DEPTH = 5;        // Limit task creation depth

// Function to merge two sorted subarrays in parallel
void optimized_parallel_merge(Element* start, Element* mid, Element* end, Element* temp) {
    const size_t n1 = mid - start;
    const size_t n2 = end - mid;
    
    if (n1 == 0 || n2 == 0) return;

    // Pre-sort right half for better cache locality
    vector<Element> right_sorted(mid, end);
    #pragma omp parallel for simd
    for (size_t j = 0; j < n2; ++j) {
        right_sorted[j] = mid[j];
    }

    // Parallel rank calculations with SIMD
    #pragma omp parallel 
    {
        // Calculate ranks for elements in the left half
        #pragma omp for simd nowait
        for (size_t i = 0; i < n1; ++i) {
            const auto& elem = start[i];
            auto pos = lower_bound(right_sorted.begin(), right_sorted.end(), elem) - right_sorted.begin();
            temp[i + pos] = elem;
        }
        // Calculate ranks for elements in the right half
        #pragma omp for simd nowait
        for (size_t j = 0; j < n2; ++j) {
            const auto& elem = right_sorted[j];
            auto pos = upper_bound(start, mid, elem) - start;
            temp[pos + j] = elem;
        }
    }

    // Block-wise copy back using cache-friendly pattern
    const size_t block_size = 4096 / sizeof(Element);
    #pragma omp parallel for simd
    for (size_t i = 0; i < n1 + n2; i += block_size) {
        const size_t end_block = min(i + block_size, n1 + n2);
        copy(temp + i, temp + end_block, start + i);
    }
}
// Recursive function to perform parallel rank sort
void parallel_rank_sort_impl(Element* start, Element* end, Element* elements_base, Element* temp_base, int depth) {
    const size_t n = end - start;
    if (n <= SEQUENTIAL_CUTOFF) {
        // Use sequential sort for small arrays
        sort(start, end);
        return;
    }

    Element* mid = start + n/2;
    const size_t offset = start - elements_base;
    Element* local_temp = temp_base + offset;

    if (depth < PARALLEL_DEPTH) {
        // Create parallel tasks for sorting subarrays
        #pragma omp task default(none) firstprivate(start, mid, elements_base, temp_base, depth)
        parallel_rank_sort_impl(start, mid, elements_base, temp_base, depth + 1);
        
        #pragma omp task default(none) firstprivate(mid, end, elements_base, temp_base, depth)
        parallel_rank_sort_impl(mid, end, elements_base, temp_base, depth + 1);
        
        #pragma omp taskwait
    } else {
        // Sort subarrays sequentially if depth limit is reached
        parallel_rank_sort_impl(start, mid, elements_base, temp_base, depth);
        parallel_rank_sort_impl(mid, end, elements_base, temp_base, depth);
    }
        // Merge the sorted subarrays
    optimized_parallel_merge(start, mid, end, local_temp);
}
// Main function to perform parallel rank sort on a vector of doubles
void parallel_rank_sort(vector<double>& arr) {
    vector<Element> elements(arr.size());
    #pragma omp parallel for simd
    for (int i = 0; i < arr.size(); ++i) {
        elements[i] = {arr[i], i};
    }

    vector<Element> temp(arr.size());
    #pragma omp parallel
    #pragma omp single
    parallel_rank_sort_impl(elements.data(), elements.data() + elements.size(),
                            elements.data(), temp.data(), 0);

    #pragma omp parallel for simd
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = elements[i].value;
    }
}

// Main function to test the parallel rank sort

int main() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);

    vector<int> sizes = {1000, 10000, 100000, 1000000, 10000000, 100000000};
    int num_runs = 10;

    for (int n : sizes) {
        vector<double> arr(n);
        for (int i = 0; i < n; ++i) {
            arr[i] = dist(gen);
        }

        cout << "Sorting " << n << " elements using Parallel Rank Sort..." << endl;

        vector<double> runtimes;

        for (int run = 0; run < num_runs; ++run) {
            vector<double> temp(arr);
            auto start = high_resolution_clock::now();
            parallel_rank_sort(temp);
            auto stop = high_resolution_clock::now();
            double duration = duration_cast<milliseconds>(stop - start).count();
            runtimes.push_back(duration);
            
            // Verify sorting
            if (!is_sorted(temp.begin(), temp.end())) {
                cerr << "Sorting failed!" << endl;
                exit(1);
            }
        }

        double avg_time = accumulate(runtimes.begin() + 1, runtimes.end(), 0.0) / (num_runs - 1);
        cout << "Average runtime for n = " << n << ": " << avg_time << " ms\n" << endl;
    }

    return 0;
}