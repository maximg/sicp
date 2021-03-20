
#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/// Huffman code builder, endocder and decoder.

namespace huffman {

template <typename T>
struct node_t {
    using ptr = std::unique_ptr<node_t>;

    T data;
    ptr left = nullptr;
    ptr right = nullptr;

    node_t(const T& data_p, ptr&& left_p, ptr&& right_p)
        : data(data_p), left(std::move(left_p)), right(std::move(right_p)) {}

    node_t(const node_t&) = delete;
    node_t& operator=(const node_t&) = delete;

    static ptr make_node_ptr(const T& data_p, ptr&& left_p, ptr&& right_p) {
        return std::make_unique<node_t<T>>(data_p, std::move(left_p), std::move(right_p));
    }
};

template <typename T>
std::ostream& operator << (std::ostream& ostr, const node_t<T>& root);

struct code_node_t {
    std::string symbols {};
    unsigned weight {0};
};

std::ostream& operator << (std::ostream& ostr, const code_node_t& code_node);

using code_table_t = node_t<code_node_t>;

struct symbol_freq_t {
    char c;
    int freq;
};

struct bit_vect_t {}; // TODO

code_table_t build(std::vector<symbol_freq_t> frequencies);
std::string decode(const code_table_t& code_table, bit_vect_t message);
bit_vect_t encode(const code_table_t& code_table, std::string_view text);

} // namespace huffman
