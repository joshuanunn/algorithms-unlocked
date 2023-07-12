#include <chrono>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <unordered_map>
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
 * @brief Class to encapsulate transform tables computed for two strings.
 *
 * Example from the book for the strings X and Y give the combined cost and op tables below.
 *
 *   X = "ACAAGC"
 *   Y = "CCGT"
 *
 * cost/op table form:
 *
 *         A  C  G  T
 *    0    1  0  0  0
 *    1    1  2  0  0
 *    2    3  0  0  0
 *    3    1  4  0  0
 *    4    5  0  0  0
 *    5    1  4  6  0
 *    6    7  0  0  0
 *    7    1  2  0  0
 * */
struct StateTable {
    int height; // number of states
    int width; // number of unique chars
    std::string pattern;
    int pattern_length;
    std::vector<int> next_state;
    std::unordered_map<char, int> char_index;
    std::unordered_map<int, char> index_char;

    // Constructor makes a copy of the pattern string, computes the height and width, and initialises the next_state vector.
    StateTable(std::string const &T, std::string const &P);

    // Convert between 2-dim (row,column) index and the internal 1-dim vector index.
    int coord(int i, int j) const;

    // Convert between 2-dim (state,character) index and the internal 1-dim vector index.
    int get_next_state(int prev_state, char prev_char);

    // Construct the next_state table. The 1-dim vector represents the 2-dim table of height (pattern states) and width
    // (unique characters of the input text T).
    void compute_state_table();
};

StateTable::StateTable(std::string const &T, std::string const &P) {
    // Maintain an internal copy of the supplied string pattern P
    pattern = P;
    pattern_length = P.size();

    // Determine the number of unique characters in the search string S
    int unique_index = 0;
    for (const auto &c: T) {
        const auto [_, success] = char_index.insert({c, unique_index});
        if (success) {
            index_char.insert({unique_index, c});
            unique_index ++;
        }
    }

    // Table dimensions
    height = P.size() + 1; // number of states == length of pattern + 1
    width = unique_index; // number of unique characters in input text T

    // Initialise 1-dim vectors of length [height x width] to represent the 2-dim nextstate table
    next_state = std::vector<int>(height * width);

    // Populate next_state table for the provided text T and pattern P
    StateTable::compute_state_table();
}

int StateTable::coord(int i, int j) const {
    return j + i * width;
}

void StateTable::compute_state_table() {
    int i;
    int pka_length;

    // Loop over states and unique characters to populate the next_state table
    for (int state = 0; state < height; state++) { // states
        for (const auto& [key, value]: char_index) { // unique chars

            auto Pk = pattern.substr(0, state);
            auto Pka = Pk + key;
            pka_length = Pka.size();
            i = std::min(pka_length, pattern_length);

            // Loop while Pi (first i chars of pattern) is not a suffix of Pka
            while (i > 0) {
                if (pattern.substr(0, i) == Pka.substr(pka_length - i, i)) {
                    break;
                };
                i--;
            }
            // Update next_state table with the next state for this state-character combination
            next_state[coord(state, value)] = i;
        }
    }
}

int StateTable::get_next_state(int prev_state, char prev_char) {
    int col = char_index.at(prev_char);
    return next_state[coord(prev_state, col)];
}

// Overload << operator to allow natural printing of nextstate table
std::ostream &operator<<(std::ostream &out, StateTable const &t) {
    out << "      ";
    for (int j = 0; j < t.width; j++) {
        out << std::setw(4) << std::setfill(' ') << t.index_char.at(j);
    }
    out << std::endl;

    for (int i = 0; i < t.height; i++) {
        out << std::setw(4) << std::setfill(' ') << i << " |";
        for (int j = 0; j < t.width; j++) {
            out << std::setw(4) << std::setfill(' ') << t.next_state[t.coord(i, j)];
        }
        out << std::endl;
    }
    return out;
}

/*
 * @brief Find all occurrences of the substring pattern in the full text T, using a pre-computed StateTable
 *
 * @param[in]  T  full text string T, previously used to compute the StateTable t
 * @param[in]  t  instance of an StateTable precomputed from the text string T and a substring pattern P
 * @return  vector of integer shifts to representing the relative position of the substring from the start of T
 * */
std::vector<int> fa_string_matcher(std::string const &T, StateTable &t) {
    std::vector<int> shifts;
    int state = 0;

    for (int i = 0; i < T.size(); i++) {
        state = t.get_next_state(state, T[i]);
        if (state == t.pattern_length) {
            shifts.push_back((i + 1) - t.pattern_length);
        }
    }
    return shifts;
}

int get_random_index(int index_max) {
    std::random_device rd;
    std::mt19937 generator(rd());
    return generator() % index_max;
}

int main(int argc, char *argv[]) {
    // Check correct usage (e.g. 'strings 1000 5')
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [string_length] [pattern_length] [repeat_count]" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::chrono::time_point <std::chrono::steady_clock> t1, t2, t3;
    long dt1, dt2;
    std::string T, P;
    std::vector<int> shifts;
    int dummy_val = 0; // Use for accumulation to prevent compiler optimising away ops

    int string_length = get_string_length(argv[1]);
    int pattern_length = get_string_length(argv[2]);
    int repeats = get_repeat_count(argv[3]);

    // Make sure that pattern length <= string length
    if (pattern_length > string_length) {
        std::cerr << "Error: pattern_length must be shorter or equal to string_length" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Example strings T and P from the book
    //T = "GTAACAGTAAACG";
    //P = "AAC";

    // Create random alphanumeric text string T of user specified length
    T = generate_random_alphanumeric_string(string_length);

    // Set P to be a substring of T of a user defined length with a random start position to guarantee a match
    int start_index = get_random_index(T.size() - 1 - pattern_length);
    P = T.substr(start_index, pattern_length);

    dt1 = 0;
    dt2 = 0;

    for (int i = 0; i < repeats; i++) {

        t1 = std::chrono::steady_clock::now();

        // Construct next_state table
        auto state_table = StateTable(T, P);
        t2 = std::chrono::steady_clock::now();

        // Find and report substring matches
        shifts = fa_string_matcher(T, state_table);

        t3 = std::chrono::steady_clock::now();

        // Accumulate measurement time
        dt1 += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        dt2 += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();

        // Print intermediate Transform table and Transformed string Z for debugging
        //std::cout << state_table << std::endl;
        std::cout << "The pattern occurs with shifts: [";
        for (const auto& i: shifts) {std::cout << " " << i;}
        std::cout << "]" << std::endl;

        // Use values in the sorted array to prevent the compiler optimising away ops
        dummy_val += shifts.size();
    }
    std::cout << "Time to compute State table: " << ((float) dt1 / (1e6 * repeats)) << " s (average per op)" << std::endl;
    std::cout << "Time to find substring matches: " << ((float) dt2 / (1e6 * repeats)) << " s (average per op)" << std::endl;

    // Dump final accumulated value to prevent compiler optimising away ops
    std::cout << dummy_val << std::endl;

    exit(EXIT_SUCCESS);
}
