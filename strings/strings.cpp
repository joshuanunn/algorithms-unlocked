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

TransformTable::TransformTable(std::string const &str_x, std::string const &str_y, int cC, int cR, int cD, int cI) {
    // Maintain an internal copy of the original strings used to calculate the transform tables, and costs
    X = str_x;
    Y = str_y;
    cc = cC;
    cr = cR;
    cd = cD;
    ci = cI;

    // Height and width of cost and op tables include the first ins row and del column
    height = X.size() + 1;
    width = Y.size() + 1;

    // Initialise 1-dim vectors of length [height x width] to represent the 2-dim transform tables
    cost = std::vector<int>(height * width);
    op = std::vector<Operation>(height * width);

    // Populate cost and op tables for the provided strings
    TransformTable::compute_transform_tables();
}

int TransformTable::coord(int i, int j) const {
    return j + i * width;
}

void TransformTable::compute_transform_tables() {
    cost[coord(0,0)] = 0;
    op[coord(0,0)] = Operation{op_type::NOOP, '-'};

    // Create left column of delete operations
    for (int i = 1; i < height; i++) {
        cost[coord(i, 0)] = i * cd;
        op[coord(i, 0)] = Operation{op_type::DELETE, X[i - 1]};
    }

    // Create top row of insert operations
    for (int j = 1; j < width; j++) {
        cost[coord(0, j)] = j * ci;
        op[coord(0, j)] = Operation{op_type::INSERT, Y[j - 1]};
    }

    // Compute values for inner cost and op tables by determining which operation applies to minimise cost
    for (int i = 1; i < height; i++) {
        for (int j = 1; j < width; j++) {
            // Possible operation i
            if (X[i - 1] == Y[j - 1]) {
                cost[coord(i, j)] = cost[coord(i - 1, j - 1)] + cc;
                op[coord(i, j)] = Operation{op_type::COPY, Y[j - 1]};
            } else {
                cost[coord(i, j)] = cost[coord(i - 1, j - 1)] + cr;
                op[coord(i, j)] = Operation{op_type::REPLACE, Y[j - 1]};
            }
            // Possible operation ii
            if ((cost[coord(i - 1, j)] + cd) < cost[coord(i, j)]) {
                cost[coord(i, j)] = cost[coord(i - 1, j)] + cd;
                op[coord(i, j)] = Operation{op_type::DELETE, X[i - 1]};
            }
            // Possible operation iii
            if ((cost[coord(i, j - 1)] + ci) < cost[coord(i, j)]) {
                cost[coord(i, j)] = cost[coord(i, j - 1)] + ci;
                op[coord(i, j)] = Operation{op_type::INSERT, Y[j - 1]};
            }
        }
    }
}

std::ostream &operator<<(std::ostream &out, TransformTable const &t) {
    std::string op_element;

    for (int i = 0; i < t.height; i++) {
        for (int j = 0; j < t.width; j++) {
            switch(t.op[t.coord(i, j)].type) {
                case op_type::COPY:
                    op_element = " cpy:";
                    break;
                case op_type::REPLACE:
                    op_element = " rep:";
                    break;
                case op_type::INSERT:
                    op_element = " ins:";
                    break;
                case op_type::DELETE:
                    op_element = " del:";
                    break;
                case op_type::NOOP:
                    op_element = " ---:";
                    break;
                default:
                    op_element = "  ";
            }
            op_element += t.op[t.coord(i, j)].apply_on;
            out << std::setw(7) << std::setfill(' ') << t.cost[t.coord(i, j)] << op_element;
        }
        out << std::endl;
    }
    return out;
}

