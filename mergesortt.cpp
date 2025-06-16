#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
#include <cstring>  // For std::memcpy
#include <thread>

using namespace std;
using namespace std::chrono;

const int MAX_THREADS = thread::hardware_concurrency(); // Get number of CPU cores

// Optimized Merge function using a preallocated auxiliary array
void merge(vector<double>& arr, vector<double>& aux, int left, int mid, int right) {
    memcpy(aux.data() + left, arr.data() + left, (right - left + 1) * sizeof(double));

    int i = left, j = mid + 1, k = left;
    
    while (i <= mid && j <= right) {
        arr[k++] = (aux[i] <= aux[j]) ? aux[i++] : aux[j++];
    }

    while (i <= mid) arr[k++] = aux[i++];
    while (j <= right) arr[k++] = aux[j++];
}

// Parallel Merge Sort with adjustable MIN_THREAD_SIZE
void mergeSort(vector<double>& arr, vector<double>& aux, int left, int right, int MIN_THREAD_SIZE) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // Use threads if the subarray is large enough
        if ((right - left) > MIN_THREAD_SIZE) {
            thread leftThread(mergeSort, ref(arr), ref(aux), left, mid, MIN_THREAD_SIZE);
            mergeSort(arr, aux, mid + 1, right, MIN_THREAD_SIZE);
            leftThread.join(); // Ensure left half is sorted before merging
        } else {
            mergeSort(arr, aux, left, mid, MIN_THREAD_SIZE);
            mergeSort(arr, aux, mid + 1, right, MIN_THREAD_SIZE);
        }

        merge(arr, aux, left, mid, right);
    }
}

// int main() {
//     // Use high-quality random number generator
//     random_device rd;
//     mt19937 gen(rd());
//     uniform_real_distribution<double> dist(0.0, 1.0);

//     vector<int> sizes = {1000, 10000, 100000, 1000000, 10000000, 100000000}; // Large array sizes for testing
//     vector<int> thread_sizes = {10000, 50000, 100000, 250000, 500000, 1000000}; // Different MIN_THREAD_SIZE values
//     int num_runs = 10; // Number of times to run each test
//     for (int n : sizes) {
//         vector<double> arr(n);
//         arr.reserve(n);  // Preallocate memory
//         for (int i = 0; i < n; i++) {
//             arr[i] = dist(gen);
//         }
//         cout << "Sorting " << n << " elements..." << endl;
//         vector<double> runtimes;
//         vector<double> aux(n); // Preallocate auxiliary array
//         int min_thread_size = max(10000,n/(4*MAX_THREADS));
//         for (int run = 0; run < num_runs; run++) {
//             vector<double> temp(n);
//             memcpy(temp.data(), arr.data(), n * sizeof(double)); // Faster copying

//             auto start = high_resolution_clock::now();
//             mergeSort(temp, aux, 0, n - 1, min_thread_size);
//             auto stop = high_resolution_clock::now();

//             double duration = duration_cast<milliseconds>(stop - start).count();
//             runtimes.push_back(duration);
            
//         }

//         // Compute average runtime
//         double sum = 0;
//         for (double time : runtimes) {
//             sum += time;
//         }
//         double avg_time = sum / num_runs;

//         cout << "MIN_THREAD_SIZE = " << min_thread_size << ", Avg runtime: " << avg_time << " ms\n";

//     }

//     return 0;
// }

int main() {
    // Use high-quality random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);

    vector<int> sizes = {1000, 10000, 100000, 1000000, 10000000, 100000000}; // Large array sizes for testing
    vector<int> thread_sizes = {10000, 50000, 100000, 250000, 500000, 1000000}; // Different MIN_THREAD_SIZE values
    int num_runs = 10; // Number of times to run each test

    for (int n : sizes) {
        vector<double> arr(n);
        for (int i = 0; i < n; i++) {
            arr[i] = dist(gen);
        }

        cout << "Finding best MIN_THREAD_SIZE for n = " << n << " elements...\n";

        double best_time = 1e9;
        int best_thread_size = thread_sizes[0];

        for (int MIN_THREAD_SIZE : thread_sizes) {
            vector<double> runtimes;
            vector<double> aux(n); // Preallocate auxiliary array

            for (int run = 0; run < num_runs; run++) {
                vector<double> temp(n);
                memcpy(temp.data(), arr.data(), n * sizeof(double)); // Faster copying

                auto start = high_resolution_clock::now();
                mergeSort(temp, aux, 0, n - 1, MIN_THREAD_SIZE);
                auto stop = high_resolution_clock::now();

                double duration = duration_cast<milliseconds>(stop - start).count();
                runtimes.push_back(duration);
            }

            // Compute average runtime
            double sum = 0;
            for (double time : runtimes) {
                sum += time;
            }
            double avg_time = sum / num_runs;

            cout << "MIN_THREAD_SIZE = " << MIN_THREAD_SIZE << ", Avg runtime: " << avg_time << " ms\n";

            if (avg_time < best_time) {
                best_time = avg_time;
                best_thread_size = MIN_THREAD_SIZE;
            }
        }
        vector<double> runtimes;
        vector<double> aux(n); // Preallocate auxiliary array

        for (int run = 0; run < num_runs; run++) {
            vector<double> temp(n);
            memcpy(temp.data(), arr.data(), n * sizeof(double)); // Faster copying

            auto start = high_resolution_clock::now();
            mergeSort(temp, aux, 0, n - 1, n/(4*MAX_THREADS));
            auto stop = high_resolution_clock::now();

            double duration = duration_cast<milliseconds>(stop - start).count();
            runtimes.push_back(duration);
        }

        // Compute average runtime
        double sum = 0;
        for (double time : runtimes) {
            sum += time;
        }
        double avg_time = sum / num_runs;

        cout << "MIN_THREAD_SIZE = " << n/(4*MAX_THREADS) << ", Avg runtime: " << avg_time << " ms\n";

        if (avg_time < best_time) {
            best_time = avg_time;
            best_thread_size = n/(4*MAX_THREADS);
        }

        cout << "Best MIN_THREAD_SIZE for n = " << n << " is " << best_thread_size << " with avg time: " << best_time << " ms\n\n";
    }

    return 0;
}