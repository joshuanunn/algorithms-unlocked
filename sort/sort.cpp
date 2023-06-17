#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
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
 * @return  answer  (last) index of A for which x found; -1 for not found
 * */
void selection_sort(std::vector<int> &A, int n) {
    int smallest;

    for(int i=0; i<n; i++) {
        smallest = A[i];
        for (int j=i+1; j<n; j++) {
            if (A[j] < smallest) {
                smallest = A[j];
            }
        }
        A[i] = smallest;
    }
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

        // Use values in the sorted array to prevent the compiler optimising away ops
        dummy_val += arr[i % array_size];
    }

    std::cout << "Selection sort: " << ((float)dt / (1e6 * repeats)) << " s (average per op)" <<std::endl;

    // Dump final accumulated value to prevent compiler optimising away ops
    std::cout << dummy_val << std::endl;

    exit(EXIT_SUCCESS);
}
