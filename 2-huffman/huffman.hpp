
#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/// Huffman code builder, endocder and decoder.

namespace huffman {

struct code_table_t {
    using code_table_ptr = std::unique_ptr<code_table_t>;

    std::string symbols {};
    unsigned weight;
    code_table_ptr left = nullptr;
    code_table_ptr right = nullptr;

    code_table_t(const std::string& symbols_p, unsigned weight_p, code_table_ptr&& left_p, code_table_ptr&& right_p)
        : symbols(symbols_p), weight(weight_p), left(std::move(left_p)), right(std::move(right_p)) {}
    code_table_t(const std::string& symbols_p, unsigned weight_p)
        : symbols(symbols_p), weight(weight_p) {}

    code_table_t(const code_table_t&) = delete;
    code_table_t& operator=(const code_table_t&) = delete;
};

std::ostream& operator << (std::ostream& ostr, const huffman::code_table_t& code_table);

struct symbol_freq_t {
    char c;
    int freq;
};

struct bit_vect_t {}; // TODO

code_table_t build(std::vector<symbol_freq_t> frequencies);
std::string decode(const code_table_t& code_table, bit_vect_t message);
bit_vect_t encode(const code_table_t& code_table, std::string_view text);

} // namespace huffman
