
#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/// Huffman code builder, endocder and decoder.
/// Code table is a vector, linked via indices.

namespace huffman {

class code_table_t {
public:
    struct node_t {
        std::string symbols {};
        unsigned weight {};
        int left {-1};
        int right {-1};

        bool is_leaf() const;
        bool encodes(char c) const;
    };

    explicit code_table_t(std::vector<node_t>&& nodes) : nodes_(std::move(nodes)) {}

    const node_t* root() const;
    const node_t* left(const node_t* node) const;
    const node_t* right(const node_t* node) const;

    void walk(std::function<void(const node_t&, unsigned /*level*/)> node_fn) const;

private:
    std::vector<node_t> nodes_;
};

std::ostream& operator << (std::ostream& ostr, const code_table_t& code_table);

struct symbol_def_t {
    char symbol;
    unsigned weight;
};

/// TODO: change to a more efficient implementation.
using bit_vect_t = std::string;

code_table_t build(const std::vector<symbol_def_t>& alphabet);
std::string decode(const code_table_t& code_table, bit_vect_t message);
bit_vect_t encode(const code_table_t& code_table, std::string_view text);

} // namespace huffman
