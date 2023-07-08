#include <algorithm>
#include <array>
#include <chrono>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "strings.hpp"

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

std::ostream &operator<<(std::ostream &out, LCSTable const &t) {
    for (int i = 0; i < t.height; i++) {
        for (int j = 0; j < t.width; j++) {
            out << std::setw(3) << std::setfill(' ') << t.table[t.coord(i, j)];
        }
        out << std::endl;
    }
    return out;
}

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

// Taken from: https://stackoverflow.com/a/444614
template <typename T = std::mt19937>
T random_generator() {
    auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
    auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
    auto seed = std::array<std::seed_seq::result_type, seed_len>();
    auto dev = std::random_device();
    std::generate_n(begin(seed), seed_len, std::ref(dev));
    auto seed_seq = std::seed_seq(begin(seed), end(seed));
    return T{seed_seq};
}

// Taken from: https://stackoverflow.com/a/444614
std::string generate_random_alphanumeric_string(std::size_t len) {
    static constexpr auto chars =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    thread_local auto rng = random_generator<>();
    auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
    auto result = std::string(len, '\0');
    std::generate_n(begin(result), len, [&]() { return chars[dist(rng)]; });
    return result;
}

int main(int argc, char *argv[]) {
    // Check correct usage (e.g. 'strings 1000 5')
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [string_length] [repeat_count]" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::chrono::time_point <std::chrono::steady_clock> t1, t2, t3;
    long dt1, dt2;
    std::string lcs;
    int dummy_val = 0; // Use for accumulation to prevent compiler optimising away ops

    int string_length = get_string_length(argv[1]);
    int repeats = get_repeat_count(argv[2]);

    // Example strings X and Y from the book
    //std::string X = "CATCGA";
    //std::string Y = "GTACCGTCA";

    // Create random alphanumeric strings of user specified length
    std::string X = generate_random_alphanumeric_string(string_length);
    std::string Y = generate_random_alphanumeric_string(string_length);

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
        dummy_val += lcs[i % string_length];
    }

    // Print final LCS string
    std::cout << "Longest common subsequence: " << lcs << std::endl;
    std::cout << "Time to compute LCS table: " << ((float) dt1 / (1e6 * repeats)) << " s (average per op)" << std::endl;
    std::cout << "Time to compute LCS string: " << ((float) dt2 / (1e6 * repeats)) << " s (average per op)" << std::endl;

    // Dump final accumulated value to prevent compiler optimising away ops
    std::cout << dummy_val << std::endl;

    exit(EXIT_SUCCESS);
}
