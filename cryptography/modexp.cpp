#include <iostream>

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
 * @brief Compute value of (x^d) mod n
 * */
long modular_exponentiation(long x, long d, long n) {
    if (d == 0) {
        return 1;
    }
    if (d % 2 == 0) { // d is positive and even
        long z = modular_exponentiation(x, d / 2, n);
        return (z * z) % n;
    } else { // d is positive and odd
        long z = modular_exponentiation(x, (d - 1) / 2, n);
        return (z * z * x) % n;
    }
}

int main(int argc, char *argv[]) {
    // Check correct usage (e.g. 'modexp 259 269 493')
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [integer_x] [integer_d] [integer_n]" << std::endl;
        exit(EXIT_FAILURE);
    }

    long integer_x = get_long_param(argv[1]);
    long integer_d = get_long_param(argv[2]);
    long integer_n = get_long_param(argv[3]);

    auto z = modular_exponentiation(integer_x, integer_d, integer_n);

    std::cout << "z: " << z << std::endl;

    exit(EXIT_SUCCESS);
}
