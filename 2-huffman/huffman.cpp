
#include "huffman.hpp"

#include <iostream>

namespace huffman {

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

std::ostream& operator << (std::ostream& ostr, const code_node_t& code_node) {
    return ostr << "{" << code_node.symbols << "} " << code_node.weight;
}

code_table_t build(std::vector<symbol_freq_t> frequencies) {
    // TODO
    return {
        { "A B C", 3 },
        code_table_t::make_node_ptr({ "B", 1 }, nullptr, nullptr),
        code_table_t::make_node_ptr({ "C", 1 }, nullptr, nullptr)
    };
}


} // namespace huffman

int main() {
    std::cout << "Huffman demo\n";

    auto code_table = huffman::build({
        {'A', 8},
        {'B', 3},
        {'C', 1},
        {'D', 1},
        {'E', 1},
        {'F', 1},
        {'G', 1},
        {'H', 1}
    });

    std::cout << code_table << "\n";

    return 0;
}
