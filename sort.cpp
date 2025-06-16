#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
#include <cstring>  // For std::memcpy

using namespace std;
using namespace std::chrono;

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
            sort(temp.begin(), temp.end());
            auto stop = high_resolution_clock::now();

            double duration = duration_cast<nanoseconds>(stop - start).count() / 1e6; // Convert to ms
            runtimes.push_back(duration);
        }
        double sum = 0;
        for (size_t i = 1; i < runtimes.size(); i++) {
            sum += runtimes[i];
        }
        double avg_time = sum / (num_runs - 1);
        cout << "Average runtime for n = " << n << ": " << avg_time << " ms\n" << endl;
    }

    return 0;
}




