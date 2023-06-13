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
 * @brief Parse argument to extract user integer search value
 *
 * @param[in]  param  argv element corresponding to search value
 * @return  (int)search_value  parsed search_value, casted to int
 * */
int get_search_value(char* param) {
    char *endptr;
    long search_value;

    errno = 0;
    search_value = std::strtol(param, &endptr, 10);

    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == param) {
        std::cerr << "could not parse search_value as an integer" << std::endl;
        exit(EXIT_FAILURE);
    }

    if ((search_value < INT32_MIN) || (search_value > INT32_MAX)) {
        std::cerr << "error: " << INT32_MIN << " <= search_value <= " << INT32_MAX << std::endl;
        exit(EXIT_FAILURE);
    }

    return (int)search_value;
}

/*
 * @brief Linear search implementation
 *
 * @param[in]  A  vector of ints to be searched
 * @param[in]  n  size of vector
 * @param[in]  x  search value to be found in A
 * @return  answer  (last) index of A for which x found; -1 for not found
 * */
int linear_search(std::vector<int> A, int n, int x) {
    // default return value of not found (-1)
    int answer = {-1};

    for(int i=0; i<n; i++) {
        if (A[i] == x) {
            answer = i;
        }
    }

    return answer;
}

/*
 * @brief Better linear search implementation
 *
 * As for linear search, but return early if search value found
 * @param[in]  A  vector of ints to be searched
 * @param[in]  n  size of vector
 * @param[in]  x  search value to be found in A
 * @return  answer  index of A for which x found; -1 for not found
 * */
int better_linear_search(std::vector<int> A, int n, int x) {
    for(int i=0; i<n; i++) {
        if (A[i] == x) {
            return i;
        }
    }

    // x not found
    return -1;
}

/*
 * @brief Sentinel linear search implementation
 *
 * Insert the search value into the last index of the vector, in
 * order to avoid the need for loop bounds checking (as the last
 * element is guaranteed to contain the search term if not found
 * earlier.
 * @param[in]  A  vector of ints to be searched
 * @param[in]  n  size of vector
 * @param[in]  x  search value to be found in A
 * @return  (index)  index of A for which x found; -1 for not found
 * */
int sentinel_linear_search(std::vector<int> A, int n, int x) {
    int last = A[n-1];
    A[n-1] = x;

    int i = {0};
    while (A[i] != x) {i++;}

    // restore last element of A
    A[n-1] = last;

    if ((i < n-1) || A[n-1] == x) {
        return i;
    }

    // x not found
    return -1;
}

/*
 * @brief Recursive linear search implementation
 *
 * A recursive version of better linear search, where we return
 * early if search value found
 * @param[in]  A  vector of ints to be searched
 * @param[in]  n  size of vector
 * @param[in]  i  current search index
 * @param[in]  x  search value to be found in A
 * @return  (index)  index of A for which x found; -1 for not found
 * */
int recursive_linear_search(std::vector<int> A, int n, int i, int x) {
    // Base recursive case
    if (i > n-1) {
        return -1;
    }

    // Return index if x found
    if (A[i] == x) {
        return i;
    }

    // Advance search index by 1
    return recursive_linear_search(std::move(A), n, i+1, x);
}

/*
 * @brief Recursive binary search implementation
 *
 * @param[in]  A  vector of ints to be searched (must be sorted in advance)
 * @param[in]  n  size of vector
 * @param[in]  x  search value to be found in A
 * @return  (index)  index of A for which x found; -1 for not found
 * */
int binary_search(std::vector<int> A, int n, int x) {
    // Set initial search index bounds to array bounds
    int p = 0;
    int r = n - 1;

    while (p <= r) {
        // Calculate midpoint of index range [p..r]
        int q = (p + r) / 2;

        // Return index if x found
        if (A[q] == x) {
            return q;
        }

        // If element at index q is > x, then set search index bounds to lower half else higher half
        if (A[q] > x) {
            r = q - 1;
        } else {
            p = q + 1;
        }
    }
    // x not found
    return -1;
}

