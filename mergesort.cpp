#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
#include <cstring>  // For std::memcpy

using namespace std;
using namespace std::chrono;

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
void mergeSort(vector<double>& arr, vector<double>& aux, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, aux, left, mid);
        mergeSort(arr, aux, mid + 1, right);
        merge(arr, aux, left, mid, right);
    }
}

int main() {
    // Use high-quality random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);

    vector<int> sizes = {1000, 10000, 100000, 1000000, 10000000, 100000000}; // Array sizes
    int num_runs = 10; // Number of times to run each test

    for (int n : sizes) {
        vector<double> arr(n);
        arr.reserve(n);  // Preallocate memory

        for (int i = 0; i < n; i++) {
            arr[i] = dist(gen);  // Faster random number generation
        }

        cout << "Sorting " << n << " elements..." << endl;

        vector<double> runtimes;
        vector<double> aux(n); // Preallocate auxiliary array

        for (int run = 0; run < num_runs; run++) {
            vector<double> temp(n);
            memcpy(temp.data(), arr.data(), n * sizeof(double)); // Faster copying

            auto start = high_resolution_clock::now();
            mergeSort(temp, aux, 0, n - 1);
            auto stop = high_resolution_clock::now();

            double duration = duration_cast<nanoseconds>(stop - start).count() / 1e6; // Convert to ms
            runtimes.push_back(duration);

           
        }

        // Compute average runtime (excluding the first run)
        double sum = 0;
        for (size_t i = 1; i < runtimes.size(); i++) {
            sum += runtimes[i];
        }
        double avg_time = sum / (num_runs - 1);

        cout << "Average runtime for n = " << n << ": " << avg_time << " ms\n" << endl;
    }

    return 0;
}




