#include <chrono>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "include/randstring.hpp"

/*
 * @brief Parse argument to extract user string length
 *
 * @param[in]  param  argv element corresponding to string length
 * @return  (int)string_length  parsed size of array, casted to int
 * */
int get_string_length(char *param) {
    char *endptr;
    long string_length;

    errno = 0;
    string_length = std::strtol(param, &endptr, 10);

    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == param) {
        std::cerr << "could not parse string_length as an integer" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (string_length > 40000) {
        std::cerr << "string_length parameter must be <= 40000" << std::endl;
        exit(EXIT_FAILURE);
    }

    return (int) string_length;
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

/*
 * @brief Class to encapsulate an LCS table computed for two strings.
 *
 * Example from the book for the strings X and Y give the LCS
 * table below, from which the LCS of "CTCA" can be derived.
 *
 *   X = "CATCGA"
 *   Y = "GTACCGTCA"
 *
 *   table = {0, 0, 0, ..., 3, 3, 3, 4}
 *
 * LCS table form:
 *
 *          G  T  A  C  C  G  T  C  A
 *       0  0  0  0  0  0  0  0  0  0
 *    C  0  0  0  0  1  1  1  1  1  1
 *    A  0  0  0  1  1  1  1  1  1  2
 *    T  0  0  1  1  1  1  1  2  2  2
 *    C  0  0  1  1  2  2  2  2  3  3
 *    G  0  1  1  1  2  2  3  3  3  3
 *    A  0  1  1  2  2  2  3  3  3  4
 * */
struct LCSTable {
    int height;
    int width;
    std::string X;
    std::string Y;
    std::vector<int> table;

    // Constructor makes a copy of the input strings, computes the height and width, and initialises the table vector.
    LCSTable(std::string const &str_x, std::string const &str_y);

    // Convert between 2-dim (row,column) index and the internal 1-dim vector index.
    int coord(int i, int j) const;

    // Construct the LCS table. The 1-dim "table" vector represents the 2-dim LCS table[0..m,0..n], for two strings X of
    // length m and Y of length n. The value of table[m,n] is the length of a longest common subsequence of X and Y.
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

int LCSTable::coord(int i, int j) const {
    return j + i * width;
}

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
    // Check correct usage (e.g. 'strings 1000 5')
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [string_length] [repeat_count]" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::chrono::time_point <std::chrono::steady_clock> t1, t2, t3;
    long dt1, dt2;
    std::string lcs, X, Y;
    int dummy_val = 0; // Use for accumulation to prevent compiler optimising away ops

    int string_length = get_string_length(argv[1]);
    int repeats = get_repeat_count(argv[2]);

    // Example strings X and Y from the book
    //X = "CATCGA";
    //Y = "GTACCGTCA";

    // Create random alphanumeric strings of user specified length
    X = generate_random_alphanumeric_string(string_length);
    Y = generate_random_alphanumeric_string(string_length);

    dt1 = 0;
    dt2 = 0;

    for (int i = 0; i < repeats; i++) {

        t1 = std::chrono::steady_clock::now();

        // Construct LCS table
        auto lcs_table = LCSTable(X, Y);
        t2 = std::chrono::steady_clock::now();

        // Calculate LCS string
        lcs = assemble_lcs(lcs_table, X.size(), Y.size());
        t3 = std::chrono::steady_clock::now();

        // Accumulate measurement time
        dt1 += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        dt2 += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();

        // Print intermediate LCS table for debugging
        //std::cout << lcs_table << std::endl;

        // Use values in the sorted array to prevent the compiler optimising away ops
        dummy_val += lcs[(100 * i) % string_length];
    }

    // Print final LCS string
    //std::cout << "Longest common subsequence: " << lcs << std::endl;
    std::cout << "Time to compute LCS table: " << ((float) dt1 / (1e6 * repeats)) << " s (average per op)" << std::endl;
    std::cout << "Time to compute LCS string: " << ((float) dt2 / (1e6 * repeats)) << " s (average per op)" << std::endl;

    // Dump final accumulated value to prevent compiler optimising away ops
    std::cout << dummy_val << std::endl;

    exit(EXIT_SUCCESS);
}
