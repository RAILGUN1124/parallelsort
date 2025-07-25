#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
#include <cstring>  // For std::memcpy

using namespace std;
using namespace std::chrono;
void insertionSort(vector<double>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        double key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}
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

// Optimized Merge Sort with preallocated auxiliary array
void mergeSort(vector<double>& arr, vector<double>& aux, int left, int right, int k) {
    if (right - left + 1 <= k) {
        // sort(arr.begin() + left, arr.begin() + right + 1);
        //insetionsort is faster than built in sort
        insertionSort(arr, left, right);
        return;
    }
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, aux, left, mid, k);
        mergeSort(arr, aux, mid + 1, right, k);
        merge(arr, aux, left, mid, right);
    }
}

int main() {
    // Use high-quality random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);

    vector<int> sizes = {1000, 10000, 100000, 1000000, 10000000, 100000000}; // Array sizes
    vector<int> k_values = {5, 10, 20, 30, 50, 100};
    int num_runs = 10; // Number of times to run each test

    for (int n : sizes) {
        vector<double> arr(n);
        arr.reserve(n);  // Preallocate memory

        for (int i = 0; i < n; i++) {
            arr[i] = dist(gen);  // Faster random number generation
        }

        cout << "Optimizing k for n = " << n << " elements...\n";

        double best_time = 1e9;
        int best_k = k_values[0];

        vector<double> runtimes;
        vector<double> aux(n); // Preallocate auxiliary array
        for(int k : k_values) {
            vector<double> runtimes;
            for (int run = 0; run < num_runs; run++) {
                vector<double> temp(n);
                memcpy(temp.data(), arr.data(), n * sizeof(double)); // Faster copying

                auto start = high_resolution_clock::now();
                mergeSort(temp, aux, 0, n - 1, k);
                auto stop = high_resolution_clock::now();

                double duration = duration_cast<nanoseconds>(stop - start).count() / 1e6; // Convert to ms
                if (!is_sorted(temp.begin(), temp.end())) {
                    cerr << "Sorting failed!" << endl;
                    exit(1);
                }
                runtimes.push_back(duration);

            }

            // Compute average runtime (excluding the first run)
            double sum = 0;
            for (size_t i = 1; i < runtimes.size(); i++) {
                sum += runtimes[i];
            }
            double avg_time = sum / (num_runs - 1);

            cout << "k = " << k << ", Avg runtime: " << avg_time << " ms\n";
            if (avg_time < best_time) {
                best_time = avg_time;
                best_k = k;
            }
        }
        cout << "Best k for n = " << n << " is " << best_k << " with avg time: " << best_time << " ms\n\n";
    }

    return 0;
}
// int main() {
//     // Use high-quality random number generator
//     random_device rd;
//     mt19937 gen(rd());
//     uniform_real_distribution<double> dist(0.0, 1.0);

//     vector<int> sizes = {1000, 10000, 100000, 1000000, 10000000, 100000000}; // Array sizes
//     vector<int> k_values = {5, 10, 20, 30, 50, 100};
//     int num_runs = 10; // Number of times to run each test

//     for (int n : sizes) {
//         vector<double> arr(n);
//         arr.reserve(n);  // Preallocate memory

//         for (int i = 0; i < n; i++) {
//             arr[i] = dist(gen);  // Faster random number generation
//         }

//         cout << "Sorting " << n << " elements..." << endl;

//         vector<double> runtimes;
//         vector<double> aux(n); // Preallocate auxiliary array
//         for (int run = 0; run < num_runs; run++) {
//             vector<double> temp(n);
//             memcpy(temp.data(), arr.data(), n * sizeof(double)); // Faster copying

//             auto start = high_resolution_clock::now();
//             mergeSort(temp, aux, 0, n - 1, 50);
//             auto stop = high_resolution_clock::now();

//             double duration = duration_cast<nanoseconds>(stop - start).count() / 1e6; // Convert to ms
//             runtimes.push_back(duration);

//         }

//         // Compute average runtime (excluding the first run)
//         double sum = 0;
//         for (size_t i = 1; i < runtimes.size(); i++) {
//             sum += runtimes[i];
//         }
//         double avg_time = sum / (num_runs - 1);
//         cout << "Average runtime for n = " << n << ": " << avg_time << " ms\n" << endl;
//     }

//     return 0;
// }
