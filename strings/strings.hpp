#ifndef STRINGS_STRINGS_HPP
#define STRINGS_STRINGS_HPP

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

// Overload << operator to allow natural printing of LCS table
std::ostream &operator<<(std::ostream &out, LCSTable const &t);

/*
 * @brief Recursively assemble an LCS string from a pre-computed LCS table
 *
 * @param[in]  t  instance of an LCSTable precomputed from two strings
 * @param[in]  i  row index into t (and the original X, Y strings)
 * @param[in]  j  column index into t (and the original X, Y strings)
 * @return  string representing the LCS, for the given i,j indices
 * */
std::string assemble_lcs(LCSTable const &t, int i, int j);

/*
 * @brief Parse argument to extract user string length
 *
 * @param[in]  param  argv element corresponding to string length
 * @return  (int)string_length  parsed size of array, casted to int
 * */
int get_string_length(char *param);

/*
 * @brief Parse argument to extract user number of repeats
 *
 * @param[in]  param  argv element corresponding to array size
 * @return  (int)repeat_count  parsed repeat count, casted to int
 * */
int get_repeat_count(char *param);

#endif //STRINGS_STRINGS_HPP