std::vector<Operation> assemble_transformation(TransformTable const &t, int i, int j) {
    std::vector<Operation> op_vector;

    // Base case where op is a no-op, return empty string
    if (t.op[t.coord(i, j)].type == op_type::NOOP) {
        op_vector.push_back(Operation{op_type::NOOP, '-'});
        return op_vector;
    // If current op is a copy or replace -- recursively return substring and append matching char
    } else if ((t.op[t.coord(i, j)].type == op_type::COPY) || (t.op[t.coord(i, j)].type == op_type::REPLACE)) {
        op_vector = assemble_transformation(t, i - 1, j - 1);
        op_vector.push_back(t.op[t.coord(i, j)]);
        return op_vector;
    // If op is delete -- recursively work back up column
    } else if (t.op[t.coord(i, j)].type == op_type::DELETE) {
        op_vector = assemble_transformation(t, i - 1, j);
        op_vector.push_back(t.op[t.coord(i, j)]);
        return op_vector;
    // Op must be an insert -- recursively work back along row
    } else {
        op_vector = assemble_transformation(t, i, j - 1);
        op_vector.push_back(t.op[t.coord(i, j)]);
        return op_vector;
    }
}

std::string apply_transformation(std::string const &str_x, std::vector<Operation> const &op_vector) {
    std::string  Z = "";
    int pos = 0;

    for (auto op = op_vector.rbegin(); op != op_vector.rend(); op++) {
        switch(op->type) {
            case op_type::COPY:
                Z.push_back(str_x[pos++]);
                break;
            case op_type::REPLACE:
                Z.push_back(op->apply_on);
                pos++;
                break;
            case op_type::INSERT:
                Z.push_back(op->apply_on);
                break;
            case op_type::DELETE:
                pos++;
                break;
            case op_type::NOOP:
                break;
        }
    }
    return Z;
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
    std::string lcs, X, Y, Z;
    int dummy_val = 0; // Use for accumulation to prevent compiler optimising away ops

    int string_length = get_string_length(argv[1]);
    int repeats = get_repeat_count(argv[2]);

    // LCS Table

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

    // Transform Tables

    // Example strings X and Y from the book
    //X = "ACAAGC";
    //Y = "CCGT";

    // Create random alphanumeric strings of user specified length
    X = generate_random_alphanumeric_string(string_length);
    Y = generate_random_alphanumeric_string(string_length);

    dt1 = 0;
    dt2 = 0;

    // Define operation costs as provided in the book
    int cost_copy = -1;
    int cost_replace = 1;
    int cost_delete = 2;
    int cost_insert = 2;

    for (int i = 0; i < repeats; i++) {

        t1 = std::chrono::steady_clock::now();

        // Construct transform table
        auto transform_table = TransformTable(X, Y, cost_copy, cost_replace, cost_delete, cost_insert);
        t2 = std::chrono::steady_clock::now();

        // Calculate Transformed string
        auto transform_operations = assemble_transformation(transform_table, X.size(), Y.size());
        Z = apply_transformation(X, transform_operations);

        t3 = std::chrono::steady_clock::now();

        // Accumulate measurement time
        dt1 += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        dt2 += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();

        // Print intermediate Transform table and Transformed string Z for debugging
        //std::cout << transform_table << std::endl;
        //std::cout << Z << std::endl;

        // Check that transformed string Z from X, matches the target Y
        if (!Z.compare(Y)) {
            std::cerr << "Error: transformed string Z does not match target Y:" << std::endl <<
                "X: " << X << std::endl << "Y: " << Y << std::endl << "Z: " << Z << std::endl;
            exit(EXIT_FAILURE);
        }

        // Use values in the sorted array to prevent the compiler optimising away ops
        dummy_val += Z[(100 * i) % string_length];
    }

    // Print final Transformed string
    //std::cout << "Transformed string (Z): " << Z << std::endl;
    std::cout << "Time to compute Transform tables: " << ((float) dt1 / (1e6 * repeats)) << " s (average per op)" << std::endl;
    std::cout << "Time to compute Transformed string: " << ((float) dt2 / (1e6 * repeats)) << " s (average per op)" << std::endl;

    // Dump final accumulated value to prevent compiler optimising away ops
    std::cout << dummy_val << std::endl;

    exit(EXIT_SUCCESS);
}
