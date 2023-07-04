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
int get_array_size(char *param) {
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

    return (int) array_size;
}

/*
 * @brief Parse argument to extract user number of repeats
 *
 * @param[in]  param  argv element corresponding to array size
 * @return  (int)repeat_count  parsed repeat count, casted to int
 * */
int get_repeat_count(char *param) {
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

    return (int) repeat_count;
}

// Class to encapsulate an LCS table computed for two strings
struct LCSTable {
    int height;
    int width;
    std::string X;
    std::string Y;
    std::vector<int> table;

    LCSTable(std::string const &str_x, std::string const &str_y);

    int coord(int i, int j) const;

    void compute_lcs_table();
};

LCSTable::LCSTable(std::string const &str_x, std::string const &str_y) {
    // Maintain an internal copy of the original strings used to calculate the LCS
    X = str_x;
    Y = str_y;

    // Height and width of LCS table include the first zeroed row and column
    height = X.size() + 1;
    width = Y.size() + 1;

    // Initialise a 1-dim vector of length [height x width] to represent the 2-dim LCS table
    table = std::vector<int>(height * width);

    // Populate LCS table for the provided strings
    LCSTable::compute_lcs_table();
}

// Convenience method for converting between 2-dim (row,column) index and the internal 1-dim vector index
int LCSTable::coord(int i, int j) const {
    return j + i * width;
}

/*
 * @brief LCSTable method to construct the LCS table
 *
 * The internal 1-dim "table" vector represents the 2-dim LCS table[0..m,0..n], for two strings X of length m and Y of
 * length n. The value of table[m,n] is the length of a longest common subsequence of X and Y.
 *
 * */
void LCSTable::compute_lcs_table() {
    // Create zero left column
    for (int i = 0; i < height; i++) {
        table[coord(i, 0)] = 0;
    }

    // Create zero top row
    for (int j = 0; j < width; j++) {
        table[coord(0, j)] = 0;
    }

    // Compute values for inner table
    for (int i = 1; i < height; i++) {
        for (int j = 1; j < width; j++) {
            if (X[i - 1] == Y[j - 1]) {
                table[coord(i, j)] = table[coord(i - 1, j - 1)] + 1;
            } else {
                table[coord(i, j)] = std::max(table[coord(i - 1, j)], table[coord(i, j - 1)]);
            }
        }
    }
}

// Overload << operator to allow natural printing of LCS table
std::ostream &operator<<(std::ostream &out, LCSTable const &t) {
    for (int i = 0; i < t.height; i++) {
        for (int j = 0; j < t.width; j++) {
            out << std::setw(3) << std::setfill(' ') << t.table[t.coord(i, j)];
        }
        out << std::endl;
    }
    return out;
}

/*
 * @brief Recursively assemble an LCS string from a pre-computed LCS table
 *
 * @param[in]  t  instance of an LCSTable precomputed from two strings
 * @param[in]  i  row index into t (and the original X, Y strings)
 * @param[in]  j  column index into t (and the original X, Y strings)
 * @return  string representing the LCS, for the given i,j indices
 * */
std::string assemble_lcs(LCSTable const &t, int i, int j) {
    // Base case, where no common substring -- return empty string
    if (t.table[t.coord(i, j)] == 0) {
        return "";
    // Common substring, where char X[i] and Y[j] match -- recursively return substring and append matching char
    } else if (t.X[i - 1] == t.Y[j - 1]) {
        return assemble_lcs(t, i - 1, j - 1) + t.X[i - 1];
    // Common substring, where char X[i] and Y[j] do not match -- recursively work back along row
    } else if (t.table[t.coord(i, j - 1)] > t.table[t.coord(i - 1, j)]) {
        return assemble_lcs(t, i, j - 1);
    // Common substring, where char X[i] and Y[j] do not match -- recursively work back up column
    } else {
        return assemble_lcs(t, i - 1, j);
    }
}

int main(int argc, char *argv[]) {
    // Check correct usage (e.g. 'strings 100000 5')
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [array_size] [repeat_count]" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::chrono::time_point <std::chrono::steady_clock> t1, t2;
    long dt;
    int dummy_val = 0; // Use for accumulation to prevent compiler optimising away ops

    int array_size = get_array_size(argv[1]);
    int repeats = get_repeat_count(argv[2]);
    std::vector<int> arr(array_size);

    // Selection sort
    std::string X = "CATCGA";
    std::string Y = "GTACCGTCA";

    dt = 0;
    for (int i = 0; i < repeats; i++) {
        // Sort array in place (only time the sort)
        t1 = std::chrono::steady_clock::now();
        auto lcs_table = LCSTable(X, Y);
        auto lcs = assemble_lcs(lcs_table, X.size(), Y.size());
        t2 = std::chrono::steady_clock::now();

        // Accumulate measurement time
        dt += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

        std::cout << lcs_table << std::endl;
        std::cout << lcs << std::endl;

        // Use values in the sorted array to prevent the compiler optimising away ops
        dummy_val += arr[i % array_size];
    }

    std::cout << "Longest common subsequence: " << ((float) dt / (1e6 * repeats)) << " s (average per op)" << std::endl;

    // Dump final accumulated value to prevent compiler optimising away ops
    std::cout << dummy_val << std::endl;

    exit(EXIT_SUCCESS);
}
