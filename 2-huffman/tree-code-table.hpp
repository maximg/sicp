
#pragma once

#include <iostream>
#include <memory>
#include <string>

/// Huffman code builder, endocder and decoder.
/// Tree form; abandoned.

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

template <typename T>
std::ostream& operator << (std::ostream& ostr, const node_t<T>& root) {
    constexpr auto indent_spaces = 2;
    auto level = 0;
    auto indent = [&] {
        return std::string(level * indent_spaces, ' ');
    };
    // Use `printer` to call `print_table` recrusively.
    auto print_node = [&](const auto& node, auto& printer) -> void {
        ++level;
        ostr << indent() << node.data << "\n";
        if (node.left) printer(*node.left, printer);
        if (node.right) printer(*node.right, printer);
        --level;
    };
    print_node(root, print_node);
    return ostr;
}

inline
std::ostream& operator << (std::ostream& ostr, const code_node_t& code_node) {
    return ostr << "{" << code_node.symbols << "} " << code_node.weight;
}

} // namespace huffman
