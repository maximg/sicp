
#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/// Huffman code builder, endocder and decoder.
/// Code table is a vector, linked via indices.

namespace huffman {

struct code_vnode_t {
    std::string symbols {};
    unsigned weight {};
    int left {-1};
    int right {-1};
};

class code_table_t {
public:
    explicit code_table_t(std::vector<code_vnode_t>&& nodes) : nodes_(std::move(nodes)) {}

    const code_vnode_t* root() const;
    const code_vnode_t* left(const code_vnode_t* node) const;
    const code_vnode_t* right(const code_vnode_t* node) const;
    bool is_leaf(const code_vnode_t* node) const;

private:
    std::vector<code_vnode_t> nodes_;
};

std::ostream& operator << (std::ostream& ostr, const code_table_t& code_table);

struct symbol_def_t {
    char symbol;
    unsigned weight;
};

struct bit_vect_t {}; // TODO

code_table_t build(std::vector<symbol_def_t> alphabet);
std::string decode(const code_table_t& code_table, bit_vect_t message);
bit_vect_t encode(const code_table_t& code_table, std::string_view text);

} // namespace huffman
