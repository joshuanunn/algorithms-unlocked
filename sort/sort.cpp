#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <climits>
#include <numeric>
#include <vector>

/*
 * @brief Parse argument to extract user array size
 *
 * @param[in]  param  argv element corresponding to array size
 * @return  (int)array_size  parsed size of array, casted to int
 * */
int get_array_size(char* param) {
    char *endptr;
    long array_size;

    errno = 0;
    array_size = std::strtol(param, &endptr, 10);

    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == param) {
        std::cerr << "could not parse array_size as an integer" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (array_size > 100000000) {
        std::cerr << "array_size parameter must be <= 100000000" << std::endl;
        exit(EXIT_FAILURE);
    }

    return (int)array_size;
}

/*
 * @brief Parse argument to extract user number of repeats
 *
 * @param[in]  param  argv element corresponding to array size
 * @return  (int)repeat_count  parsed repeat count, casted to int
 * */
int get_repeat_count(char* param) {
    char *endptr;
    long repeat_count;

    errno = 0;
    repeat_count = std::strtol(param, &endptr, 10);

    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == param) {
        std::cerr << "could not parse repeat_count as an integer" << std::endl;
        exit(EXIT_FAILURE);
    }

    return (int)repeat_count;
}

/*
 * @brief Selection sort implementation
 *
 * @param[in]  A  reference to vector of ints to be sorted
 * @param[in]  n  size of vector
 * */
void selection_sort(std::vector<int> &A, int n) {
    int smallest;

    for(int i=0; i<n; i++) {
        smallest = i;
        for (int j=i+1; j<n; j++) {
            if (A[j] < A[smallest]) {
                smallest = j;
            }
        }
        A[i] = A[smallest];
    }
}

/*
 * @brief Insertion sort implementation
 *
 * @param[in]  A  reference to vector of ints to be sorted
 * @param[in]  n  size of vector
 * */
void insertion_sort(std::vector<int> &A, int n) {
    int key;
    int j;

    for(int i=1; i<n; i++) {
        key = A[i];
        j = i - 1;
        while (j>=0 && A[j]>key) {
            A[j+1] = A[j];
            j--;
        }
        A[j+1] = key;
    }
}

/*
 * @brief Supporting function for merge sort implementation
 *
 * @param[in]  A  reference to vector of ints, where subarrays
 *                A[p..q] and A[q+1..r] are assumed sorted
 * @param[in]  p  index of subarrays of A (A[p..q] and A[q+1..r])
 * @param[in]  q  index of subarrays of A (A[p..q] and A[q+1..r])
 * @param[in]  r  index of subarrays of A (A[p..q] and A[q+1..r])
 * */
void merge(std::vector<int> &A, int p, int q, int r) {
    // Calculate sizes of each subarray A[p..q] and A[q+1..r]
    int n1 = q - p + 1;
    int n2 = r - q;

    // Create a copy of subarray B = A[p..q], terminated with MAX_INT
    std::vector<int> B(n1+1); // Allocate extra element for MAX_INT
    for(int i=p; i<=q; i++) {
        B[i - p] = A[i];
    }
    B[n1] = INT_MAX;

    // Create a copy of subarray C = A[q+1..r], terminated with MAX_INT
    std::vector<int> C(n2+1); // Allocate extra element for MAX_INT
    for(int i=q+1; i<=r; i++) {
        C[i - (q + 1)] = A[i];
    }
    C[n2] = INT_MAX;

    // Merge and sort subarrays B and C into A[p..r]
    int i=0;
    int j=0;

    for(int k=p; k<=r; k++) {
        if (B[i] <= C[j]) {
            A[k] = B[i++];
        } else {
            A[k] = C[j++];
        }
    }
}

/*
 * @brief Merge sort implementation
 *
 * @param[in]  A  reference to vector of ints to be sorted
 * @param[in]  p  start index of subarray of A
 * @param[in]  r  end index of subarray of A
 * */
