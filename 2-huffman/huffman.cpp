
#include "huffman.hpp"

#include <iostream>

namespace huffman {

code_table_t build(std::vector<symbol_freq_t> frequencies) {
    // TODO
    return {
        "A B C", 3,
        std::make_unique<code_table_t>("B", 1, nullptr, nullptr),
        std::make_unique<code_table_t>("C", 1, nullptr, nullptr),
    };
}

std::ostream& operator << (std::ostream& ostr, const huffman::code_table_t& code_table) {
    constexpr auto indent_spaces = 2;
    auto level = 0;
    auto indent = [&] {
        return std::string(level * indent_spaces, ' ');
    };
    // Use `printer` to call `print_table` recrusively.
    auto print_table = [&](const huffman::code_table_t& node, auto& printer) -> void {
        ++level;
        ostr << indent() << "{" << node.symbols << "} " << node.weight << "\n";
        if (node.left) printer(*node.left, printer);
        if (node.right) printer(*node.right, printer);
        --level;
    };
    print_table(code_table, print_table);
    return ostr;
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
