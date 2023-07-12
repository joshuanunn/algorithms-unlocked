#include <iostream>
#include <tuple>

/*
 * @brief Parse argument to extract user long integer value
 *
 * @param[in]  param  argv element corresponding to integer
 * @return  param converted to long int
 * */
long get_long_param(char *param) {
    char *endptr;
    long long_param;

    errno = 0;
    long_param = std::strtol(param, &endptr, 10);

    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == param) {
        std::cerr << "could not parse long_param as an integer" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (long_param < 0) {
        std::cerr << "integer parameters must be >= 0" << std::endl;
        exit(EXIT_FAILURE);
    }

    return long_param;
}

/*
 * @brief Compute the greatest common divisor (g) of two integers (a, b)
 *
 * @return  tuple of {g, i, j}, where g = ai + bj
 * */
std::tuple<long, long, long> euclid(long a, long b) {
    if (b == 0) {
        return {a, 1, 0};
    }

    auto [g, i_, j_] = euclid(b, a % b);
    auto i = j_;
    auto j = i_ - (a / b) * j_;

    return {g, i, j};
}

int main(int argc, char *argv[]) {
    // Check correct usage (e.g. 'euclid 30 18')
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [integer_a] [integer_b]" << std::endl;
        exit(EXIT_FAILURE);
    }

    long integer_a = get_long_param(argv[1]);
    long integer_b = get_long_param(argv[2]);

    auto [gcd, i, j] = euclid(integer_a, integer_b);

    std::cout << "gcd: " << gcd << " i: " << i << " j: " << j << std::endl;

    exit(EXIT_SUCCESS);
}