void merge_sort(std::vector<int> &A, int p, int r) {
    // Trivial basecase for one or zero elements, as they are "sorted"
    if (p >= r) return;

    // Calculate midpoint of array range
    int q = (p + r) / 2;

    // Recursively sort each subarray A[p..q] and A[q+1..r]
    merge_sort(A, p, q);
    merge_sort(A, q+1, r);

    // Finally, merge sorted subarrays
    merge(A, p, q, r);
}

/*
 * @brief Verify elements of a vector of ints are monotonically increasing
 *
 * @param[in]  A  reference to vector of ints to be checked
 * @param[in]  n  size of vector
 * */
bool verify_sorted(std::vector<int> const &A, int n) {
    int previous_value = A[0];

    for (int i=1; i<n; i++) {
        if (A[i] < previous_value) {
            return false;
        }
        previous_value = A[i];
    }
    return true;
}

int main(int argc, char* argv[]) {
    // Check correct usage (e.g. 'sort 100000 5')
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [array_size] [repeat_count]" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::chrono::time_point<std::chrono::steady_clock> t1, t2;
    long dt;
    int dummy_val = 0; // Use for accumulation to prevent compiler optimising away ops

    int array_size = get_array_size(argv[1]);
    int repeats = get_repeat_count(argv[2]);
    std::vector<int> arr(array_size);

    // Selection sort
    dt = 0;
    for (int i=0; i<repeats; i++) {
        // Create a vector with random integers (seeded)
        std::srand(42);
        std::generate(arr.begin(), arr.end(), std::rand);

        // Sort array in place (only time the sort)
        t1 = std::chrono::steady_clock::now();
        selection_sort(arr, array_size);
        t2 = std::chrono::steady_clock::now();

        // Accumulate measurement time
        dt += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();

        // Check the sort actually worked
        if (!verify_sorted(arr, array_size)) {
            std::cerr << "Selection sort failure!" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Use values in the sorted array to prevent the compiler optimising away ops
        dummy_val += arr[i % array_size];
    }

    std::cout << "Selection sort: " << ((float)dt / (1e6 * repeats)) << " s (average per op)" <<std::endl;


    // Insertion sort
    dt = 0;
    for (int i=0; i<repeats; i++) {
        // Create a vector with random integers (seeded)
        std::srand(42);
        std::generate(arr.begin(), arr.end(), std::rand);

        // Sort array in place (only time the sort)
        t1 = std::chrono::steady_clock::now();
        insertion_sort(arr, array_size);
        t2 = std::chrono::steady_clock::now();

        // Accumulate measurement time
        dt += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();

        // Check the sort actually worked
        if (!verify_sorted(arr, array_size)) {
            std::cerr << "Insertion sort failure!" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Use values in the sorted array to prevent the compiler optimising away ops
        dummy_val += arr[i % array_size];
    }

    std::cout << "Insertion sort: " << ((float)dt / (1e6 * repeats)) << " s (average per op)" <<std::endl;


    // Merge sort
    dt = 0;
    for (int i=0; i<repeats; i++) {
        // Create a vector with random integers (seeded)
        std::srand(42);
        std::generate(arr.begin(), arr.end(), std::rand);

        // Sort array in place (only time the sort)
        t1 = std::chrono::steady_clock::now();
        merge_sort(arr, 0, array_size-1);
        t2 = std::chrono::steady_clock::now();

        // Accumulate measurement time
        dt += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();

        // Check the sort actually worked
        if (!verify_sorted(arr, array_size)) {
            std::cerr << "Merge sort failure!" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Use values in the sorted array to prevent the compiler optimising away ops
        dummy_val += arr[i % array_size];
    }

    std::cout << "Merge sort: " << ((float)dt / (1e6 * repeats)) << " s (average per op)" <<std::endl;


    // Dump final accumulated value to prevent compiler optimising away ops
    std::cout << dummy_val << std::endl;

    exit(EXIT_SUCCESS);
}
