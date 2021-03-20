
#include "huffman.hpp"

#include <queue>
#include <stdexcept>

namespace huffman {

bool code_vnode_t::is_leaf() const {
    return left < 0 && right < 0;
}

bool code_vnode_t::encodes(char c) const {
    return symbols.find(c) != std::string::npos;
}

const code_vnode_t* code_table_t::root() const {
    return nodes_.empty() ? nullptr : &nodes_.back();
}

const code_vnode_t* code_table_t::left(const code_vnode_t* node) const {
    return node->left < 0 ? nullptr : &nodes_[node->left];
}

const code_vnode_t* code_table_t::right(const code_vnode_t* node) const {
    return node->right < 0 ? nullptr : &nodes_[node->right];
}

std::ostream& operator << (std::ostream& ostr, const code_table_t& code_table) {
    constexpr auto indent_spaces = 2;
    auto level = 0;
    auto indent = [&] {
        return std::string(level * indent_spaces, ' ');
    };
    // Use `printer` to call `print_table` recrusively.
    auto print_node = [&](const auto* node, auto& printer) -> void {
        if (!node) return;
        ++level;
        ostr << indent() << "{" << node->symbols << "} " << node->weight << "\n";
        printer(code_table.left(node), printer);
        printer(code_table.right(node), printer);
        --level;
    };
    print_node(code_table.root(), print_node);
    return ostr;
}

code_table_t build(std::vector<symbol_def_t> alphabet) {
    std::vector<code_vnode_t> nodes;
    nodes.reserve(2 * alphabet.size());
    auto compare = [&](const int& left, const int& right) {
        return nodes[left].weight > nodes[right].weight;
    };
    std::priority_queue<int, std::vector<int>, decltype(compare)> queue(compare);
    for (const auto& freq : alphabet) {
        nodes.push_back({ std::string(1, freq.symbol), freq.weight, -1, -1});
        queue.push(nodes.size() - 1);
    }
    while (queue.size() > 1) {
        const auto left = queue.top(); queue.pop();
        const auto right = queue.top(); queue.pop();
        const auto symbols = nodes[left].symbols + " " + nodes[right].symbols;
        const auto weight = nodes[left].weight + nodes[right].weight;
        nodes.push_back({ symbols, weight, left, right });
        queue.push(nodes.size() - 1);
    }
    nodes.resize(nodes.size());
    return code_table_t{ std::move(nodes) };
}

bit_vect_t encode(const code_table_t& code_table, std::string_view text) {
    bit_vect_t output;
    for (const char c : text) {
        const code_vnode_t* current = code_table.root();
        while (current) {
            if (current->is_leaf()) {
                //output.push_back(' ');
                current = nullptr;
            }
            else if (code_table.left(current)->encodes(c)) {
                output.push_back('0');
                current = code_table.left(current);
            }
            else if (code_table.right(current)->encodes(c)) {
                output.push_back('1');
                current = code_table.right(current);
            }
            else
                throw std::runtime_error("invalid alphabet");
        }
    }
    return output;
}

std::string decode(const code_table_t& code_table, bit_vect_t message) {
    std::string output {};
    const code_vnode_t* current = code_table.root();
    for (const char c : message) {
        if (c == '0') {
            current = code_table.left(current);
        }
        else if (c == '1') {
            current = code_table.right(current);
        }
        else if (c == ' ') { // skip
        }
        else
            throw std::runtime_error("invalid message");
        if (current->is_leaf()) {
            output.push_back(current->symbols[0]);
            current = code_table.root();
        }
    }
    return output;
}

} // namespace huffman

int main() {
    std::cout << "Huffman encoding\n";

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

    const std::string text = "ABABCEDEDFGH";

    const auto encoded = huffman::encode(code_table, text);
    std::cout << encoded << "\n";

    const auto decoded = huffman::decode(code_table, encoded);
    std::cout << decoded << "\n";

    std::cout << (decoded == text ? "OK" : "FAILED") << "\n";

    return 0;
}
