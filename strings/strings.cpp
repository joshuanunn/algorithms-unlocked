#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <climits>
#include <numeric>
#include <string>
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

std::vector<int> compute_lcs_table(std::string X, std::string Y) {
    int m = X.size();
    int n = Y.size();

    // Use a 1-dim vector l of length [m+1 rows x n+1 cols] to represent the 2-dim LCS table and deal with strides manually
    std::vector<int> l ((m+1) * (n+1));

    // Zero left column
    for (int i=0; i<m+1; i++) {
        l[i*(n+1)] = 0;
    }

    // Zero top row
    for (int j=0; j<n+1; j++) {
        l[j] = 0;
    }

    for (int i=1; i<m+1; i++) {
        for (int j=1; j<n+1; j++) {
            if (X[i-1] == Y[j-1]) {
                // l[i,j] = l[i-1, j-1] + 1
                l[j + i*(n+1)] = l[(j-1) + (i-1)*(n+1)] + 1;
            } else {
                l[j + i*(n+1)] = std::max(l[j + (i-1)*(n+1)], l[(j-1) + i*(n+1)]);
            }
        }
    }
    return l;
}

void print_lcs_table(std::vector<int> l, std::string X, std::string Y) {
    int m = X.size();
    int n = Y.size();

    for (int i=0; i<m+1; i++) {
        for (int j=0; j<n+1; j++) {
            std::cout << std::setw(3) << std::setfill(' ') << l[j + i*(n+1)];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

std::string assemble_lcs(std::string const &X, std::string const &Y, std::vector<int> const &l, int i, int j) {
    int height = X.size() + 1;
    int width = Y.size() + 1;

    int element = l[j + i*width];          // l[i,j]
    int element_i_mj = l[(j-1) + i*width]; // l[i,j-1]
    int element_mi_j = l[j + (i-1)*width]; // l[i-1,j]

    if (element == 0) {
        return "";
    } else if (X[i-1] == Y[j-1]) {
        return assemble_lcs(X, Y, l, i-1, j-1) + X[i-1];
    } else if (element_i_mj > element_mi_j) {
        return assemble_lcs(X, Y, l, i, j-1);
    } else {
        return assemble_lcs(X, Y, l, i-1, j);
    }
}

int main(int argc, char* argv[]) {
    // Check correct usage (e.g. 'strings 100000 5')
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
    std::string X = "CATCGA";
    std::string Y = "GTACCGTCA";

    dt = 0;
    for (int i=0; i<repeats; i++) {
        // Sort array in place (only time the sort)
        t1 = std::chrono::steady_clock::now();
        auto lcs_table = compute_lcs_table(X, Y);
        auto lcs = assemble_lcs(X, Y, lcs_table, X.size(), Y.size());
        t2 = std::chrono::steady_clock::now();

        // Accumulate measurement time
        dt += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();

        print_lcs_table(lcs_table, X, Y);
        std::cout << lcs << std::endl;

        // Use values in the sorted array to prevent the compiler optimising away ops
        dummy_val += arr[i % array_size];
    }

    std::cout << "Longest common subsequence: " << ((float)dt / (1e6 * repeats)) << " s (average per op)" << std::endl;

    // Dump final accumulated value to prevent compiler optimising away ops
    std::cout << dummy_val << std::endl;

    exit(EXIT_SUCCESS);
}