/*
 * @brief Recursive binary search implementation
 *
 * @param[in]  A  vector of ints to be searched (must be sorted in advance)
 * @param[in]  p  lower index search bound (inclusive)
 * @param[in]  r  upper index search bound (inclusive)
 * @param[in]  x  search value to be found in A
 * @return  (index)  index of A for which x found; -1 for not found
 * */
int recursive_binary_search(std::vector<int> A, int p, int r, int x) {
    // Base recursive case
    if (p > r) {
        return -1;
    }

    // Calculate midpoint of index range [p..r]
    int q = (p + r) / 2;

    // Return index if x found
    if (A[q] == x) {
        return q;
    }

    // If element at index q is > x, then recurse to lower half else higher half
    if (A[q] > x) {
        return recursive_binary_search(std::move(A), p, q-1, x);
    } else {
        return recursive_binary_search(std::move(A), q+1, r, x);
    }
}

int main(int argc, char* argv[]) {
    // Check correct usage (e.g. 'search 100000 50000')
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [array_size] [search_value]" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::chrono::time_point<std::chrono::steady_clock> t1, t2;
    long dt;
    int dummy_val = 0; // Use for accumulation to prevent compiler optimising away ops
    int repeats = 10e4;

    int array_size = get_array_size(argv[1]);
    int search_value = get_search_value(argv[2]);

    // Create a vector with sequential integers with random start
    std::vector<int> arr(array_size);
    std::iota(arr.begin(), arr.end(), 0);

    // Linear search
    t1 = std::chrono::steady_clock::now();
    for (int i=0; i<repeats; i++) {
        dummy_val += linear_search(arr, array_size, search_value);
    }
    t2 = std::chrono::steady_clock::now();

    dt = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    std::cout << "Linear search: " << ((float)dt / (10e5 * repeats)) << " s (average per op)" <<std::endl;


    // Better linear search
    t1 = std::chrono::steady_clock::now();
    for (int i=0; i<repeats; i++) {
        dummy_val += better_linear_search(arr, array_size, search_value);
    }
    t2 = std::chrono::steady_clock::now();

    dt = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    std::cout << "Better linear search: " << ((float)dt / (10e5 * repeats)) << " s (average per op)" <<std::endl;


    // Sentinel search
    t1 = std::chrono::steady_clock::now();
    for (int i=0; i<repeats; i++) {
        dummy_val += sentinel_linear_search(arr, array_size, search_value);
    }
    t2 = std::chrono::steady_clock::now();

    dt = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    std::cout << "Sentinel linear search: " << ((float)dt / (10e5 * repeats)) << " s (average per op)" <<std::endl;


    // Recursive linear search
    t1 = std::chrono::steady_clock::now();
    for (int i=0; i<repeats; i++) {
        dummy_val += recursive_linear_search(arr,array_size,0,search_value);
    }
    t2 = std::chrono::steady_clock::now();

    dt = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    std::cout << "Recursive linear search: " << ((float)dt / (10e5 * repeats)) << " s (average per op)" <<std::endl;


    // Binary search
    t1 = std::chrono::steady_clock::now();
    for (int i=0; i<repeats; i++) {
        dummy_val += binary_search(arr,array_size,search_value);
    }
    t2 = std::chrono::steady_clock::now();

    dt = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    std::cout << "Binary search: " << ((float)dt / (10e5 * repeats)) << " s (average per op)" <<std::endl;


    // Recursive binary search
    t1 = std::chrono::steady_clock::now();
    for (int i=0; i<repeats; i++) {
        dummy_val += recursive_binary_search(arr,0,array_size-1,search_value);
    }
    t2 = std::chrono::steady_clock::now();

    dt = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    std::cout << "Recursive binary search: " << ((float)dt / (10e5 * repeats)) << " s (average per op)" <<std::endl;

    // Dump final accumulated value to prevent compiler optimising away ops
    std::cout << dummy_val << std::endl;

    exit(EXIT_SUCCESS);
}
